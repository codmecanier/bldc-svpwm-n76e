void BEMF_Gpio_ADCIN_Init();

unsigned char BEMF_Calculate(unsigned char eleccycle,unsigned int dcvolt,unsigned int bemf,bit pwmondct);

#define SAMPLE_BUFFER_LENGTH 2



#define BLDC_Stop 0
#define BLDC_Align 1
#define BLDC_Startup 2
#define BLDC_Run 3
#define BLDC_IPD 4
#define BLDC_SureStart 5

#define DC_CH 0
#define BEMF_CH 1
#define SLOPE 2
