machine class:
{
# GPU-Exclusive Tasks with Tight SLAs Scenario
        Number of machines: 10
        CPU type: ARM
        Number of cores: 8
        Memory: 16384
        S-States: [110, 90, 70, 50, 25, 10, 0]
        P-States: [12, 9, 6, 4]
        C-States: [12, 5, 2, 0]
        MIPS: [900, 800, 600, 400]
        GPUs: yes
}
task class:
{
        Start time: 40000
        End time: 700000
        Inter arrival: 2000
        Expected runtime: 3000000
        Memory: 12
        VM type: LINUX_RT
        GPU enabled: yes
        SLA type: SLA0
        CPU type: ARM
        Task type: AI
        Seed: 778899
}
