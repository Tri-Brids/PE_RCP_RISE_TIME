
/********************************************************************************
 * Includes
 *******************************************************************************/
#include "Test_0_08_multi_ADC.h"

/********************************************************************************
 * Defines
 *******************************************************************************/


/********************************************************************************
 * Typedefs
 *******************************************************************************/


/********************************************************************************
 * Structures
 *******************************************************************************/

/********************************************************************************
 * Static Variables
 *******************************************************************************/

/********************************************************************************
 * Global Variables
 *******************************************************************************/


/********************************************************************************
 * Function Prototypes
 *******************************************************************************/

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
// Stop Both EPWMs
//*****************************************************************************
void stopEPWMs(void)
{
    //
    // Disable the time base clock sync (stops all EPWMs)
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    
    //
    // Small delay to ensure EPWMs have stopped
    //
    DEVICE_DELAY_US(100);
    
    //
    // Reset both EPWM counters to zero for next sync
    //
    EPWM_setTimeBaseCounter(myEPWM0_BASE, 0);  // EPWM1

    
    systemSynced = 0;
}

//*****************************************************************************
// Trigger System Synchronization - Restarts Both EPWMs from zero
//*****************************************************************************
void triggerSystemSync(void)
{
    //
    // Reset both EPWM counters to ensure they start from zero
    //
    EPWM_setTimeBaseCounter(myEPWM0_BASE, 0);  // EPWM1
    
    //
    // Clear any pending ADC interrupts
    //
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER3);
    
    //
    // Enable TBCLKSYNC to allow EPWM modules to run
    // This starts both EPWMs from counter=0 simultaneously
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    
    //
    // Wait for EPWMs to stabilize
    //
    // DEVICE_DELAY_US(50);
    
    //
    // Mark system as synchronized
    //
    systemSynced = 1;
}

//*****************************************************************************
// Set ADC Acquisition Window (Both ADCs)
//*****************************************************************************
void setAcquisitionWindow(uint16_t cycles)
{
    //
    // Reconfigure ADC-A SOC0 with new acquisition window
    //
    ADC_setupSOC(myADC0_BASE, 
                 ADC_SOC_NUMBER0, 
                 ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN0, 
                 cycles);
    
    //
    // Reconfigure ADC-A SOC1 with new acquisition window
    //
    ADC_setupSOC(myADC0_BASE, 
                 ADC_SOC_NUMBER1, 
                 ADC_TRIGGER_EPWM1_SOCB,
                 ADC_CH_ADCIN2, 
                 cycles);

    
    //
    // Reconfigure ADC-A SOC2 with new acquisition window
    //
    ADC_setupSOC(myADC0_BASE, 
                 ADC_SOC_NUMBER2, 
                 ADC_TRIGGER_EPWM2_SOCA,
                 ADC_CH_ADCIN4, 
                 cycles);
    
    //
    // Clear any pending interrupts
    //
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER3);
    
    //
    // Small delay for ADCs to stabilize after reconfiguration
    //
    DEVICE_DELAY_US(100);
}

//*****************************************************************************
// Run Single Test (50 samples per ADC using circular buffers)
//*****************************************************************************
void runSingleTest(uint16_t testNumber)
{
    uint32_t timeout;
    
    sprintf(uartBuffer, " Sampling both ADCs...");
    UART_writeString(uartBuffer);
    
    //
    // Reset circular buffers and counters for both ADCs
    //
    adcASampleCount = 0;
    adcBSampleCount = 0;
    adcCSampleCount = 0;
    adcAIndex = 0;
    adcBIndex = 0;
    adcCIndex = 0;
    adcAComplete = 0;
    adcBComplete = 0;
    adcCComplete = 0;
    
    //
    // Turn on LED during sampling
    //
    GPIO_writePin(myBoardLED0_GPIO, 0);

    
    triggerSystemSync();
    //
    // Collect 50 samples from both ADCs using software trigger
    //
    while(adcASampleCount < RESULTS_BUFFER_SIZE || adcBSampleCount < RESULTS_BUFFER_SIZE || adcCSampleCount < RESULTS_BUFFER_SIZE)
    {
        // Trigger ADC-A conversion if not complete
        if(adcASampleCount < RESULTS_BUFFER_SIZE)
        {
            ADC_forceSOC(myADC0_BASE, ADC_SOC_NUMBER0);
            
            // Wait for ADC-A conversion complete with timeout
            timeout = 0;
            while(adcAComplete == 0)
            {
                timeout++;
                if(timeout > 1000000)
                {
                    UART_writeString("\r\nERROR: ADC-A timeout!\r\n\r\n");
                    while(1);
                }
            }
            adcAComplete = 0;
        }
        

        // Trigger ADC-B conversion if not complete
        if(adcBSampleCount < RESULTS_BUFFER_SIZE)
        {
            ADC_forceSOC(myADC0_BASE, ADC_SOC_NUMBER1);
            
            // Wait for ADC-B conversion complete with timeout
            timeout = 0;
            while(adcBComplete == 0)
            {
                timeout++;
                if(timeout > 1000000)
                {
                    UART_writeString("\r\nERROR: ADC-B timeout!\r\n\r\n");
                    while(1);
                }
            }
            adcBComplete = 0;
        }

        // Trigger ADC-C conversion if not complete
        if(adcCSampleCount < RESULTS_BUFFER_SIZE)
        {
            ADC_forceSOC(myADC0_BASE, ADC_SOC_NUMBER2);
            
            // Wait for ADC-C conversion complete with timeout
            timeout = 0;
            while(adcCComplete == 0)
            {
                timeout++;
                if(timeout > 1000000)
                {
                    UART_writeString("\r\nERROR: ADC-C timeout!\r\n\r\n");
                    while(1);
                }
            }
            adcCComplete = 0;
        }
        
        // Delay between samples
        DEVICE_DELAY_US(SAMPLE_DELAY_US);
    }
    
    //
    // Turn off LED
    //
    GPIO_writePin(myBoardLED0_GPIO, 1);
    
    //
    // Calculate statistics for this test (both ADCs)
    //
    calculateStatistics(adcAResults, &adcATestResults[testNumber]);
    calculateStatistics(adcBResults, &adcBTestResults[testNumber]);
    calculateStatistics(adcCResults, &adcCTestResults[testNumber]);
    
    //
    // Display quick results
    //
    displayTestResult(testNumber, 'A', &adcATestResults[testNumber]);
    displayTestResult(testNumber, 'B', &adcBTestResults[testNumber]);
    displayTestResult(testNumber, 'V', &adcCTestResults[testNumber]);
}

//*****************************************************************************
// ADC-A Interrupt Service Routine
//*****************************************************************************
__interrupt void INT_myADC0_1_ISR(void)
{
    //
    // Read ADC-A result into circular buffer
    //
    adcAResults[adcAIndex] = ADC_readResult(myADC0_RESULT_BASE, ADC_SOC_NUMBER0);
    
    //
    // Increment counters
    //
    adcAIndex++;
    adcASampleCount++;
    
    //
    // Wrap buffer index (circular)
    //
    if(adcAIndex >= RESULTS_BUFFER_SIZE)
    {
        adcAIndex = 0;
    }
    
    //
    // Clear interrupt flags
    //
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INT_myADC0_1_INTERRUPT_ACK_GROUP);
    
    //
    // Signal completion
    //
    adcAComplete = 1;
}

//*****************************************************************************
// ADC-B Interrupt Service Routine
//*****************************************************************************
__interrupt void INT_myADC0_2_ISR(void)
{
    //
    // Read ADC-B result into circular buffer
    //
    adcBResults[adcBIndex] = ADC_readResult(myADC0_RESULT_BASE, ADC_SOC_NUMBER1);
    
    //
    // Increment counters
    //
    adcBIndex++;
    adcBSampleCount++;
    
    //
    // Wrap buffer index (circular)
    //
    if(adcBIndex >= RESULTS_BUFFER_SIZE)
    {
        adcBIndex = 0;
    }
    
    //
    // Clear interrupt flags
    //
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER2);
    Interrupt_clearACKGroup(INT_myADC0_2_INTERRUPT_ACK_GROUP);
    
    //
    // Signal completion
    //
    adcBComplete = 1;
}

//*****************************************************************************
// ADC-C Interrupt Service Routine
//*****************************************************************************
__interrupt void INT_myADC0_3_ISR(void)
{
    //
    // Read ADC-C result into circular buffer
    //
    adcCResults[adcCIndex] = ADC_readResult(myADC0_RESULT_BASE, ADC_SOC_NUMBER2);
    
    //
    // Increment counters
    //
    adcCIndex++;
    adcCSampleCount++;
    
    //
    // Wrap buffer index (circular)
    //
    if(adcCIndex >= RESULTS_BUFFER_SIZE)
    {
        adcCIndex = 0;
    }
    
    //
    // Clear interrupt flags
    //
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER3);
    Interrupt_clearACKGroup(INT_myADC0_3_INTERRUPT_ACK_GROUP);
    
    //
    // Signal completion
    //
    adcCComplete = 1;
}

//*****************************************************************************
// Calculate Statistics for Current Test
//*****************************************************************************
void calculateStatistics(volatile uint16_t* results, WindowStats* stats)
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
        uint16_t value = results[i];
        
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
    stats->stdDev = calculateStdDev(results, stats->avg);
}

//*****************************************************************************
// Calculate Standard Deviation
//*****************************************************************************
float calculateStdDev(volatile uint16_t* results, uint16_t avg)
{
    uint16_t i;
    float variance = 0.0f;
    float avgFloat = (float)avg;
    
    //
    // Calculate variance
    //
    for(i = 0; i < RESULTS_BUFFER_SIZE; i++)
    {
        float diff = (float)results[i] - avgFloat;
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
    uint32_t sumMinA = 0, sumMaxA = 0, sumAvgA = 0, sumRangeA = 0;
    uint32_t sumMinB = 0, sumMaxB = 0, sumAvgB = 0, sumRangeB = 0;
    float sumStdDevA = 0.0f;
    float sumStdDevB = 0.0f;
    
    //
    // Sum all 10 test results for both ADCs
    //
    for(i = 0; i < TESTS_PER_WINDOW; i++)
    {
        // ADC-A
        sumMinA += adcATestResults[i].min;
        sumMaxA += adcATestResults[i].max;
        sumAvgA += adcATestResults[i].avg;
        sumRangeA += adcATestResults[i].range;
        sumStdDevA += adcATestResults[i].stdDev;
        
        // ADC-B
        sumMinB += adcBTestResults[i].min;
        sumMaxB += adcBTestResults[i].max;
        sumAvgB += adcBTestResults[i].avg;
        sumRangeB += adcBTestResults[i].range;
        sumStdDevB += adcBTestResults[i].stdDev;
    }
    
    //
    // Calculate averages and store in window results arrays
    //
    // ADC-A
    adcAWindowResults[windowIndex].windowCycles = ACQ_WINDOW_START + windowIndex;
    adcAWindowResults[windowIndex].min = sumMinA / TESTS_PER_WINDOW;
    adcAWindowResults[windowIndex].max = sumMaxA / TESTS_PER_WINDOW;
    adcAWindowResults[windowIndex].avg = sumAvgA / TESTS_PER_WINDOW;
    adcAWindowResults[windowIndex].range = sumRangeA / TESTS_PER_WINDOW;
    adcAWindowResults[windowIndex].stdDev = sumStdDevA / TESTS_PER_WINDOW;
    
    // ADC-B
    adcBWindowResults[windowIndex].windowCycles = ACQ_WINDOW_START + windowIndex;
    adcBWindowResults[windowIndex].min = sumMinB / TESTS_PER_WINDOW;
    adcBWindowResults[windowIndex].max = sumMaxB / TESTS_PER_WINDOW;
    adcBWindowResults[windowIndex].avg = sumAvgB / TESTS_PER_WINDOW;
    adcBWindowResults[windowIndex].range = sumRangeB / TESTS_PER_WINDOW;
    adcBWindowResults[windowIndex].stdDev = sumStdDevB / TESTS_PER_WINDOW;
}

//*****************************************************************************
// Display Single Test Result
//*****************************************************************************
void displayTestResult(uint16_t testNumber, char adcName, WindowStats* stats)
{
    uint16_t stdDevInt = (uint16_t)stats->stdDev;
    uint16_t stdDevFrac = (uint16_t)((stats->stdDev - stdDevInt) * 100);
    
    sprintf(uartBuffer, 
            "     ADC-%c: Avg:%4u Range:%3u StdDev:%u.%02u\r\n",
            adcName,
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
    uint16_t stdDevIntA = (uint16_t)adcAWindowResults[windowIndex].stdDev;
    uint16_t stdDevFracA = (uint16_t)((adcAWindowResults[windowIndex].stdDev - stdDevIntA) * 100);
    
    uint16_t stdDevIntB = (uint16_t)adcBWindowResults[windowIndex].stdDev;
    uint16_t stdDevFracB = (uint16_t)((adcBWindowResults[windowIndex].stdDev - stdDevIntB) * 100);
    
    UART_writeString("\r\n  ==> WINDOW AVERAGES:\r\n");
    
    sprintf(uartBuffer, 
            "      ADC-A: Min:%4u Max:%4u Avg:%4u Range:%3u StdDev:%u.%02u\r\n",
            adcAWindowResults[windowIndex].min,
            adcAWindowResults[windowIndex].max,
            adcAWindowResults[windowIndex].avg,
            adcAWindowResults[windowIndex].range,
            stdDevIntA,
            stdDevFracA);
    UART_writeString(uartBuffer);
    
    sprintf(uartBuffer, 
            "      ADC-B: Min:%4u Max:%4u Avg:%4u Range:%3u StdDev:%u.%02u\r\n",
            adcBWindowResults[windowIndex].min,
            adcBWindowResults[windowIndex].max,
            adcBWindowResults[windowIndex].avg,
            adcBWindowResults[windowIndex].range,
            stdDevIntB,
            stdDevFracB);
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
    UART_writeString("       ADC-A ACQUISITION WINDOW SWEEP RESULTS\r\n");
    UART_writeString("========================================================\r\n\r\n");
    
    UART_writeString(" Cycles |  Time  | Min  | Max  | Avg  | Range | StdDev\r\n");
    UART_writeString("--------|--------|------|------|------|-------|--------\r\n");
    
    for(i = 0; i < NUM_WINDOWS; i++)
    {
        uint16_t stdDevInt = (uint16_t)adcAWindowResults[i].stdDev;
        uint16_t stdDevFrac = (uint16_t)((adcAWindowResults[i].stdDev - stdDevInt) * 100);
        
        sprintf(uartBuffer, "   %2u   | %4uns | %4u | %4u | %4u |  %3u  | %2u.%02u\r\n",
                adcAWindowResults[i].windowCycles,
                adcAWindowResults[i].windowCycles * 5,
                adcAWindowResults[i].min,
                adcAWindowResults[i].max,
                adcAWindowResults[i].avg,
                adcAWindowResults[i].range,
                stdDevInt,
                stdDevFrac);
        UART_writeString(uartBuffer);
    }
    
    UART_writeString("========================================================\r\n");
    
    UART_writeString("\r\n\r\n");
    UART_writeString("========================================================\r\n");
    UART_writeString("       ADC-B ACQUISITION WINDOW SWEEP RESULTS\r\n");
    UART_writeString("========================================================\r\n\r\n");
    
    UART_writeString(" Cycles |  Time  | Min  | Max  | Avg  | Range | StdDev\r\n");
    UART_writeString("--------|--------|------|------|------|-------|--------\r\n");
    
    for(i = 0; i < NUM_WINDOWS; i++)
    {
        uint16_t stdDevInt = (uint16_t)adcBWindowResults[i].stdDev;
        uint16_t stdDevFrac = (uint16_t)((adcBWindowResults[i].stdDev - stdDevInt) * 100);
        
        sprintf(uartBuffer, "   %2u   | %4uns | %4u | %4u | %4u |  %3u  | %2u.%02u\r\n",
                adcBWindowResults[i].windowCycles,
                adcBWindowResults[i].windowCycles * 5,
                adcBWindowResults[i].min,
                adcBWindowResults[i].max,
                adcBWindowResults[i].avg,
                adcBWindowResults[i].range,
                stdDevInt,
                stdDevFrac);
        UART_writeString(uartBuffer);
    }
    
    UART_writeString("========================================================\r\n");
}

//*****************************************************************************
// Display Comparison Analysis
//*****************************************************************************
void displayComparison(void)
{
    uint16_t i;
    int16_t avgDiff, rangeDiff;
    
    UART_writeString("\r\n\r\n");
    UART_writeString("========================================================\r\n");
    UART_writeString("       ADC-A vs ADC-B COMPARISON\r\n");
    UART_writeString("========================================================\r\n\r\n");
    
    UART_writeString(" Cycles | ADC-A Avg | ADC-B Avg | Diff | A Range | B Range\r\n");
    UART_writeString("--------|-----------|-----------|------|---------|--------\r\n");
    
    for(i = 0; i < NUM_WINDOWS; i++)
    {
        avgDiff = (int16_t)adcAWindowResults[i].avg - (int16_t)adcBWindowResults[i].avg;
        rangeDiff = (int16_t)adcAWindowResults[i].range - (int16_t)adcBWindowResults[i].range;
        
        sprintf(uartBuffer, "   %2u   |   %4u    |   %4u    | %+4d |   %3u   |  %3u\r\n",
                adcAWindowResults[i].windowCycles,
                adcAWindowResults[i].avg,
                adcBWindowResults[i].avg,
                avgDiff,
                adcAWindowResults[i].range,
                adcBWindowResults[i].range);
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

/********************************************************************************
 * Code
 *******************************************************************************/
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
    // Wait for user to start
    //
    UART_writeString("Press ANY KEY to start dual ADC sweep test...\r\n\r\n");
    waitForKeyPress();
    
    UART_writeString("Starting dual ADC acquisition window sweep...\r\n");
    UART_writeString("========================================================\r\n");
    
    //
    // Main sweep loop: Test each acquisition window from 1 to 20 cycles
    //
    for(i = 0; i < NUM_WINDOWS; i++)
    {
        currentWindow = ACQ_WINDOW_START + i;

                stopEPWMs();
                delayMs(5);
   
        //
        // Set new acquisition window (once per window, same for both ADCs)
        //
        setAcquisitionWindow(currentWindow);
        delayMs(10);
        
        //
        // Run 10 tests with synchronization before EACH test

            //
            // Trigger synchronization - restarts both EPWMs from zero
            //
          //  sprintf(uartBuffer, "  -> Test %2u: Syncing EPWMs...", j + 1);
          //  UART_writeString(uartBuffer);
           // triggerSystemSync();
            //delayMs(10);
        //
        for(j = 0; j < TESTS_PER_WINDOW; j++)
        {
            //triggerSystemSync();
            runSingleTest(j);
            delayMs(30);
        }
        
        //
        // Calculate average of 10 tests for this window (both ADCs)
        //
        calculateWindowAverage(i);
        
        //
        // Display summary for this window
        //
       // displayWindowSummary(i);
        
        delayMs(100);
    }
    
    //
    // Stop EPWMs after all tests complete
    //
   // UART_writeString("\r\n-> Stopping EPWMs...\r\n");
    stopEPWMs();
    //UART_writeString("-> EPWMs stopped.\r\n");
    
    //
    // Display final summary tables
    //
    displayFinalTable();
    
    //
    // Display comparison analysis
    //
    displayComparison();
    
    //
    // Test complete
    //
    
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



/* EOF */
