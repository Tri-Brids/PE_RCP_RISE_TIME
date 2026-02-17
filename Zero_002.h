#ifndef Test_0_08_MULTI_ADC_CONFIG_H_
#define Test_0_08_MULTI_ADC_CONFIG_H_

/*********************************************************************************
 * Includes
 *********************************************************************************/
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*********************************************************************************
 * Defines
 *********************************************************************************/
#define RESULTS_BUFFER_SIZE     50
#define TESTS_PER_WINDOW        50
#define SAMPLE_DELAY_US         200

#define ACQ_WINDOW_START        1
#define ACQ_WINDOW_END          20
#define NUM_WINDOWS             (ACQ_WINDOW_END - ACQ_WINDOW_START + 1)

#define TIMEOUT_CYCLES          1000000

#define MAX_CHANNELS            4  /* Maximum channels for shared runtime arrays */

#define ADC0_NUM_CH     3
#define ADC1_NUM_CH     3
#define ADC2_NUM_CH     2
#define ADC3_NUM_CH     4

/*********************************************************************************
 * Typedefs
 *********************************************************************************/
typedef struct {
    uint16_t windowCycles;
    uint16_t min;
    uint16_t max;
    uint16_t avg;
    uint16_t range;
    float    stdDev;
} WindowStats;

/*********************************************************************************
 * Extern Variable Declarations - HYBRID APPROACH
 * 
 * LEVEL 1: Shared runtime arrays (reused during sampling)
 * LEVEL 2: Persistent storage (keeps final results per ADC)
 *********************************************************************************/

/* LEVEL 1: Shared volatile arrays - REUSED by all ADCs during sampling */
extern volatile uint16_t adcResults[MAX_CHANNELS][RESULTS_BUFFER_SIZE];
extern volatile uint16_t adcIndex[MAX_CHANNELS];
extern volatile uint16_t adcSampleCount[MAX_CHANNELS];
extern volatile uint16_t adcComplete[MAX_CHANNELS];

/* LEVEL 2: Persistent storage - SEPARATE for each ADC */
extern WindowStats adc0TestResults[ADC0_NUM_CH][TESTS_PER_WINDOW];
extern WindowStats adc1TestResults[ADC1_NUM_CH][TESTS_PER_WINDOW];
extern WindowStats adc2TestResults[ADC2_NUM_CH][TESTS_PER_WINDOW];
extern WindowStats adc3TestResults[ADC3_NUM_CH][TESTS_PER_WINDOW];

extern WindowStats adc0WindowResults[ADC0_NUM_CH][NUM_WINDOWS];
extern WindowStats adc1WindowResults[ADC1_NUM_CH][NUM_WINDOWS];
extern WindowStats adc2WindowResults[ADC2_NUM_CH][NUM_WINDOWS];
extern WindowStats adc3WindowResults[ADC3_NUM_CH][NUM_WINDOWS];

/* EPWM sync state */
extern volatile uint16_t systemSynced;

/* Shared UART scratch buffer */
extern char uartBuffer[256];

/*********************************************************************************
 * Function Prototypes
 *********************************************************************************/

/* ISRs - ADC0 (3 channels) */
__interrupt void INT_myADC0_1_ISR(void);
__interrupt void INT_myADC0_2_ISR(void);
__interrupt void INT_myADC0_3_ISR(void);

/* ISRs - ADC1 (3 channels) */
__interrupt void INT_myADC1_1_ISR(void);
__interrupt void INT_myADC1_2_ISR(void);
__interrupt void INT_myADC1_3_ISR(void);

/* ISRs - ADC2 (2 channels) */
__interrupt void INT_myADC2_1_ISR(void);
__interrupt void INT_myADC2_2_ISR(void);

/* ISRs - ADC3 (4 channels) */
__interrupt void INT_myADC3_1_ISR(void);
__interrupt void INT_myADC3_2_ISR(void);
__interrupt void INT_myADC3_3_ISR(void);
__interrupt void INT_myADC3_4_ISR(void);

/* Control */
void waitForKeyPress(void);
void stopEPWMs(void);
void startPWM(void);
void delayMs(uint16_t ms);

/* Acquisition window setters */
void setAcquisitionWindowADC0(uint16_t cycles);
void setAcquisitionWindowADC1(uint16_t cycles);
void setAcquisitionWindowADC2(uint16_t cycles);
void setAcquisitionWindowADC3(uint16_t cycles);

/* Reconfigure the SOCs to remaining ADC Pins and set their Acquisition Windows */
void ReconfigureandsetAcquisitionWindowADC0(uint16_t cycles);
void ReconfigureandsetAcquisitionWindowADC1(uint16_t cycles);
void ReconfigureandsetAcquisitionWindowADC2(uint16_t cycles);
void ReconfigureandsetAcquisitionWindowADC3(uint16_t cycles);

/* Single test runners */
void runSingleTestADC0(uint16_t testNumber);
void runSingleTestADC1(uint16_t testNumber);
void runSingleTestADC2(uint16_t testNumber);
void runSingleTestADC3(uint16_t testNumber);

/* Statistics */
void  calculateStatistics(volatile uint16_t* results, WindowStats* stats);
float calculateStdDev(volatile uint16_t* results, uint16_t avg);
void  calculateWindowAverageADC0(uint16_t windowIndex);
void  calculateWindowAverageADC1(uint16_t windowIndex);
void  calculateWindowAverageADC2(uint16_t windowIndex);
void  calculateWindowAverageADC3(uint16_t windowIndex);

/* Display */
void displayTestResult(uint16_t testNum, const char* label, WindowStats* stats);
void displayFinalTableADC0(void);
void displayFinalTableADC1(void);
void displayFinalTableADC2(void);
void displayFinalTableADC3(void);

/* UART */
void UART_writeString(const char* str);
char UART_readChar(void);

#endif /* Test_0_08_MULTI_ADC_CONFIG_H_ */
