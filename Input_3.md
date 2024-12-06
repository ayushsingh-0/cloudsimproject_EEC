machine class:
{
# GPU-Enhanced Task Scenario
        Number of machines: 12
        CPU type: X86
        Number of cores: 8
        Memory: 16384
        S-States: [120, 100, 80, 60, 30, 15, 0]
        P-States: [10, 8, 6, 4]
        C-States: [10, 3, 1, 0]
        MIPS: [1000, 900, 700, 500]
        GPUs: yes
}
task class:
{
        Start time: 60000
        End time: 1500000
        Inter arrival: 10000
        Expected runtime: 2500000
        Memory: 12
        VM type: LINUX
        GPU enabled: yes
        SLA type: SLA0
        CPU type: X86
        Task type: AI
        Seed: 789123
}
