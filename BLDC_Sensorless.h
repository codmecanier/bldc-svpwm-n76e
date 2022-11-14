void BEMF_Gpio_ADCIN_Init();

void Set_Phase_U_Voltage_ADC_Value(unsigned int i);
void Set_Phase_V_Voltage_ADC_Value(unsigned int i);
void Set_Phase_W_Voltage_ADC_Value(unsigned int i);

void Start_BEMF_Detect_ADC(unsigned char eleccycle, unsigned char times,bit pwm_on_sense);
unsigned char BEMF_Calculate(unsigned char eleccycle);

#define SAMPLE_BUFFER_LENGTH 2

#define BLDC_Stop 0
#define BLDC_Align 1
#define BLDC_Startup 2
#define BLDC_Run 3
#define BLDC_IPD 4
#define BLDC_SureStart 5