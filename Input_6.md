machine class:
{
# Overloaded Memory Scenario
        Number of machines: 8
        CPU type: X86
        Number of cores: 4
        Memory: 4096
        S-States: [100, 80, 60, 50, 20, 5, 0]
        P-States: [10, 7, 5, 3]
        C-States: [10, 4, 1, 0]
        MIPS: [800, 600, 400, 200]
        GPUs: no
}
task class:
{
        Start time: 30000
        End time: 500000
        Inter arrival: 1000
        Expected runtime: 1000000
        Memory: 16
        VM type: LINUX
        GPU enabled: no
        SLA type: SLA1
        CPU type: X86
        Task type: WEB
        Seed: 445566
}
