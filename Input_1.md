machine class:
{
# High Workload Surge Scenario
        Number of machines: 20
        CPU type: X86
        Number of cores: 16
        Memory: 32768
        S-States: [150, 120, 100, 90, 60, 20, 0]
        P-States: [15, 10, 7, 5]
        C-States: [15, 5, 2, 0]
        MIPS: [1200, 1000, 700, 500]
        GPUs: yes
}
task class:
{
        Start time: 50000
        End time : 1000000
        Inter arrival: 3000
        Expected runtime: 1500000
        Memory: 16
        VM type: LINUX_RT
        GPU enabled: yes
        SLA type: SLA1
        CPU type: X86
        Task type: HPC
        Seed: 123456
}
