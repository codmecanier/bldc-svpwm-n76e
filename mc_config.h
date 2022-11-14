#ifndef __MC_Config_H__
#define __MC_Config_H__

// The syncronous motor including the PMSM motor and the ACIM Motor 
#define CFG_BLDC_PMSM

#define CFG_BLDC_SENSORLESS

// Asyncronous Motor ACIM （三相异步电动机）
#define CFG_ACIM

//启用SVPWM模式
#define CFG_ENABLE_SVPWM  

#ifdef CFG_ENABLE_SVPWM

#define DEFAULT_ELEC_ANGLE_OFFSET_CW             215
#define DEFAULT_ELEC_ANGLE_OFFSET_CCW            189
#define DEFAULT_STABLE_COUNT_PERIOD      10

#endif 

// Use external comparator for detecting the BEMF from motor not the internal ADC for a higher spinning velocity support
#define BEMF_DETECT_EXTERNAL_COMPARATOR

#ifdef CFG_BLDC_SENSORLESS
//BLDC Sensorless startup Setting region (无霍尔传感器无刷电机驱动设定)
#define DEFAULT_STARTUP_FREQUENCY            10
#define DEFAULT_STARTUP_END_FREQUENCY        70
#define DEFAULT_STARTUP_PWM                  23
#define DEFAULT_STARTUP_END_PWM              50
#define DEFAULT_ACCELERATION_TIME            1000
#define DEFAULT_LOCK_POSITION_TIME           500
#define DEFAULT_LOCK_POSITION_PWM            26
#define DEFAULT_DIREACTION_CHANGE_DELAY      400
#endif

#ifdef CFG_ACIM  //异步电机配置区
#define DEFAULT_VVVF_MAX_FREQUENCY              4000
#define DEFAULT_VVVF_MIN_FREQUENCY              1 
#define DEFAULT_VVVF_FREQUENCY_SECTION_CUT      0
#define DEFAULT_VVVF_RATIO_1                    3
#define DEFAULT_VVVF_RATIO_2                    3
#define DEFAULT_VVVF_ACCELERATION_HZS2          1000
#define DEFAULT_VVVF_DECELERATION_HZS2          1000
#endif


#endif