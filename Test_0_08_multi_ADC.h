
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

/*********************************************************************************
 * Defines
 *********************************************************************************/

#define RESULTS_BUFFER_SIZE     50      // Samples per test (circular buffer)
#define TESTS_PER_WINDOW        10      // Number of repetitions per window
#define SAMPLE_DELAY_US         200     // Delay between samples

#define ACQ_WINDOW_START        1       // Starting acquisition window (cycles)
#define ACQ_WINDOW_END          20      // Ending acquisition window (cycles)
#define NUM_WINDOWS             (ACQ_WINDOW_END - ACQ_WINDOW_START + 1)  // 20 windows

/*********************************************************************************
 * Typedefs
 *********************************************************************************/

typedef struct {
    uint16_t windowCycles;      // Acquisition window in SYSCLK cycles
    uint16_t min;               // Minimum ADC reading
    uint16_t max;               // Maximum ADC reading
    uint16_t avg;               // Average ADC reading
    uint16_t range;             // Max - Min
    float    stdDev;            // Standard deviation
} WindowStats;

/*********************************************************************************
 * Structures
 *********************************************************************************/

/*********************************************************************************
 * Extern Variable Declarations
 * Actual definitions live in Test_0_08_multi_ADC.c
 *********************************************************************************/

// --- Sample result circular buffers ---

extern volatile uint16_t adcAResults[RESULTS_BUFFER_SIZE];
extern volatile uint16_t adcBResults[RESULTS_BUFFER_SIZE];
extern volatile uint16_t adcCResults[RESULTS_BUFFER_SIZE];

// --- Circular buffer write indices ---
extern volatile uint16_t adcAIndex;
extern volatile uint16_t adcBIndex;
extern volatile uint16_t adcCIndex;

// --- Total samples collected this test ---
extern volatile uint16_t adcASampleCount;
extern volatile uint16_t adcBSampleCount;
extern volatile uint16_t adcCSampleCount;

// --- Conversion-complete flags (set by ISR, cleared by main loop) ---
extern volatile uint16_t adcAComplete;
extern volatile uint16_t adcBComplete;
extern volatile uint16_t adcCComplete;

// --- EPWM sync state flag ---
extern volatile uint16_t systemSynced;

// --- Per-test statistics (TESTS_PER_WINDOW entries, one per test run) ---
extern WindowStats adcATestResults[TESTS_PER_WINDOW];
extern WindowStats adcBTestResults[TESTS_PER_WINDOW];
extern WindowStats adcCTestResults[TESTS_PER_WINDOW];

// --- Window-averaged statistics (NUM_WINDOWS entries, one per acq window) ---
extern WindowStats adcAWindowResults[NUM_WINDOWS];
extern WindowStats adcBWindowResults[NUM_WINDOWS];
extern WindowStats adcCWindowResults[NUM_WINDOWS];

// --- Shared UART formatting buffer ---
extern char uartBuffer[256];

/*********************************************************************************
 * Global Function Prototypes
 *********************************************************************************/

// Interrupt Service Routines

__interrupt void INT_myADC0_1_ISR(void);
__interrupt void INT_myADC0_2_ISR(void);
__interrupt void INT_myADC0_3_ISR(void);

// Control & Timing
void waitForKeyPress(void);
void stopEPWMs(void);
void triggerSystemSync(void);
void delayMs(uint16_t ms);

// ADC Configuration
void setAcquisitionWindow(uint16_t cycles);

// Test Execution
void runSingleTest(uint16_t testNumber);

// Statistics
void calculateStatistics(volatile uint16_t* results, WindowStats* stats);
float calculateStdDev(volatile uint16_t* results, uint16_t avg);
void calculateWindowAverage(uint16_t windowIndex);

// Display / UART Output
void displayTestResult(uint16_t testNumber, char adcName, WindowStats* stats);
void displayWindowSummary(uint16_t windowIndex);
void displayFinalTable(void);
void displayComparison(void);
void UART_writeString(const char* str);
char UART_readChar(void);

/*********************************************************************************
 * Code
 *********************************************************************************/
 #endif /* Test_0_08_MULTI_ADC_CONFIG_H_ */
