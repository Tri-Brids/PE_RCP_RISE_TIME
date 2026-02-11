/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef BOARD_H
#define BOARD_H

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//
// Included Files
//

#include "driverlib.h"
#include "device.h"

//*****************************************************************************
//
// PinMux Configurations
//
//*****************************************************************************

//
// EPWM1 -> myEPWM0 Pinmux
//
//
// EPWM1A - GPIO Settings
//
#define GPIO_PIN_EPWM1A 0
#define myEPWM0_EPWMA_GPIO 0
#define myEPWM0_EPWMA_PIN_CONFIG GPIO_0_EPWM1A
//
// EPWM1B - GPIO Settings
//
#define GPIO_PIN_EPWM1B 1
#define myEPWM0_EPWMB_GPIO 1
#define myEPWM0_EPWMB_PIN_CONFIG GPIO_1_EPWM1B

//
// EPWM2 -> myEPWM1 Pinmux
//
//
// EPWM2A - GPIO Settings
//
#define GPIO_PIN_EPWM2A 2
#define myEPWM1_EPWMA_GPIO 2
#define myEPWM1_EPWMA_PIN_CONFIG GPIO_2_EPWM2A
//
// EPWM2B - GPIO Settings
//
#define GPIO_PIN_EPWM2B 3
#define myEPWM1_EPWMB_GPIO 3
#define myEPWM1_EPWMB_PIN_CONFIG GPIO_3_EPWM2B

//
// EPWM3 -> myEPWM2 Pinmux
//
//
// EPWM3A - GPIO Settings
//
#define GPIO_PIN_EPWM3A 4
#define myEPWM2_EPWMA_GPIO 4
#define myEPWM2_EPWMA_PIN_CONFIG GPIO_4_EPWM3A
//
// EPWM3B - GPIO Settings
//
#define GPIO_PIN_EPWM3B 5
#define myEPWM2_EPWMB_GPIO 5
#define myEPWM2_EPWMB_PIN_CONFIG GPIO_5_EPWM3B

//
// EPWM4 -> myEPWM3 Pinmux
//
//
// EPWM4A - GPIO Settings
//
#define GPIO_PIN_EPWM4A 6
#define myEPWM3_EPWMA_GPIO 6
#define myEPWM3_EPWMA_PIN_CONFIG GPIO_6_EPWM4A
//
// EPWM4B - GPIO Settings
//
#define GPIO_PIN_EPWM4B 7
#define myEPWM3_EPWMB_GPIO 7
#define myEPWM3_EPWMB_PIN_CONFIG GPIO_7_EPWM4B

//
// EPWM5 -> myEPWM4 Pinmux
//
//
// EPWM5A - GPIO Settings
//
#define GPIO_PIN_EPWM5A 8
#define myEPWM4_EPWMA_GPIO 8
#define myEPWM4_EPWMA_PIN_CONFIG GPIO_8_EPWM5A
//
// EPWM5B - GPIO Settings
//
#define GPIO_PIN_EPWM5B 9
#define myEPWM4_EPWMB_GPIO 9
#define myEPWM4_EPWMB_PIN_CONFIG GPIO_9_EPWM5B

//
// EPWM6 -> myEPWM5 Pinmux
//
//
// EPWM6A - GPIO Settings
//
#define GPIO_PIN_EPWM6A 10
#define myEPWM5_EPWMA_GPIO 10
#define myEPWM5_EPWMA_PIN_CONFIG GPIO_10_EPWM6A
//
// EPWM6B - GPIO Settings
//
#define GPIO_PIN_EPWM6B 11
#define myEPWM5_EPWMB_GPIO 11
#define myEPWM5_EPWMB_PIN_CONFIG GPIO_11_EPWM6B

//
// EPWM7 -> myEPWM6 Pinmux
//
//
// EPWM7A - GPIO Settings
//
#define GPIO_PIN_EPWM7A 12
#define myEPWM6_EPWMA_GPIO 12
#define myEPWM6_EPWMA_PIN_CONFIG GPIO_12_EPWM7A
//
// EPWM7B - GPIO Settings
//
#define GPIO_PIN_EPWM7B 13
#define myEPWM6_EPWMB_GPIO 13
#define myEPWM6_EPWMB_PIN_CONFIG GPIO_13_EPWM7B

//
// EPWM8 -> myEPWM7 Pinmux
//
//
// EPWM8A - GPIO Settings
//
#define GPIO_PIN_EPWM8A 14
#define myEPWM7_EPWMA_GPIO 14
#define myEPWM7_EPWMA_PIN_CONFIG GPIO_14_EPWM8A
//
// EPWM8B - GPIO Settings
//
#define GPIO_PIN_EPWM8B 15
#define myEPWM7_EPWMB_GPIO 15
#define myEPWM7_EPWMB_PIN_CONFIG GPIO_15_EPWM8B
//
// GPIO34 - GPIO Settings
//
#define myBoardLED0_GPIO_GPIO_PIN_CONFIG GPIO_34_GPIO34

//
// SCIA -> mySCI0 Pinmux
//
//
// SCIRXDA - GPIO Settings
//
#define GPIO_PIN_SCIRXDA 28
#define mySCI0_SCIRX_GPIO 28
#define mySCI0_SCIRX_PIN_CONFIG GPIO_28_SCIRXDA
//
// SCITXDA - GPIO Settings
//
#define GPIO_PIN_SCITXDA 29
#define mySCI0_SCITX_GPIO 29
#define mySCI0_SCITX_PIN_CONFIG GPIO_29_SCITXDA

//*****************************************************************************
//
// ADC Configurations
//
//*****************************************************************************
#define myADC0_BASE ADCA_BASE
#define myADC0_RESULT_BASE ADCARESULT_BASE
#define myADC0_SOC0 ADC_SOC_NUMBER0
#define myADC0_FORCE_SOC0 ADC_FORCE_SOC0
#define myADC0_SAMPLE_WINDOW_SOC0 5
#define myADC0_TRIGGER_SOURCE_SOC0 ADC_TRIGGER_EPWM1_SOCA
#define myADC0_CHANNEL_SOC0 ADC_CH_ADCIN0
#define myADC0_SOC1 ADC_SOC_NUMBER1
#define myADC0_FORCE_SOC1 ADC_FORCE_SOC1
#define myADC0_SAMPLE_WINDOW_SOC1 5
#define myADC0_TRIGGER_SOURCE_SOC1 ADC_TRIGGER_EPWM1_SOCB
#define myADC0_CHANNEL_SOC1 ADC_CH_ADCIN2
#define myADC0_SOC2 ADC_SOC_NUMBER2
#define myADC0_FORCE_SOC2 ADC_FORCE_SOC2
#define myADC0_SAMPLE_WINDOW_SOC2 5
#define myADC0_TRIGGER_SOURCE_SOC2 ADC_TRIGGER_EPWM2_SOCA
#define myADC0_CHANNEL_SOC2 ADC_CH_ADCIN4
void myADC0_init();

#define myADC1_BASE ADCB_BASE
#define myADC1_RESULT_BASE ADCBRESULT_BASE
#define myADC1_SOC3 ADC_SOC_NUMBER3
#define myADC1_FORCE_SOC3 ADC_FORCE_SOC3
#define myADC1_SAMPLE_WINDOW_SOC3 5
#define myADC1_TRIGGER_SOURCE_SOC3 ADC_TRIGGER_EPWM2_SOCB
#define myADC1_CHANNEL_SOC3 ADC_CH_ADCIN0
#define myADC1_SOC8 ADC_SOC_NUMBER8
#define myADC1_FORCE_SOC8 ADC_FORCE_SOC8
#define myADC1_SAMPLE_WINDOW_SOC8 5
#define myADC1_TRIGGER_SOURCE_SOC8 ADC_TRIGGER_EPWM5_SOCA
#define myADC1_CHANNEL_SOC8 ADC_CH_ADCIN2
#define myADC1_SOC9 ADC_SOC_NUMBER9
#define myADC1_FORCE_SOC9 ADC_FORCE_SOC9
#define myADC1_SAMPLE_WINDOW_SOC9 5
#define myADC1_TRIGGER_SOURCE_SOC9 ADC_TRIGGER_EPWM5_SOCB
#define myADC1_CHANNEL_SOC9 ADC_CH_ADCIN4
void myADC1_init();

#define myADC2_BASE ADCC_BASE
#define myADC2_RESULT_BASE ADCCRESULT_BASE
#define myADC2_SOC10 ADC_SOC_NUMBER10
#define myADC2_FORCE_SOC10 ADC_FORCE_SOC10
#define myADC2_SAMPLE_WINDOW_SOC10 5
#define myADC2_TRIGGER_SOURCE_SOC10 ADC_TRIGGER_EPWM6_SOCA
#define myADC2_CHANNEL_SOC10 ADC_CH_ADCIN2
#define myADC2_SOC11 ADC_SOC_NUMBER11
#define myADC2_FORCE_SOC11 ADC_FORCE_SOC11
#define myADC2_SAMPLE_WINDOW_SOC11 75
#define myADC2_TRIGGER_SOURCE_SOC11 ADC_TRIGGER_EPWM6_SOCB
#define myADC2_CHANNEL_SOC11 ADC_CH_ADCIN4
void myADC2_init();

#define myADC3_BASE ADCD_BASE
#define myADC3_RESULT_BASE ADCDRESULT_BASE
#define myADC3_SOC4 ADC_SOC_NUMBER4
#define myADC3_FORCE_SOC4 ADC_FORCE_SOC4
#define myADC3_SAMPLE_WINDOW_SOC4 5
#define myADC3_TRIGGER_SOURCE_SOC4 ADC_TRIGGER_EPWM3_SOCA
#define myADC3_CHANNEL_SOC4 ADC_CH_ADCIN0
#define myADC3_SOC5 ADC_SOC_NUMBER5
#define myADC3_FORCE_SOC5 ADC_FORCE_SOC5
#define myADC3_SAMPLE_WINDOW_SOC5 5
#define myADC3_TRIGGER_SOURCE_SOC5 ADC_TRIGGER_EPWM3_SOCB
#define myADC3_CHANNEL_SOC5 ADC_CH_ADCIN1
#define myADC3_SOC6 ADC_SOC_NUMBER6
#define myADC3_FORCE_SOC6 ADC_FORCE_SOC6
#define myADC3_SAMPLE_WINDOW_SOC6 5
#define myADC3_TRIGGER_SOURCE_SOC6 ADC_TRIGGER_EPWM4_SOCA
#define myADC3_CHANNEL_SOC6 ADC_CH_ADCIN2
#define myADC3_SOC7 ADC_SOC_NUMBER7
#define myADC3_FORCE_SOC7 ADC_FORCE_SOC7
#define myADC3_SAMPLE_WINDOW_SOC7 5
#define myADC3_TRIGGER_SOURCE_SOC7 ADC_TRIGGER_EPWM4_SOCB
#define myADC3_CHANNEL_SOC7 ADC_CH_ADCIN3
#define myADC3_SOC12 ADC_SOC_NUMBER12
#define myADC3_FORCE_SOC12 ADC_FORCE_SOC12
#define myADC3_SAMPLE_WINDOW_SOC12 5
#define myADC3_TRIGGER_SOURCE_SOC12 ADC_TRIGGER_EPWM7_SOCA
#define myADC3_CHANNEL_SOC12 ADC_CH_ADCIN4
#define myADC3_SOC13 ADC_SOC_NUMBER13
#define myADC3_FORCE_SOC13 ADC_FORCE_SOC13
#define myADC3_SAMPLE_WINDOW_SOC13 5
#define myADC3_TRIGGER_SOURCE_SOC13 ADC_TRIGGER_EPWM7_SOCB
#define myADC3_CHANNEL_SOC13 ADC_CH_ADCIN5
#define myADC3_SOC14 ADC_SOC_NUMBER14
#define myADC3_FORCE_SOC14 ADC_FORCE_SOC14
#define myADC3_SAMPLE_WINDOW_SOC14 5
#define myADC3_TRIGGER_SOURCE_SOC14 ADC_TRIGGER_EPWM8_SOCA
#define myADC3_CHANNEL_SOC14 ADC_CH_ADCIN14
#define myADC3_SOC15 ADC_SOC_NUMBER15
#define myADC3_FORCE_SOC15 ADC_FORCE_SOC15
#define myADC3_SAMPLE_WINDOW_SOC15 5
#define myADC3_TRIGGER_SOURCE_SOC15 ADC_TRIGGER_EPWM8_SOCB
#define myADC3_CHANNEL_SOC15 ADC_CH_ADCIN15
void myADC3_init();


//*****************************************************************************
//
// EPWM Configurations
//
//*****************************************************************************
#define myEPWM0_BASE EPWM1_BASE
#define myEPWM0_TBPRD 25000
#define myEPWM0_COUNTER_MODE EPWM_COUNTER_MODE_UP_DOWN
#define myEPWM0_TBPHS 0
#define myEPWM0_CMPA 12500
#define myEPWM0_CMPB 12500
#define myEPWM0_CMPC 0
#define myEPWM0_CMPD 0
#define myEPWM0_DBRED 0
#define myEPWM0_DBFED 0
#define myEPWM0_TZA_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM0_TZB_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM0_INTERRUPT_SOURCE EPWM_INT_TBCTR_DISABLED
#define myEPWM1_BASE EPWM2_BASE
#define myEPWM1_TBPRD 25000
#define myEPWM1_COUNTER_MODE EPWM_COUNTER_MODE_UP_DOWN
#define myEPWM1_TBPHS 0
#define myEPWM1_CMPA 12500
#define myEPWM1_CMPB 12500
#define myEPWM1_CMPC 0
#define myEPWM1_CMPD 0
#define myEPWM1_DBRED 0
#define myEPWM1_DBFED 0
#define myEPWM1_TZA_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM1_TZB_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM1_INTERRUPT_SOURCE EPWM_INT_TBCTR_DISABLED
#define myEPWM2_BASE EPWM3_BASE
#define myEPWM2_TBPRD 25000
#define myEPWM2_COUNTER_MODE EPWM_COUNTER_MODE_UP_DOWN
#define myEPWM2_TBPHS 0
#define myEPWM2_CMPA 12500
#define myEPWM2_CMPB 12500
#define myEPWM2_CMPC 0
#define myEPWM2_CMPD 0
#define myEPWM2_DBRED 0
#define myEPWM2_DBFED 0
#define myEPWM2_TZA_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM2_TZB_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM2_INTERRUPT_SOURCE EPWM_INT_TBCTR_DISABLED
#define myEPWM3_BASE EPWM4_BASE
#define myEPWM3_TBPRD 25000
#define myEPWM3_COUNTER_MODE EPWM_COUNTER_MODE_UP_DOWN
#define myEPWM3_TBPHS 0
#define myEPWM3_CMPA 12500
#define myEPWM3_CMPB 12500
#define myEPWM3_CMPC 0
#define myEPWM3_CMPD 0
#define myEPWM3_DBRED 0
#define myEPWM3_DBFED 0
#define myEPWM3_TZA_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM3_TZB_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM3_INTERRUPT_SOURCE EPWM_INT_TBCTR_DISABLED
#define myEPWM4_BASE EPWM5_BASE
#define myEPWM4_TBPRD 25000
#define myEPWM4_COUNTER_MODE EPWM_COUNTER_MODE_UP_DOWN
#define myEPWM4_TBPHS 0
#define myEPWM4_CMPA 12500
#define myEPWM4_CMPB 12500
#define myEPWM4_CMPC 0
#define myEPWM4_CMPD 0
#define myEPWM4_DBRED 0
#define myEPWM4_DBFED 0
#define myEPWM4_TZA_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM4_TZB_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM4_INTERRUPT_SOURCE EPWM_INT_TBCTR_DISABLED
#define myEPWM5_BASE EPWM6_BASE
#define myEPWM5_TBPRD 25000
#define myEPWM5_COUNTER_MODE EPWM_COUNTER_MODE_UP_DOWN
#define myEPWM5_TBPHS 0
#define myEPWM5_CMPA 12500
#define myEPWM5_CMPB 12500
#define myEPWM5_CMPC 0
#define myEPWM5_CMPD 0
#define myEPWM5_DBRED 0
#define myEPWM5_DBFED 0
#define myEPWM5_TZA_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM5_TZB_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM5_INTERRUPT_SOURCE EPWM_INT_TBCTR_DISABLED
#define myEPWM6_BASE EPWM7_BASE
#define myEPWM6_TBPRD 25000
#define myEPWM6_COUNTER_MODE EPWM_COUNTER_MODE_UP_DOWN
#define myEPWM6_TBPHS 0
#define myEPWM6_CMPA 12500
#define myEPWM6_CMPB 12500
#define myEPWM6_CMPC 0
#define myEPWM6_CMPD 0
#define myEPWM6_DBRED 0
#define myEPWM6_DBFED 0
#define myEPWM6_TZA_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM6_TZB_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM6_INTERRUPT_SOURCE EPWM_INT_TBCTR_DISABLED
#define myEPWM7_BASE EPWM8_BASE
#define myEPWM7_TBPRD 25000
#define myEPWM7_COUNTER_MODE EPWM_COUNTER_MODE_UP_DOWN
#define myEPWM7_TBPHS 0
#define myEPWM7_CMPA 12500
#define myEPWM7_CMPB 12500
#define myEPWM7_CMPC 0
#define myEPWM7_CMPD 0
#define myEPWM7_DBRED 0
#define myEPWM7_DBFED 0
#define myEPWM7_TZA_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM7_TZB_ACTION EPWM_TZ_ACTION_HIGH_Z
#define myEPWM7_INTERRUPT_SOURCE EPWM_INT_TBCTR_DISABLED

//*****************************************************************************
//
// GPIO Configurations
//
//*****************************************************************************
#define myBoardLED0_GPIO 34
void myBoardLED0_GPIO_init();

//*****************************************************************************
//
// INTERRUPT Configurations
//
//*****************************************************************************

// Interrupt Settings for INT_myADC0_1
// ISR need to be defined for the registered interrupts
#define INT_myADC0_1 INT_ADCA1
#define INT_myADC0_1_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP1
extern __interrupt void INT_myADC0_1_ISR(void);

// Interrupt Settings for INT_myADC0_2
// ISR need to be defined for the registered interrupts
#define INT_myADC0_2 INT_ADCA2
#define INT_myADC0_2_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP10
extern __interrupt void INT_myADC0_2_ISR(void);

// Interrupt Settings for INT_myADC0_3
// ISR need to be defined for the registered interrupts
#define INT_myADC0_3 INT_ADCA3
#define INT_myADC0_3_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP10
extern __interrupt void INT_myADC0_3_ISR(void);

// Interrupt Settings for INT_myADC1_1
// ISR need to be defined for the registered interrupts
#define INT_myADC1_1 INT_ADCB1
#define INT_myADC1_1_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP1
extern __interrupt void INT_myADC1_1_ISR(void);

// Interrupt Settings for INT_myADC1_2
// ISR need to be defined for the registered interrupts
#define INT_myADC1_2 INT_ADCB2
#define INT_myADC1_2_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP10
extern __interrupt void INT_myADC1_2_ISR(void);

// Interrupt Settings for INT_myADC1_3
// ISR need to be defined for the registered interrupts
#define INT_myADC1_3 INT_ADCB3
#define INT_myADC1_3_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP10
extern __interrupt void INT_myADC1_3_ISR(void);

// Interrupt Settings for INT_myADC2_1
// ISR need to be defined for the registered interrupts
#define INT_myADC2_1 INT_ADCC1
#define INT_myADC2_1_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP1
extern __interrupt void INT_myADC2_1_ISR(void);

// Interrupt Settings for INT_myADC2_2
// ISR need to be defined for the registered interrupts
#define INT_myADC2_2 INT_ADCC2
#define INT_myADC2_2_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP10
extern __interrupt void INT_myADC2_2_ISR(void);

// Interrupt Settings for INT_myADC3_1
// ISR need to be defined for the registered interrupts
#define INT_myADC3_1 INT_ADCD1
#define INT_myADC3_1_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP1
extern __interrupt void INT_myADC3_1_ISR(void);

// Interrupt Settings for INT_myADC3_2
// ISR need to be defined for the registered interrupts
#define INT_myADC3_2 INT_ADCD2
#define INT_myADC3_2_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP10
extern __interrupt void INT_myADC3_2_ISR(void);

// Interrupt Settings for INT_myADC3_3
// ISR need to be defined for the registered interrupts
#define INT_myADC3_3 INT_ADCD3
#define INT_myADC3_3_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP10
extern __interrupt void INT_myADC3_3_ISR(void);

// Interrupt Settings for INT_myADC3_4
// ISR need to be defined for the registered interrupts
#define INT_myADC3_4 INT_ADCD4
#define INT_myADC3_4_INTERRUPT_ACK_GROUP INTERRUPT_ACK_GROUP10
extern __interrupt void INT_myADC3_4_ISR(void);

//*****************************************************************************
//
// SCI Configurations
//
//*****************************************************************************
#define mySCI0_BASE SCIA_BASE
#define mySCI0_BAUDRATE 115200
#define mySCI0_CONFIG_WLEN SCI_CONFIG_WLEN_8
#define mySCI0_CONFIG_STOP SCI_CONFIG_STOP_ONE
#define mySCI0_CONFIG_PAR SCI_CONFIG_PAR_NONE
#define mySCI0_FIFO_TX_LVL SCI_FIFO_TX0
#define mySCI0_FIFO_RX_LVL SCI_FIFO_RX0
void mySCI0_init();

//*****************************************************************************
//
// SYNC Scheme Configurations
//
//*****************************************************************************

//*****************************************************************************
//
// SYSCTL Configurations
//
//*****************************************************************************

//*****************************************************************************
//
// Board Configurations
//
//*****************************************************************************
void	Board_init();
void	ADC_init();
void	EPWM_init();
void	GPIO_init();
void	INTERRUPT_init();
void	SCI_init();
void	SYNC_init();
void	SYSCTL_init();
void	PinMux_init();

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif  // end of BOARD_H definition
