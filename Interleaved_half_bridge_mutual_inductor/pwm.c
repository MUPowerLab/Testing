#include "F2837xD_device.h"
#include "F2837xD_EPwm_defines.h"
#include "pwm.h"

float ot = 0.33333333333333333333333333333333333;

void init_pwm_struct(tPWMctrl *PWMctrl) {
    PWMctrl->m_scale = PWM_HALF_PERIOD;
    PWMctrl->m=.1;
    PWMctrl->theta=0;
    PWMctrl->thetastep=0.006;
}



void update_pwm(tPWMctrl *PWMctrl) {
    // increment theta
    PWMctrl->theta=PWMctrl->theta+PWMctrl->thetastep;

    //wrap theta, theta needs to between 0 and 1 for the sinpuf32 function, in otherwords its normalized
    if (PWMctrl->theta>1)
        PWMctrl->theta-=1;

    // Calculate Duty Cycle for Sine PWM
    PWMctrl->daf = (__sinpuf32(PWMctrl->theta)*PWMctrl->m);
    PWMctrl->dbf = (__sinpuf32(PWMctrl->theta+ot)*PWMctrl->m);
    PWMctrl->dcf = (__sinpuf32(PWMctrl->theta-ot)*PWMctrl->m);

    // Level shift, scale, and convert float to unsigned int for the PWM regs
    PWMctrl->da = (PWMctrl->daf + 1) * PWMctrl->m_scale;
    PWMctrl->db = (PWMctrl->dbf + 1) * PWMctrl->m_scale;
    PWMctrl->dc = (PWMctrl->dcf + 1) * PWMctrl->m_scale;

    EPwm2Regs.CMPA.bit.CMPA = PWMctrl->da;
    EPwm6Regs.CMPA.bit.CMPA = PWMctrl->da;

/*    EPwm1Regs.CMPA.bit.CMPA = 0;
    EPwm2Regs.CMPA.bit.CMPA = PWMctrl->da;
    EPwm3Regs.CMPA.bit.CMPA= PWM_FULL_PERIOD;

    EPwm4Regs.CMPA.bit.CMPA = PWM_FULL_PERIOD;
    EPwm5Regs.CMPA.bit.CMPA = PWM_FULL_PERIOD;
    EPwm6Regs.CMPA.bit.CMPA= 0;

    EPwm7Regs.CMPA.bit.CMPA = PWM_FULL_PERIOD;
    EPwm8Regs.CMPA.bit.CMPA = PWMctrl->db;
    EPwm9Regs.CMPA.bit.CMPA= PWM_FULL_PERIOD;

    EPwm10Regs.CMPA.bit.CMPA = PWM_FULL_PERIOD;
    EPwm11Regs.CMPA.bit.CMPA = PWMctrl->dc;
    EPwm12Regs.CMPA.bit.CMPA= PWM_FULL_PERIOD;*/

}
