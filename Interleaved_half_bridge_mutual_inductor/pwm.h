typedef struct {
    float theta;  // Current theta value
    float m;  // this is a control variable between 0 and 1;
    float m_scale; // This is meant to be static and set to the max ePWM Counter value
    float thetastep; // control variable, how much theta is added to theta at each interupt
    float daf,dbf,dcf,dinj;
    unsigned da;
    unsigned db;
    unsigned dc;
    unsigned reference_mode;
} tPWMctrl;


#define PWM_FULL_PERIOD 1000
#define PWM_HALF_PERIOD PWM_FULL_PERIOD/2


void init_pwm_struct(tPWMctrl *PWMctrl);
void update_pwm(tPWMctrl *PWMctrl);
