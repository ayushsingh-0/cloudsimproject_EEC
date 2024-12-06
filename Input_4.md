machine class:
{
# Heterogeneous Environment Scenario
        Number of machines: 15
        CPU type: RISCV
        Number of cores: 6
        Memory: 12288
        S-States: [110, 90, 70, 50, 25, 10, 0]
        P-States: [9, 7, 5, 3]
        C-States: [9, 4, 1, 0]
        MIPS: [900, 700, 500, 300]
        GPUs: no
}
task class:
{
        Start time: 50000
        End time: 1000000
        Inter arrival: 8000
        Expected runtime: 1800000
        Memory: 8
        VM type: AIX
        GPU enabled: no
        SLA type: SLA2
        CPU type: RISCV
        Task type: CRYPTO
        Seed: 987654
}
