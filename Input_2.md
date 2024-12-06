machine class:
{
# Low-Intensity, Energy-Saving Scenario
        Number of machines: 10
        CPU type: ARM
        Number of cores: 4
        Memory: 8192
        S-States: [90, 70, 50, 40, 20, 5, 0]
        P-States: [8, 6, 4, 2]
        C-States: [8, 2, 1, 0]
        MIPS: [800, 600, 400, 200]
        GPUs: no
}
task class:
{
        Start time: 100000
        End time: 2000000
        Inter arrival: 20000
        Expected runtime: 1000000
        Memory: 4
        VM type: WIN
        GPU enabled: no
        SLA type: SLA3
        CPU type: ARM
        Task type: STREAM
        Seed: 654321
}
