

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "pwm.h"


// Prototype statements for functions found within this file.



void InitEPwm2(void);
void InitEPwm6(void);

__interrupt void epwm2_isr(void);


#define EPWM2_TIMER_TBPRD  1000  // Period register
#define EPWM6_TIMER_TBPRD  1000  // Period register


// States
#define IDLE 0
#define RUN_MODE_1 1
#define TEST_MODE_1 2
#define TEST_MODE_2 3
#define TEST_MODE_3 4

int state = 0;
int start_stop_flag = 0;
int start_test_mode_1_flag = 0;
int start_test_mode_2_flag = 0;
int start_test_mode_3_flag = 0;
int update_pwm_flag = 0;

// gate driver enable disable defines
#define DISABLE_MOSFET GpioDataRegs.GPACLEAR.bit.GPIO27 = 1
#define ENABLE_MOSFET GpioDataRegs.GPASET.bit.GPIO27 = 1






void main(void)
{
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xS_SysCtrl.c file.
    InitSysCtrl();

// Step 2. Initialize GPIO:
// This example function is found in the F2837xS_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
    InitGpio();

    //makes these pins outputs
    EALLOW;
    // Enable Mosfet Driver pin
    GpioCtrlRegs.GPADIR.bit.GPIO27 = 1;
    // LED pins

    DISABLE_MOSFET;

    EDIS;






    CpuSysRegs.PCLKCR2.bit.EPWM2=1;
    CpuSysRegs.PCLKCR2.bit.EPWM6=1;

// For this case just init GPIO pins for ePWM1, ePWM2, ePWM3
// These functions are in the F28M36x_EPwm.c file

    InitEPwm2Gpio();
    InitEPwm6Gpio();


// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
    DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the F2837xS_PieCtrl.c file.
    InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.

    InitPieVectTable();

    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    EALLOW; // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM2_INT = &epwm2_isr;
    EDIS;   // This is needed to disable write to EALLOW protected registers

    // Enable CPU INT3 which is connected to EPWM1-3 INT:
    IER |= M_INT3;
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    // Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    // For this example, only initialize the ePWM

    EALLOW;
    EPwm2Regs.TZFRC.bit.OST = 1;
    EPwm6Regs.TZFRC.bit.OST = 1;
    //EPwm6Regs.TZSEL.bit.OSHT1 = TZ_ENABLE; //WTF? ok this connects to TZ1_ internally and if this is not routed will screw everything up, not needed if software only Trip Zone
    EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO;
    EPwm6Regs.TZCTL.bit.TZA = TZ_FORCE_LO;
    EPwm6Regs.TZCTL.bit.TZB = TZ_FORCE_LO;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC =0;
    EDIS;




// Step 4. User specific code, enable interrupts:

// Enable CPU INT3 which is connected to EPWM1-3 INT:
    //IER |= M_INT3;

// Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
//  PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
//    PieCtrlRegs.PIEIER3.bit.INTx6 = 1;
//  PieCtrlRegs.PIEIER3.bit.INTx3 = 1;


// Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    // Setup the PWM modules
    InitEPwm2();
    InitEPwm6();

    tPWMctrl PWMctrl;
    init_pwm_struct(&PWMctrl);




    // Step 5. IDLE loop. Just sit and loop forever (optional):
    for(;;)
    {
        switch(state) {
        case IDLE:
            if (start_stop_flag)
            {
                start_stop_flag = 0;
                start_test_mode_1_flag = 0;
                start_test_mode_2_flag = 0;
                start_test_mode_3_flag = 0;
                state = RUN_MODE_1;
                ENABLE_MOSFET;
                EALLOW;
                CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1; // start pwm counter
                EPwm2Regs.TZCLR.bit.OST = 1; // clear trip zone
                EPwm6Regs.TZCLR.bit.OST = 1; // clear trip zone
                EDIS;
            }
            else if (start_test_mode_2_flag) {
                start_stop_flag = 0;
                start_test_mode_1_flag = 0;
                start_test_mode_2_flag = 0;
                start_test_mode_3_flag = 0;
            }
            else if (start_test_mode_2_flag) {
                start_stop_flag = 0;
                start_test_mode_1_flag = 0;
                start_test_mode_2_flag = 0;
                start_test_mode_3_flag = 0;
            }
            else if (start_test_mode_3_flag)
            {
                start_stop_flag = 0;
                start_test_mode_1_flag = 0;
                start_test_mode_2_flag = 0;
                start_test_mode_3_flag = 0;
            }

            break;
        case RUN_MODE_1:
            if (start_stop_flag)
            {
                start_stop_flag = 0;
                DISABLE_MOSFET;
                EALLOW;
                EPwm2Regs.TZFRC.bit.OST = 1;
                EPwm6Regs.TZFRC.bit.OST = 1;
                CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0; // stop pwm counter
                EDIS;
                state = IDLE;
            } else if (update_pwm_flag)
            {
                update_pwm_flag = 0;
                update_pwm(&PWMctrl);
            }
            break;
        case TEST_MODE_1:

            break;
        case TEST_MODE_2:

            break;
        case TEST_MODE_3:

            break;


        }
    }

}

__interrupt void epwm2_isr(void)
{
    // Clear INT flag for this PWM int
    EPwm2Regs.ETCLR.bit.INT = 1;
    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    update_pwm_flag = 1;
}





void InitEPwm2(void)
{
    // Setup TBCLK
    EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;         // Set timer period 801 TBCLKs
    EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;          // Phase is 0
    EPwm2Regs.TBCTR = 0x0000;                    // Clear counter

    // Set Compare values
    EPwm2Regs.CMPA.bit.CMPA = 0;    // Set compare A value


    // Setup counter mode
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadow register load on ZERO
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    //Set actions
    EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;
    EPwm2Regs.AQCTLA.bit.PRD = AQ_CLEAR;
    EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;
    EPwm2Regs.AQCTLA.bit.CBD = AQ_CLEAR;

    EPwm2Regs.DBCTL.bit.IN_MODE  = 0;
    EPwm2Regs.DBCTL.bit.DEDB_MODE = 0 ;
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm2Regs.DBCTL.bit.OUTSWAP = 0;
    EPwm2Regs.DBFED.bit.DBFED = 5;
    EPwm2Regs.DBRED.bit.DBRED = 5;

    EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;
    EPwm2Regs.ETSEL.bit.INTEN = 1;
    EPwm2Regs.ETPS.bit.INTPRD = ET_1ST;

}


void InitEPwm6(void)
{
    // Setup TBCLK
    EPwm6Regs.TBPRD = EPWM6_TIMER_TBPRD;         // Set timer period 801 TBCLKs
    EPwm6Regs.TBPHS.bit.TBPHS = 0x0000;          // Phase is 0
    EPwm6Regs.TBCTR = 0x0000;                    // Clear counter

    // Set Compare values
    EPwm6Regs.CMPA.bit.CMPA = 0;    // Set compare A value

    // Setup counter mode
    EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadow register load on ZERO
    EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    //Set actions
    EPwm6Regs.AQCTLA.bit.CAU = AQ_SET;
    EPwm6Regs.AQCTLA.bit.PRD = AQ_CLEAR;
    EPwm6Regs.AQCTLA.bit.CAD = AQ_SET;
    EPwm6Regs.AQCTLA.bit.CBD = AQ_CLEAR;

    EPwm6Regs.DBCTL.bit.IN_MODE  = 0;
    EPwm6Regs.DBCTL.bit.DEDB_MODE = 0 ;
    EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm6Regs.DBCTL.bit.OUTSWAP = 0;
    EPwm6Regs.DBFED.bit.DBFED = 5;
    EPwm6Regs.DBRED.bit.DBRED = 5;

}






