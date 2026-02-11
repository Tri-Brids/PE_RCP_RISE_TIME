/*
 * ADC Acquisition Window Sweep Test - Per-Window Synchronization
 * F28379D LaunchPad
 * 
 * Tests acquisition windows from 1 to 20 SYSCLK cycles
 * EPWM1 and ADC are re-synchronized before each window test
 * Each window is tested 10 times and results are averaged
 * 
 * Hardware Setup:
 * - Wire GPIO0 (EPWM1A) to ADCINA0 (J1-Pin30)
 * - UART: GPIO42(TX)/GPIO43(RX) at 115200 baud
 * - LED: GPIO34 indicates testing activity
 */

#include "driverlib.h"
#include "device.h"
#include "board.h"
#include <stdio.h>
#include <string.h>

//*****************************************************************************
// Configuration
//*****************************************************************************
#define RESULTS_BUFFER_SIZE     50      // Samples per test (circular buffer)
#define TESTS_PER_WINDOW        10      // Number of repetitions per window
#define SAMPLE_DELAY_US         200     // Delay between samples

#define ACQ_WINDOW_START        1       // Starting acquisition window (cycles)
#define ACQ_WINDOW_END          20      // Ending acquisition window (cycles)
#define NUM_WINDOWS             (ACQ_WINDOW_END - ACQ_WINDOW_START + 1)  // 20 windows

//*****************************************************************************
// Statistics Structure
//*****************************************************************************
typedef struct {
    uint16_t windowCycles;      // Acquisition window in SYSCLK cycles
    uint16_t min;               // Minimum ADC reading
    uint16_t max;               // Maximum ADC reading
    uint16_t avg;               // Average ADC reading
    uint16_t range;             // Max - Min
    float    stdDev;            // Standard deviation
} WindowStats;

//*****************************************************************************
// Global Variables
//*****************************************************************************
// Circular buffer for ADC samples (reused for each test)
volatile uint16_t adcResults[RESULTS_BUFFER_SIZE];
volatile uint16_t resultsIndex = 0;
volatile uint16_t sampleCount = 0;
volatile uint16_t conversionComplete = 0;
volatile uint16_t systemSynced = 0;

// Temporary storage for 10 tests of current window
WindowStats testResults[TESTS_PER_WINDOW];

// Final averaged results for all windows (array[0] = 1 cycle, array[19] = 20 cycles)
WindowStats windowResults[NUM_WINDOWS];

char uartBuffer[256];

//*****************************************************************************
// Function Prototypes
//*****************************************************************************
__interrupt void INT_myADC0_1_ISR(void);
void waitForKeyPress(void);
void stopEPWM1(void);
void triggerSystemSync(void);
void setAcquisitionWindow(uint16_t cycles);
void runSingleTest(uint16_t testNumber);
void calculateStatistics(WindowStats* stats);
void calculateWindowAverage(uint16_t windowIndex);
void displayTestResult(uint16_t testNumber, WindowStats* stats);
void displayWindowSummary(uint16_t windowIndex);
void displayFinalTable(void);
// void displayDetailedAnalysis(void);
void UART_writeString(const char* str);
char UART_readChar(void);
void delayMs(uint16_t ms);
float calculateStdDev(uint16_t avg);

//*****************************************************************************
// Main Function
//*****************************************************************************
void main(void)
{
    uint16_t i, j;
    uint16_t currentWindow;
    
    //
    // Initialize device clock and peripherals
    //
    Device_init();
    Device_initGPIO();
    
    //
    // Initialize PIE and vector table
    //
    Interrupt_initModule();
    Interrupt_initVectorTable();
    
    //
    // Initialize board from SysConfig
    //
    Board_init();
    
    //
    // Disable EPWM modules initially
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_EPWM2);
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);  // Stop all PWMs
    
    //
    // Enable global interrupts
    //
    EINT;
    ERTM;
    
    //
    // Startup delay
    //
    delayMs(500);
    
    //
    // Display welcome message
    //
    UART_writeString("\r\n\r\n");
    UART_writeString("========================================================\r\n");
    UART_writeString("   ADC Acquisition Window Sweep Test\r\n");
    UART_writeString("   F28379D LaunchPad - Per-Window Synchronization\r\n");
    UART_writeString("========================================================\r\n\r\n");
    
    UART_writeString("System Configuration:\r\n");
    sprintf(uartBuffer, "  SYSCLK:       200 MHz (5ns per cycle)\r\n");
    UART_writeString(uartBuffer);
    sprintf(uartBuffer, "  ADC:          12-bit (0-4095 counts)\r\n");
    UART_writeString(uartBuffer);
    sprintf(uartBuffer, "  EPWM1:        2 kHz, 50%% duty cycle\r\n");
    UART_writeString(uartBuffer);
    sprintf(uartBuffer, "  Sync Mode:    EPWM restart before each window\r\n\r\n");
    UART_writeString(uartBuffer);
    
    UART_writeString("Test Configuration:\r\n");
    sprintf(uartBuffer, "  Windows:      %u to %u cycles (%uns to %uns)\r\n", 
            ACQ_WINDOW_START, ACQ_WINDOW_END,
            ACQ_WINDOW_START * 5, ACQ_WINDOW_END * 5);
    UART_writeString(uartBuffer);
    sprintf(uartBuffer, "  Tests/Window: %u repetitions\r\n", TESTS_PER_WINDOW);
    UART_writeString(uartBuffer);
    sprintf(uartBuffer, "  Samples/Test: %u samples\r\n", RESULTS_BUFFER_SIZE);
    UART_writeString(uartBuffer);
    sprintf(uartBuffer, "  Sample Rate:  Every %u us\r\n\r\n", SAMPLE_DELAY_US);
    UART_writeString(uartBuffer);
    
    sprintf(uartBuffer, "Total Measurements: %u windows x %u tests x %u samples = %lu\r\n\r\n",
            NUM_WINDOWS, TESTS_PER_WINDOW, RESULTS_BUFFER_SIZE,
            (uint32_t)NUM_WINDOWS * TESTS_PER_WINDOW * RESULTS_BUFFER_SIZE);
    UART_writeString(uartBuffer);
    
    UART_writeString("Hardware Setup:\r\n");
    UART_writeString("  1. Wire GPIO0 (EPWM1A) to ADCINA0 (J1-Pin30)\r\n");
    UART_writeString("  2. UART connected at 115200 baud\r\n");
    UART_writeString("  3. LED on GPIO34 shows activity\r\n\r\n");
    
    UART_writeString("Test Purpose:\r\n");
    UART_writeString("  Characterize ADC performance vs acquisition window.\r\n");
    UART_writeString("  Samples 50%% duty PWM to measure settling behavior.\r\n");
    UART_writeString("  EPWM restarts before each window for consistency.\r\n\r\n");
    
    UART_writeString("========================================================\r\n\r\n");
    
    //
    // Wait for user to start
    //
    UART_writeString("Press ANY KEY to start synchronized sweep test...\r\n\r\n");
    waitForKeyPress();
    
    UART_writeString("Starting acquisition window sweep...\r\n");
    UART_writeString("========================================================\r\n");
    
    //
    // Main sweep loop: Test each acquisition window from 1 to 20 cycles
    //
    for(i = 0; i < NUM_WINDOWS; i++)
    {
        currentWindow = ACQ_WINDOW_START + i;
        
        //
        // Display window header
        //
        sprintf(uartBuffer, "\r\n[Window %2u/%u] %2u cycles (%3uns)\r\n",
                i + 1, NUM_WINDOWS, currentWindow, currentWindow * 5);
        UART_writeString(uartBuffer);
        UART_writeString("--------------------------------------------------\r\n");
        
        //
        // STEP 1: Stop EPWM1 (if running from previous window)
        //
        if(i > 0)  // Skip on first iteration since EPWMs are already stopped
        {
            UART_writeString("  -> Stopping EPWM1...\r\n");
            stopEPWM1();
            delayMs(10);
        }
        
        //
        // STEP 2: Set new acquisition window
        //
        UART_writeString("  -> Setting acquisition window...\r\n");
        setAcquisitionWindow(currentWindow);
        delayMs(10);
        
        //
        // STEP 3: Trigger synchronization - restarts EPWM1 from zero
        //
        UART_writeString("  -> Synchronizing EPWM1 + ADC...\r\n");
        triggerSystemSync();
        UART_writeString("  -> Synchronized! EPWM1 running.\r\n\r\n");
        
        delayMs(50);
        
        //
        // STEP 4: Run 10 tests for this window
        //
        for(j = 0; j < TESTS_PER_WINDOW; j++)
        {
            runSingleTest(j);
            delayMs(30);
        }
        
        //
        // STEP 5: Calculate average of 10 tests for this window
        //
        calculateWindowAverage(i);
        
        //
        // STEP 6: Display summary for this window
        //
        displayWindowSummary(i);
        
        delayMs(100);
    }
    
    //
    // Stop EPWM1 after all tests complete
    //
    UART_writeString("\r\n-> Stopping EPWM1...\r\n");
    stopEPWM1();
    UART_writeString("-> EPWM1 stopped.\r\n");
    
    //
    // Display final summary table
    //
    displayFinalTable();
    
    //
    // Display detailed analysis
    //
    // displayDetailedAnalysis();
    
    //
    // Test complete
    //
    UART_writeString("\r\n========================================================\r\n");
    UART_writeString("   SWEEP TEST COMPLETE!\r\n");
    UART_writeString("========================================================\r\n");
    UART_writeString("System halted. Reset to run again.\r\n\r\n");
    
    //
    // Turn off LED
    //
    GPIO_writePin(myBoardLED0_GPIO, 1);
    
    //
    // Halt system
    //
    while(1)
    {
        // System stopped
    }
}

//*****************************************************************************
// Wait for UART Key Press
//*****************************************************************************
void waitForKeyPress(void)
{
    char receivedChar;
    
    // Simple blocking read - waits until character arrives
    receivedChar = SCI_readCharBlockingFIFO(mySCI0_BASE);
    
    sprintf(uartBuffer, "Key pressed: '%c'\r\n\r\n", receivedChar);
    UART_writeString(uartBuffer);
}

//*****************************************************************************
// Stop EPWM1
//*****************************************************************************
void stopEPWM1(void)
{
    //
    // Disable the time base clock sync (stops all EPWMs)
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    
    //
    // Small delay to ensure EPWM has stopped
    //
    DEVICE_DELAY_US(100);
    
    //
    // Reset EPWM1 counter to zero for next sync
    //
    EPWM_setTimeBaseCounter(myEPWM0_BASE, 0);
    
    systemSynced = 0;
}

//*****************************************************************************
// Trigger System Synchronization (One-Shot) - Restarts EPWM1
//*****************************************************************************
void triggerSystemSync(void)
{
    //
    // Reset EPWM1 counter to ensure it starts from zero
    //
    EPWM_setTimeBaseCounter(myEPWM0_BASE, 0);
    
    //
    // Reset EPWM2 counter
    //
    EPWM_setTimeBaseCounter(myEPWM1_BASE, 0);
    
    //
    // Clear any pending ADC interrupts
    //
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    
    //
    // Enable EPWM2 clock temporarily for sync pulse
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM2);
    DEVICE_DELAY_US(10);
    
    //
    // Enable TBCLKSYNC to allow EPWM modules to run
    // This starts EPWM1 from counter=0, synchronized with EPWM2
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    
    //
    // Wait for EPWM2 to complete one full period
    // EPWM2 period = 1999 counts at 100MHz = ~20us
    // During this period, EPWM2 generates SOCA trigger at period match
    // This triggers the first ADC conversion, synchronized with EPWM1 start
    //
    DEVICE_DELAY_US(50);
    
    //
    // Disable EPWM2 - one-shot sync pulse complete
    // EPWM1 continues running independently from counter=0
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_EPWM2);
    
    //
    // Mark system as synchronized
    //
    systemSynced = 1;
}

//*****************************************************************************
// Set ADC Acquisition Window
//*****************************************************************************
void setAcquisitionWindow(uint16_t cycles)
{
    //
    // Reconfigure ADC SOC0 with new acquisition window
    // Note: ADC trigger remains EPWM2_SOCA (set in board.c)
    // but we use software trigger during actual testing
    //
    ADC_setupSOC(myADC0_BASE, 
                 ADC_SOC_NUMBER0, 
                 ADC_TRIGGER_EPWM1_SOCA,  // Keep EPWM2 trigger for sync
                 ADC_CH_ADCIN0, 
                 cycles);                  // Variable acquisition window
    
    //
    // Clear any pending interrupts
    //
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    
    //
    // Small delay for ADC to stabilize after reconfiguration
    //
    DEVICE_DELAY_US(100);
}

//*****************************************************************************
// Run Single Test (50 samples using circular buffer)
//*****************************************************************************
void runSingleTest(uint16_t testNumber)
{
    uint32_t timeout;
    
    sprintf(uartBuffer, "  Test %2u/%u: ", testNumber + 1, TESTS_PER_WINDOW);
    UART_writeString(uartBuffer);
    
    //
    // Reset circular buffer and counters
    //
    sampleCount = 0;
    resultsIndex = 0;
    conversionComplete = 0;
    
    //
    // Turn on LED during sampling
    //
    GPIO_writePin(myBoardLED0_GPIO, 0);
    
    //
    // Collect 50 samples using software trigger
    //
    while(sampleCount < RESULTS_BUFFER_SIZE)
    {
        // Trigger ADC conversion via software
        ADC_forceSOC(myADC0_BASE, ADC_SOC_NUMBER0);
        
        // Wait for conversion complete with timeout
        timeout = 0;
        while(conversionComplete == 0)
        {
            timeout++;
            if(timeout > 1000000)
            {
                UART_writeString("\r\nERROR: ADC timeout!\r\n\r\n");
                while(1);  // Halt for debugging
            }
        }
        conversionComplete = 0;
        
        // Delay between samples
        DEVICE_DELAY_US(SAMPLE_DELAY_US);
    }
    
    //
    // Turn off LED
    //
    GPIO_writePin(myBoardLED0_GPIO, 1);
    
    //
    // Calculate statistics for this test
    //
    calculateStatistics(&testResults[testNumber]);
    
    //
    // Display quick result
    //
    displayTestResult(testNumber, &testResults[testNumber]);
}

//*****************************************************************************
// ADC Interrupt Service Routine
//*****************************************************************************
__interrupt void INT_myADC0_1_ISR(void)
{
    //
    // Read ADC result into circular buffer
    //
    adcResults[resultsIndex] = ADC_readResult(myADC0_RESULT_BASE, ADC_SOC_NUMBER0);
    
    //
    // Increment counters
    //
    resultsIndex++;
    sampleCount++;
    
    //
    // Wrap buffer index (circular)
    //
    if(resultsIndex >= RESULTS_BUFFER_SIZE)
    {
        resultsIndex = 0;
    }
    
    //
    // Clear interrupt flags
    //
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INT_myADC0_1_INTERRUPT_ACK_GROUP);
    
    //
    // Signal completion
    //
    conversionComplete = 1;
}

//*****************************************************************************
// Calculate Statistics for Current Test
//*****************************************************************************
void calculateStatistics(WindowStats* stats)
{
    uint16_t i;
    uint32_t sum = 0;
    
    //
    // Initialize
    //
    stats->min = 0xFFFF;
    stats->max = 0;
    
    //
    // Find min, max, and sum
    //
    for(i = 0; i < RESULTS_BUFFER_SIZE; i++)
    {
        uint16_t value = adcResults[i];
        
        sum += value;
        
        if(value < stats->min)
        {
            stats->min = value;
        }
        
        if(value > stats->max)
        {
            stats->max = value;
        }
    }
    
    //
    // Calculate average and range
    //
    stats->avg = sum / RESULTS_BUFFER_SIZE;
    stats->range = stats->max - stats->min;
    
    //
    // Calculate standard deviation
    //
    stats->stdDev = calculateStdDev(stats->avg);
}

//*****************************************************************************
// Calculate Standard Deviation
//*****************************************************************************
float calculateStdDev(uint16_t avg)
{
    uint16_t i;
    float variance = 0.0f;
    float avgFloat = (float)avg;
    
    //
    // Calculate variance
    //
    for(i = 0; i < RESULTS_BUFFER_SIZE; i++)
    {
        float diff = (float)adcResults[i] - avgFloat;
        variance += (diff * diff);
    }
    
    variance /= RESULTS_BUFFER_SIZE;
    
    //
    // Calculate square root using Newton's method
    //
    float result = 0.0f;
    
    if(variance > 0.01f)
    {
        result = variance / 2.0f;  // Initial guess
        
        for(i = 0; i < 10; i++)
        {
            if(result > 0.0f)
            {
                result = (result + variance / result) / 2.0f;
            }
        }
    }
    
    return result;
}

//*****************************************************************************
// Calculate Average of 10 Tests for Current Window
//*****************************************************************************
void calculateWindowAverage(uint16_t windowIndex)
{
    uint16_t i;
    uint32_t sumMin = 0, sumMax = 0, sumAvg = 0, sumRange = 0;
    float sumStdDev = 0.0f;
    
    //
    // Sum all 10 test results
    //
    for(i = 0; i < TESTS_PER_WINDOW; i++)
    {
        sumMin += testResults[i].min;
        sumMax += testResults[i].max;
        sumAvg += testResults[i].avg;
        sumRange += testResults[i].range;
        sumStdDev += testResults[i].stdDev;
    }
    
    //
    // Calculate averages and store in window results array
    //
    windowResults[windowIndex].windowCycles = ACQ_WINDOW_START + windowIndex;
    windowResults[windowIndex].min = sumMin / TESTS_PER_WINDOW;
    windowResults[windowIndex].max = sumMax / TESTS_PER_WINDOW;
    windowResults[windowIndex].avg = sumAvg / TESTS_PER_WINDOW;
    windowResults[windowIndex].range = sumRange / TESTS_PER_WINDOW;
    windowResults[windowIndex].stdDev = sumStdDev / TESTS_PER_WINDOW;
}

//*****************************************************************************
// Display Single Test Result
//*****************************************************************************
void displayTestResult(uint16_t testNumber, WindowStats* stats)
{
    uint16_t stdDevInt = (uint16_t)stats->stdDev;
    uint16_t stdDevFrac = (uint16_t)((stats->stdDev - stdDevInt) * 100);
    
    sprintf(uartBuffer, 
            "Avg:%4u Range:%3u StdDev:%u.%02u\r\n",
            stats->avg,
            stats->range,
            stdDevInt,
            stdDevFrac);
    UART_writeString(uartBuffer);
}

//*****************************************************************************
// Display Window Summary (Average of 10 Tests)
//*****************************************************************************
void displayWindowSummary(uint16_t windowIndex)
{
    uint16_t stdDevInt = (uint16_t)windowResults[windowIndex].stdDev;
    uint16_t stdDevFrac = (uint16_t)((windowResults[windowIndex].stdDev - stdDevInt) * 100);
    
    UART_writeString("\r\n  ==> WINDOW AVERAGE: ");
    sprintf(uartBuffer, 
            "Min:%4u Max:%4u Avg:%4u Range:%3u StdDev:%u.%02u\r\n",
            windowResults[windowIndex].min,
            windowResults[windowIndex].max,
            windowResults[windowIndex].avg,
            windowResults[windowIndex].range,
            stdDevInt,
            stdDevFrac);
    UART_writeString(uartBuffer);
}

//*****************************************************************************
// Display Final Summary Table
//*****************************************************************************
void displayFinalTable(void)
{
    uint16_t i;
    
    UART_writeString("\r\n\r\n");
    UART_writeString("========================================================\r\n");
    UART_writeString("       ACQUISITION WINDOW SWEEP - FINAL RESULTS\r\n");
    UART_writeString("========================================================\r\n\r\n");
    
    UART_writeString(" Cycles |  Time  | Min  | Max  | Value  \r\n");
    UART_writeString("--------|--------|------|------|------\r\n");
    
    for(i = 0; i < NUM_WINDOWS; i++)
    {
        uint16_t stdDevInt = (uint16_t)windowResults[i].stdDev;
        uint16_t stdDevFrac = (uint16_t)((windowResults[i].stdDev - stdDevInt) * 100);
        
        sprintf(uartBuffer, "   %2u   | %4uns | %4u | %4u | %4u\r\n",
                windowResults[i].windowCycles,
                windowResults[i].windowCycles * 5,
                windowResults[i].min,
                windowResults[i].max,
                windowResults[i].avg
                //windowResults[i].range
                //stdDevInt,
                //stdDevFrac
                );
        UART_writeString(uartBuffer);
    }
    
    UART_writeString("========================================================\r\n");
}

//*****************************************************************************
// UART Helper Functions
//*****************************************************************************
void UART_writeString(const char* str)
{
    uint16_t i = 0;
    
    while(str[i] != '\0')
    {
        SCI_writeCharBlockingFIFO(mySCI0_BASE, str[i]);
        i++;
    }
}

char UART_readChar(void)
{
    return SCI_readCharBlockingFIFO(mySCI0_BASE);
}

//*****************************************************************************
// Millisecond Delay Function
//*****************************************************************************
void delayMs(uint16_t ms)
{
    uint16_t i;
    
    for(i = 0; i < ms; i++)
    {
        DEVICE_DELAY_US(1000);
    }
}
