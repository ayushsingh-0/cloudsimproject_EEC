//
//  Scheduler.cpp
//  CloudSim
//
//  Created by ELMOOTAZBELLAH ELNOZAHY on 10/20/24.
//

#include "Scheduler.hpp"
#include <unordered_map>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include <stdexcept>

// Static variables for metadata
static Scheduler scheduler;
static unsigned active_machines = 0;
static std::unordered_map<MachineId_t, std::vector<VMId_t>> machine_to_vms; 
static std::unordered_map<MachineId_t, std::vector<TaskId_t>> migrating_tasks;
static std::unordered_map<VMId_t, std::pair<MachineId_t, MachineId_t>> migrating_vms;

static Priority_t sla_to_priority(SLAType_t sla);

// Utility functions
static bool cpu_compatible(MachineId_t machine_id, TaskId_t task_id);
static bool memory_fits(MachineId_t machine_id, TaskId_t task_id);
static bool gpu_compatible(MachineId_t machine_id, TaskId_t task_id);

/**
 * Helper: Shuts down machines with no active tasks or VMs to save energy.
 * Accesses machines using the getter function.
 */
static void turn_off_unused_machines(const Scheduler& scheduler) {
    const std::vector<MachineId_t>& machines = scheduler.getMachines();
    for (MachineId_t machine_id : machines) {
        MachineInfo_t machine_info = Machine_GetInfo(machine_id);

        // Check if the machine is idle
        if (machine_info.active_tasks == 0 && machine_info.active_vms == 0) {
            Machine_SetState(machine_id, S4); // Deactivate machine
            active_machines--;

            // Optional logging for debugging
            //SimOutput("Machine " + std::to_string(machine_id) + " turned off (S4)", 3);
        }
    }
}

/**
 * Initialize    
 */
void Scheduler::Init() {
    // SimOutput("Scheduler::Init(): Initializing scheduler", 2);

    for (unsigned i = 0; i < Machine_GetTotal(); i++) {
        MachineId_t machine_id = MachineId_t(i);
        machines.push_back(machine_id);
        machine_to_vms[machine_id] = {}; // Initialize with no VMs
        migrating_tasks[machine_id] = {};
        Machine_SetState(machine_id, S0); // Activate all machines initially
        active_machines++;
    }

    // SimOutput("Scheduler::Init(): Initialization complete", 2);
}

/*
    Schedule new tasks
*/
void Scheduler::NewTask(Time_t now, TaskId_t task_id) {
    TaskInfo_t task_info = GetTaskInfo(task_id);
    MachineId_t selected_machine = 0xDEADBEEF;
    VMId_t selected_vm = 0xDEADBEEF;

    // Find the first compatible machine
    for (MachineId_t machine : getMachines()) {
        if (cpu_compatible(machine, task_id) && memory_fits(machine, task_id) && gpu_compatible(machine, task_id)) {
            selected_machine = machine;
            break;
        }
    }

    // Activate a new machine if none found
    if (selected_machine == 0xDEADBEEF) {
        for (unsigned i = active_machines; i < Machine_GetTotal(); i++) {
            Machine_SetState(MachineId_t(i), S0); // Activate machine
            active_machines++;
            selected_machine = MachineId_t(i);
            break;
        }
    }

    // Assign task to an existing or new VM
    if (selected_machine != 0xDEADBEEF) {
        for (VMId_t vm : machine_to_vms[selected_machine]) {
            VMInfo_t vm_info = VM_GetInfo(vm);
            if (vm_info.vm_type == task_info.required_vm && vm_info.cpu == task_info.required_cpu) {
                selected_vm = vm;
                break;
            }
        }
        if (selected_vm == 0xDEADBEEF) {
            selected_vm = VM_Create(task_info.required_vm, Machine_GetCPUType(selected_machine));
            VM_Attach(selected_vm, selected_machine);
            machine_to_vms[selected_machine].push_back(selected_vm);
        }
        VM_AddTask(selected_vm, task_id, sla_to_priority(task_info.required_sla));
    } else {
        SimOutput("NewTask(): No resources available for task " + std::to_string(task_id), 0);
    }

    turn_off_unused_machines(*this); // Optimize energy
}

/*
    Task Completion
*/
void Scheduler::TaskComplete(Time_t now, TaskId_t task_id) {
    SimOutput("TaskComplete(): Task " + std::to_string(task_id) + " completed at " + std::to_string(now), 3);
    turn_off_unused_machines(*this); // Reevaluate unused machines
}

/*
    SLA Violation Handling
*/
void SLAWarning(Time_t time, TaskId_t task_id) {
    TaskInfo_t task_info = GetTaskInfo(task_id);
    MachineId_t dest_machine = 0xDEADBEEF;

    for (MachineId_t machine : scheduler.getMachines()) {
        if (cpu_compatible(machine, task_id) && memory_fits(machine, task_id)) {
            dest_machine = machine;
            break;
        }
    }

    if (dest_machine != 0xDEADBEEF) {
        VMId_t new_vm = VM_Create(task_info.required_vm, Machine_GetCPUType(dest_machine));
        VM_Attach(new_vm, dest_machine);
        machine_to_vms[dest_machine].push_back(new_vm);
        VM_AddTask(new_vm, task_id, HIGH_PRIORITY);
    } else {
        SimOutput("SLAWarning(): No resources available to resolve SLA violation for task " + std::to_string(task_id), 0);
    }
}

/*
    Periodic Check
*/
void Scheduler::PeriodicCheck(Time_t now) {
    SimOutput("PeriodicCheck(): Monitoring system at time " + std::to_string(now), 2);
    turn_off_unused_machines(*this);
}

/* 
    Shutdown
*/
void Scheduler::Shutdown(Time_t time) {
    SimOutput("Shutdown(): Finalizing simulation", 2);
    for (VMId_t vm : vms) {
        VM_Shutdown(vm);
    }
    for (MachineId_t machine : getMachines()) {
        Machine_SetState(machine, S5);
    }
    SimOutput("Shutdown(): Total energy consumed: " + std::to_string(Machine_GetClusterEnergy()) + " KW-Hour", 2);
}

void InitScheduler() {
    SimOutput("InitScheduler(): Initializing scheduler", 4);
    scheduler.Init();
}

void HandleNewTask(Time_t time, TaskId_t task_id) {
    SimOutput("HandleNewTask(): Received new task " + to_string(task_id) + " at time " + to_string(time), 4);
    scheduler.NewTask(time, task_id);
}

void HandleTaskCompletion(Time_t time, TaskId_t task_id) {
    SimOutput("HandleTaskCompletion(): Task " + to_string(task_id) + " completed at time " + to_string(time), 4);
    scheduler.TaskComplete(time, task_id);
}

void SchedulerCheck(Time_t time) {
    SimOutput("SchedulerCheck(): Monitoring system state at " + std::to_string(time), 4);
    scheduler.PeriodicCheck(time);
}

void StateChangeComplete(Time_t time, MachineId_t machine_id) {
    SimOutput("StateChangeComplete(): Machine " + std::to_string(machine_id) + " completed state change at " + std::to_string(time), 4);
    MachineInfo_t machine_info = Machine_GetInfo(machine_id);

    // Reactivate the machine or process tasks waiting for this machine
    if (machine_info.s_state == S0) {
        std::vector<TaskId_t>& tasks_to_process = migrating_tasks[machine_id];
        for (TaskId_t task_id : tasks_to_process) {
            scheduler.NewTask(time, task_id);
        }
        migrating_tasks[machine_id].clear();
    }
}

void MemoryWarning(Time_t time, MachineId_t machine_id) {
    SimOutput("MemoryWarning(): Memory overcommit detected on Machine " + std::to_string(machine_id) + " at " + std::to_string(time), 3);

    // Handle SLA violation for tasks running on the affected machine
    std::vector<VMId_t> vms_on_machine = machine_to_vms[machine_id];
    for (VMId_t vm : vms_on_machine) {
        VMInfo_t vm_info = VM_GetInfo(vm);
        if (!vm_info.active_tasks.empty()) {
            SLAWarning(time, vm_info.active_tasks[0]); // Handle the first affected task
            return;
        }
    }
}

void SimulationComplete(Time_t time) {
    // This function is called before the simulation terminates Add whatever you feel like.
    cout << "SLA violation report" << endl;
    cout << "SLA0: " << GetSLAReport(SLA0) << "%" << endl;
    cout << "SLA1: " << GetSLAReport(SLA1) << "%" << endl;
    cout << "SLA2: " << GetSLAReport(SLA2) << "%" << endl;     // SLA3 do not have SLA violation issues
    cout << "Total Energy " << Machine_GetClusterEnergy() << "KW-Hour" << endl;
    cout << "Simulation run finished in " << double(time)/1000000 << " seconds" << endl;
    SimOutput("SimulationComplete(): Simulation finished at time " + to_string(time), 4);
    
    SimOutput("Shutdown(): Finalizing simulation", 2);
    for (VMId_t vm : scheduler.getVMs()) {
        VM_Shutdown(vm);
    }
    for (MachineId_t machine : scheduler.getMachines()) {
        Machine_SetState(machine, S5);
    }
    SimOutput("Shutdown(): Total energy consumed: " + std::to_string(Machine_GetClusterEnergy()) + " KW-Hour", 2);
}

void MigrationDone(Time_t time, VMId_t vm_id) {
    SimOutput("MigrationDone(): VM " + std::to_string(vm_id) + " completed migration at " + std::to_string(time), 4);

    // Update migration metadata and clear tasks waiting for this VM
    auto src_dest = migrating_vms[vm_id];
    MachineId_t dest_machine = src_dest.second;

    machine_to_vms[dest_machine].push_back(vm_id);
    migrating_vms.erase(vm_id); // Clear migration record
}

// Helpers
static bool cpu_compatible(MachineId_t machine_id, TaskId_t task_id) {
    return Machine_GetCPUType(machine_id) == GetTaskInfo(task_id).required_cpu;
}

static bool memory_fits(MachineId_t machine_id, TaskId_t task_id) {
    MachineInfo_t info = Machine_GetInfo(machine_id);
    return GetTaskMemory(task_id) + info.memory_used <= info.memory_size;
}

static bool gpu_compatible(MachineId_t machine_id, TaskId_t task_id) {
    TaskInfo_t task_info = GetTaskInfo(task_id);
    return !task_info.gpu_capable || Machine_GetInfo(machine_id).gpus;
}

static Priority_t sla_to_priority(SLAType_t sla) {
    switch (sla) {
        case SLA0:
        case SLA1:
            return HIGH_PRIORITY;
        case SLA2:
            return MID_PRIORITY;
        case SLA3:
            return LOW_PRIORITY;
        default:
            return MID_PRIORITY; // Default to medium priority for unknown SLAs
    }
}

// Getters because machines and vms are private
const std::vector<MachineId_t>& Scheduler::getMachines() const {
    return machines; 
}

const std::vector<VMId_t>& Scheduler::getVMs() const {
    return vms;
}