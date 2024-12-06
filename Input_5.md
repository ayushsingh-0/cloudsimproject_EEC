machine class:
{
# High Priority SLA Compliance Scenario
        Number of machines: 18
        CPU type: POWER
        Number of cores: 12
        Memory: 24576
        S-States: [140, 110, 90, 70, 40, 15, 0]
        P-States: [14, 11, 8, 5]
        C-States: [14, 5, 2, 0]
        MIPS: [1100, 950, 700, 500]
        GPUs: yes
}
task class:
{
        Start time: 75000
        End time: 1200000
        Inter arrival: 5000
        Expected runtime: 2200000
        Memory: 10
        VM type: LINUX_RT
        GPU enabled: no
        SLA type: SLA0
        CPU type: POWER
        Task type: HPC
        Seed: 112233
}
