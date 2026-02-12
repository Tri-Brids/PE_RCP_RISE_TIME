/********************************************************************************
 * Includes
 *******************************************************************************/
#include "zero.h"

/********************************************************************************
 * Global Variable Definitions
 * Declared as extern in the .h - defined exactly once here
 *******************************************************************************/

/* ADC0 */
volatile uint16_t adc0Results[ADC0_NUM_CH][RESULTS_BUFFER_SIZE];
volatile uint16_t adc0Index[ADC0_NUM_CH]       = {0, 0, 0};
volatile uint16_t adc0SampleCount[ADC0_NUM_CH] = {0, 0, 0};
volatile uint16_t adc0Complete[ADC0_NUM_CH]    = {0, 0, 0};

/* ADC1 */
volatile uint16_t adc1Results[ADC1_NUM_CH][RESULTS_BUFFER_SIZE];
volatile uint16_t adc1Index[ADC1_NUM_CH]       = {0, 0, 0};
volatile uint16_t adc1SampleCount[ADC1_NUM_CH] = {0, 0, 0};
volatile uint16_t adc1Complete[ADC1_NUM_CH]    = {0, 0, 0};

/* ADC2 */
volatile uint16_t adc2Results[ADC2_NUM_CH][RESULTS_BUFFER_SIZE];
volatile uint16_t adc2Index[ADC2_NUM_CH]       = {0, 0};
volatile uint16_t adc2SampleCount[ADC2_NUM_CH] = {0, 0};
volatile uint16_t adc2Complete[ADC2_NUM_CH]    = {0, 0};

/* ADC3 */
volatile uint16_t adc3Results[ADC3_NUM_CH][RESULTS_BUFFER_SIZE];
volatile uint16_t adc3Index[ADC3_NUM_CH]       = {0, 0, 0, 0};
volatile uint16_t adc3SampleCount[ADC3_NUM_CH] = {0, 0, 0, 0};
volatile uint16_t adc3Complete[ADC3_NUM_CH]    = {0, 0, 0, 0};

volatile uint16_t systemSynced = 0;

WindowStats adc0TestResults[ADC0_NUM_CH][TESTS_PER_WINDOW];
WindowStats adc1TestResults[ADC1_NUM_CH][TESTS_PER_WINDOW];
WindowStats adc2TestResults[ADC2_NUM_CH][TESTS_PER_WINDOW];
WindowStats adc3TestResults[ADC3_NUM_CH][TESTS_PER_WINDOW];

WindowStats adc0WindowResults[ADC0_NUM_CH][NUM_WINDOWS];
WindowStats adc1WindowResults[ADC1_NUM_CH][NUM_WINDOWS];
WindowStats adc2WindowResults[ADC2_NUM_CH][NUM_WINDOWS];
WindowStats adc3WindowResults[ADC3_NUM_CH][NUM_WINDOWS];

char uartBuffer[256];

/********************************************************************************
 * Helper Macros
 * Generic ISR body - reads one SOC result into the correct channel array
 *******************************************************************************/
#define ADC_ISR_BODY(resultsArr, indexArr, countArr, completeArr, ch,   \
                     resultBase, socNum, adcBase, intNum, ackGroup)     \
    (resultsArr)[ch][(indexArr)[ch]] =                                  \
        ADC_readResult((resultBase), (socNum));                         \
    (indexArr)[ch]++;                                                   \
    (countArr)[ch]++;                                                   \
    if((indexArr)[ch] >= RESULTS_BUFFER_SIZE) (indexArr)[ch] = 0;      \
    ADC_clearInterruptStatus((adcBase), (intNum));                      \
    Interrupt_clearACKGroup((ackGroup));                                \
    (completeArr)[ch] = 1;

/********************************************************************************
 * UART Helpers
 *******************************************************************************/
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

void waitForKeyPress(void)
{
    char c = SCI_readCharBlockingFIFO(mySCI0_BASE);
    sprintf(uartBuffer, "Key pressed: '%c'\r\n\r\n", c);
    UART_writeString(uartBuffer);
}

void delayMs(uint16_t ms)
{
    uint16_t i;
    for(i = 0; i < ms; i++) DEVICE_DELAY_US(1000);
}

/********************************************************************************
 * EPWM Sync Control
 *******************************************************************************/
void stopEPWMs(void)
{
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    DEVICE_DELAY_US(100);
    EPWM_setTimeBaseCounter(myEPWM0_BASE, 0);
    systemSynced = 0;
}

void triggerSystemSync(void)
{
    EPWM_setTimeBaseCounter(myEPWM0_BASE, 0);

    /* Clear all ADC interrupt flags */
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER3);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER3);
    ADC_clearInterruptStatus(myADC2_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC2_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC3_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC3_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC3_BASE, ADC_INT_NUMBER3);
    ADC_clearInterruptStatus(myADC3_BASE, ADC_INT_NUMBER4);

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    systemSynced = 1;
}

/********************************************************************************
 * Statistics
 *******************************************************************************/
float calculateStdDev(volatile uint16_t* results, uint16_t avg)
{
    uint16_t i;
    float variance = 0.0f;
    float avgF     = (float)avg;
    float result   = 0.0f;

    for(i = 0; i < RESULTS_BUFFER_SIZE; i++)
    {
        float d = (float)results[i] - avgF;
        variance += d * d;
    }
    variance /= RESULTS_BUFFER_SIZE;

    if(variance > 0.01f)
    {
        result = variance / 2.0f;
        for(i = 0; i < 10; i++)
            if(result > 0.0f) result = (result + variance / result) / 2.0f;
    }
    return result;
}

void calculateStatistics(volatile uint16_t* results, WindowStats* stats)
{
    uint16_t i;
    uint32_t sum = 0;
    stats->min = 0xFFFF;
    stats->max = 0;

    for(i = 0; i < RESULTS_BUFFER_SIZE; i++)
    {
        uint16_t v = results[i];
        sum += v;
        if(v < stats->min) stats->min = v;
        if(v > stats->max) stats->max = v;
    }
    stats->avg    = (uint16_t)(sum / RESULTS_BUFFER_SIZE);
    stats->range  = stats->max - stats->min;
    stats->stdDev = calculateStdDev(results, stats->avg);
}

/********************************************************************************
 * Display Helpers
 *******************************************************************************/
void displayTestResult(uint16_t testNum, const char* label, WindowStats* stats)
{
    uint16_t sdI = (uint16_t)stats->stdDev;
    uint16_t sdF = (uint16_t)((stats->stdDev - sdI) * 100);
    sprintf(uartBuffer,
            "  [%2u] %-10s Avg:%4u Range:%3u StdDev:%u.%02u\r\n",
            testNum, label,
            stats->avg, stats->range, sdI, sdF);
    UART_writeString(uartBuffer);
}

/* Print one row of the summary table */
static void printTableRow(uint16_t winCycles, WindowStats* s)
{
    uint16_t sdI = (uint16_t)s->stdDev;
    uint16_t sdF = (uint16_t)((s->stdDev - sdI) * 100);
    sprintf(uartBuffer,
            "   %2u   | %4uns | %4u | %4u | %4u |  %3u  | %2u.%02u\r\n",
            winCycles, winCycles * 5,
            s->min, s->max, s->avg, s->range, sdI, sdF);
    UART_writeString(uartBuffer);
}

static void printTableHeader(const char* adcLabel, const char* chLabel)
{
    UART_writeString("\r\n");
    UART_writeString("========================================================\r\n");
    sprintf(uartBuffer, "  %s  %-10s  ACQUISITION WINDOW SWEEP\r\n",
            adcLabel, chLabel);
    UART_writeString(uartBuffer);
    UART_writeString("========================================================\r\n");
    UART_writeString(" Cycles |  Time  | Min  | Max  | Avg  | Range | StdDev\r\n");
    UART_writeString("--------|--------|------|------|------|-------|--------\r\n");
}

/********************************************************************************
 * Acquisition Window Setters
 *******************************************************************************/
void setAcquisitionWindowADC0(uint16_t cycles)
{
    ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN0, cycles);
    ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM1_SOCB, ADC_CH_ADCIN2, cycles);
    ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN4, cycles);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER3);
    DEVICE_DELAY_US(100);
}

void setAcquisitionWindowADC1(uint16_t cycles)
{
    ADC_setupSOC(myADC1_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_EPWM2_SOCB, ADC_CH_ADCIN0, cycles);
    ADC_setupSOC(myADC1_BASE, ADC_SOC_NUMBER8, ADC_TRIGGER_EPWM5_SOCA, ADC_CH_ADCIN2, cycles);
    ADC_setupSOC(myADC1_BASE, ADC_SOC_NUMBER9, ADC_TRIGGER_EPWM5_SOCB, ADC_CH_ADCIN4, cycles);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER3);
    DEVICE_DELAY_US(100);
}

void setAcquisitionWindowADC2(uint16_t cycles)
{
    ADC_setupSOC(myADC2_BASE, ADC_SOC_NUMBER10, ADC_TRIGGER_EPWM6_SOCA, ADC_CH_ADCIN2, cycles);
    ADC_setupSOC(myADC2_BASE, ADC_SOC_NUMBER11, ADC_TRIGGER_EPWM6_SOCB, ADC_CH_ADCIN4, cycles);
    ADC_clearInterruptStatus(myADC2_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC2_BASE, ADC_INT_NUMBER2);
    DEVICE_DELAY_US(100);
}

void setAcquisitionWindowADC3(uint16_t cycles)
{
    ADC_setupSOC(myADC3_BASE, ADC_SOC_NUMBER4, ADC_TRIGGER_EPWM3_SOCA, ADC_CH_ADCIN0, cycles);
    ADC_setupSOC(myADC3_BASE, ADC_SOC_NUMBER5, ADC_TRIGGER_EPWM3_SOCB, ADC_CH_ADCIN1, cycles);
    ADC_setupSOC(myADC3_BASE, ADC_SOC_NUMBER6, ADC_TRIGGER_EPWM4_SOCA, ADC_CH_ADCIN2, cycles);
    ADC_setupSOC(myADC3_BASE, ADC_SOC_NUMBER7, ADC_TRIGGER_EPWM4_SOCB, ADC_CH_ADCIN3, cycles);
    ADC_clearInterruptStatus(myADC3_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC3_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC3_BASE, ADC_INT_NUMBER3);
    ADC_clearInterruptStatus(myADC3_BASE, ADC_INT_NUMBER4);
    DEVICE_DELAY_US(100);
}

/********************************************************************************
* Reconfigure the SOCs to remaining ADC Pins and set their Acquisition Windows *
 *******************************************************************************/
void ReconfigureandsetAcquisitionWindowADC0(uint16_t cycles)
{
    ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA, ADC_CH_ADCIN1, cycles);
    ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM1_SOCB, ADC_CH_ADCIN3, cycles);
    ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN5, cycles);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER3);
    DEVICE_DELAY_US(100);
}

void ReconfigureandsetAcquisitionWindowADC1(uint16_t cycles)
{
    ADC_setupSOC(myADC1_BASE, ADC_SOC_NUMBER3, ADC_TRIGGER_EPWM2_SOCB, ADC_CH_ADCIN1, cycles);
    ADC_setupSOC(myADC1_BASE, ADC_SOC_NUMBER8, ADC_TRIGGER_EPWM5_SOCA, ADC_CH_ADCIN3, cycles);
    ADC_setupSOC(myADC1_BASE, ADC_SOC_NUMBER9, ADC_TRIGGER_EPWM5_SOCB, ADC_CH_ADCIN5, cycles);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER3);
    DEVICE_DELAY_US(100);
}

void ReconfigureandsetAcquisitionWindowADC2(uint16_t cycles)
{
    ADC_setupSOC(myADC2_BASE, ADC_SOC_NUMBER10, ADC_TRIGGER_EPWM6_SOCA, ADC_CH_ADCIN3, cycles);
    ADC_setupSOC(myADC2_BASE, ADC_SOC_NUMBER11, ADC_TRIGGER_EPWM6_SOCB, ADC_CH_ADCIN5, cycles);
    ADC_clearInterruptStatus(myADC2_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(myADC2_BASE, ADC_INT_NUMBER2);
    DEVICE_DELAY_US(100);
}

void ReconfigureandsetAcquisitionWindowADC3(uint16_t cycles)
{

}

/********************************************************************************
 * ISRs - ADC0  (ch0=SOC0, ch1=SOC1, ch2=SOC2)
 *******************************************************************************/
__interrupt void INT_myADC0_1_ISR(void)
{
    ADC_ISR_BODY(adc0Results, adc0Index, adc0SampleCount, adc0Complete,
                 0, myADC0_RESULT_BASE, ADC_SOC_NUMBER0,
                 myADC0_BASE, ADC_INT_NUMBER1,
                 INT_myADC0_1_INTERRUPT_ACK_GROUP)
}

__interrupt void INT_myADC0_2_ISR(void)
{
    ADC_ISR_BODY(adc0Results, adc0Index, adc0SampleCount, adc0Complete,
                 1, myADC0_RESULT_BASE, ADC_SOC_NUMBER1,
                 myADC0_BASE, ADC_INT_NUMBER2,
                 INT_myADC0_2_INTERRUPT_ACK_GROUP)
}

__interrupt void INT_myADC0_3_ISR(void)
{
    ADC_ISR_BODY(adc0Results, adc0Index, adc0SampleCount, adc0Complete,
                 2, myADC0_RESULT_BASE, ADC_SOC_NUMBER2,
                 myADC0_BASE, ADC_INT_NUMBER3,
                 INT_myADC0_3_INTERRUPT_ACK_GROUP)
}

/********************************************************************************
 * ISRs - ADC1  (ch0=SOC3, ch1=SOC8, ch2=SOC9)
 *******************************************************************************/
__interrupt void INT_myADC1_1_ISR(void)
{
    ADC_ISR_BODY(adc1Results, adc1Index, adc1SampleCount, adc1Complete,
                 0, myADC1_RESULT_BASE, ADC_SOC_NUMBER3,
                 myADC1_BASE, ADC_INT_NUMBER1,
                 INT_myADC1_1_INTERRUPT_ACK_GROUP)
}

__interrupt void INT_myADC1_2_ISR(void)
{
    ADC_ISR_BODY(adc1Results, adc1Index, adc1SampleCount, adc1Complete,
                 1, myADC1_RESULT_BASE, ADC_SOC_NUMBER8,
                 myADC1_BASE, ADC_INT_NUMBER2,
                 INT_myADC1_2_INTERRUPT_ACK_GROUP)
}

__interrupt void INT_myADC1_3_ISR(void)
{
    ADC_ISR_BODY(adc1Results, adc1Index, adc1SampleCount, adc1Complete,
                 2, myADC1_RESULT_BASE, ADC_SOC_NUMBER9,
                 myADC1_BASE, ADC_INT_NUMBER3,
                 INT_myADC1_3_INTERRUPT_ACK_GROUP)
}

/********************************************************************************
 * ISRs - ADC2  (ch0=SOC10, ch1=SOC11)
 *******************************************************************************/
__interrupt void INT_myADC2_1_ISR(void)
{
    ADC_ISR_BODY(adc2Results, adc2Index, adc2SampleCount, adc2Complete,
                 0, myADC2_RESULT_BASE, ADC_SOC_NUMBER10,
                 myADC2_BASE, ADC_INT_NUMBER1,
                 INT_myADC2_1_INTERRUPT_ACK_GROUP)
}

__interrupt void INT_myADC2_2_ISR(void)
{
    ADC_ISR_BODY(adc2Results, adc2Index, adc2SampleCount, adc2Complete,
                 1, myADC2_RESULT_BASE, ADC_SOC_NUMBER11,
                 myADC2_BASE, ADC_INT_NUMBER2,
                 INT_myADC2_2_INTERRUPT_ACK_GROUP)
}

/********************************************************************************
 * ISRs - ADC3  (ch0=SOC4, ch1=SOC5, ch2=SOC6, ch3=SOC7)
 *******************************************************************************/
__interrupt void INT_myADC3_1_ISR(void)
{
    ADC_ISR_BODY(adc3Results, adc3Index, adc3SampleCount, adc3Complete,
                 0, myADC3_RESULT_BASE, ADC_SOC_NUMBER4,
                 myADC3_BASE, ADC_INT_NUMBER1,
                 INT_myADC3_1_INTERRUPT_ACK_GROUP)
}

__interrupt void INT_myADC3_2_ISR(void)
{
    ADC_ISR_BODY(adc3Results, adc3Index, adc3SampleCount, adc3Complete,
                 1, myADC3_RESULT_BASE, ADC_SOC_NUMBER5,
                 myADC3_BASE, ADC_INT_NUMBER2,
                 INT_myADC3_2_INTERRUPT_ACK_GROUP)
}

__interrupt void INT_myADC3_3_ISR(void)
{
    ADC_ISR_BODY(adc3Results, adc3Index, adc3SampleCount, adc3Complete,
                 2, myADC3_RESULT_BASE, ADC_SOC_NUMBER6,
                 myADC3_BASE, ADC_INT_NUMBER3,
                 INT_myADC3_3_INTERRUPT_ACK_GROUP)
}

__interrupt void INT_myADC3_4_ISR(void)
{
    ADC_ISR_BODY(adc3Results, adc3Index, adc3SampleCount, adc3Complete,
                 3, myADC3_RESULT_BASE, ADC_SOC_NUMBER7,
                 myADC3_BASE, ADC_INT_NUMBER4,
                 INT_myADC3_4_INTERRUPT_ACK_GROUP)
}

/********************************************************************************
 * Generic channel-polling helper used by all four runSingleTest functions.
 * Forces a software SOC, waits for the ISR to set completeFlag, clears it.
 *******************************************************************************/
static void pollChannel(uint32_t adcBase,
                        ADC_SOCNumber socNum,
                        volatile uint16_t* sampleCount,
                        volatile uint16_t* completeFlag,
                        const char* errorMsg)
{
    uint32_t timeout;
    if(*sampleCount < RESULTS_BUFFER_SIZE)
    {
        ADC_forceSOC(adcBase, socNum);
        timeout = 0;
        while(*completeFlag == 0)
        {
            if(++timeout > 1000000)
            {
                UART_writeString(errorMsg);
                while(1);
            }
        }
        *completeFlag = 0;
    }
}

/********************************************************************************
 * runSingleTestADC0  -  3 channels: SOC0(IN0), SOC1(IN2), SOC2(IN4)
 *******************************************************************************/
void runSingleTestADC0(uint16_t testNumber)
{
    uint16_t ch;
    UART_writeString(" [ADC0] Sampling IN0, IN2, IN4...");

    for(ch = 0; ch < ADC0_NUM_CH; ch++)
    {
        adc0SampleCount[ch] = 0;
        adc0Index[ch]       = 0;
        adc0Complete[ch]    = 0;
    }

    GPIO_writePin(myBoardLED0_GPIO, 0);
    triggerSystemSync();

    while(   adc0SampleCount[0] < RESULTS_BUFFER_SIZE
          || adc0SampleCount[1] < RESULTS_BUFFER_SIZE
          || adc0SampleCount[2] < RESULTS_BUFFER_SIZE)
    {
        pollChannel(myADC0_BASE, ADC_SOC_NUMBER0,
                    &adc0SampleCount[0], &adc0Complete[0],
                    "\r\nERROR: ADC0 ch0 timeout!\r\n");

        pollChannel(myADC0_BASE, ADC_SOC_NUMBER1,
                    &adc0SampleCount[1], &adc0Complete[1],
                    "\r\nERROR: ADC0 ch1 timeout!\r\n");

        pollChannel(myADC0_BASE, ADC_SOC_NUMBER2,
                    &adc0SampleCount[2], &adc0Complete[2],
                    "\r\nERROR: ADC0 ch2 timeout!\r\n");

        DEVICE_DELAY_US(SAMPLE_DELAY_US);
    }

    GPIO_writePin(myBoardLED0_GPIO, 1);

    calculateStatistics(adc0Results[0], &adc0TestResults[0][testNumber]);
    calculateStatistics(adc0Results[1], &adc0TestResults[1][testNumber]);
    calculateStatistics(adc0Results[2], &adc0TestResults[2][testNumber]);

    displayTestResult(testNumber, "A0-IN0", &adc0TestResults[0][testNumber]);
    displayTestResult(testNumber, "A0-IN2", &adc0TestResults[1][testNumber]);
    displayTestResult(testNumber, "A0-IN4", &adc0TestResults[2][testNumber]);
}

/********************************************************************************
 * runSingleTestADC1  -  3 channels: SOC3(IN0), SOC8(IN2), SOC9(IN4)
 *******************************************************************************/
void runSingleTestADC1(uint16_t testNumber)
{
    uint16_t ch;
    UART_writeString(" [ADC1] Sampling IN0, IN2, IN4...");

    for(ch = 0; ch < ADC1_NUM_CH; ch++)
    {
        adc1SampleCount[ch] = 0;
        adc1Index[ch]       = 0;
        adc1Complete[ch]    = 0;
    }

    GPIO_writePin(myBoardLED0_GPIO, 0);
    triggerSystemSync();

    while(   adc1SampleCount[0] < RESULTS_BUFFER_SIZE
          || adc1SampleCount[1] < RESULTS_BUFFER_SIZE
          || adc1SampleCount[2] < RESULTS_BUFFER_SIZE)
    {
        pollChannel(myADC1_BASE, ADC_SOC_NUMBER3,
                    &adc1SampleCount[0], &adc1Complete[0],
                    "\r\nERROR: ADC1 ch0 timeout!\r\n");

        pollChannel(myADC1_BASE, ADC_SOC_NUMBER8,
                    &adc1SampleCount[1], &adc1Complete[1],
                    "\r\nERROR: ADC1 ch1 timeout!\r\n");

        pollChannel(myADC1_BASE, ADC_SOC_NUMBER9,
                    &adc1SampleCount[2], &adc1Complete[2],
                    "\r\nERROR: ADC1 ch2 timeout!\r\n");

        DEVICE_DELAY_US(SAMPLE_DELAY_US);
    }

    GPIO_writePin(myBoardLED0_GPIO, 1);

    calculateStatistics(adc1Results[0], &adc1TestResults[0][testNumber]);
    calculateStatistics(adc1Results[1], &adc1TestResults[1][testNumber]);
    calculateStatistics(adc1Results[2], &adc1TestResults[2][testNumber]);

    displayTestResult(testNumber, "A1-IN0", &adc1TestResults[0][testNumber]);
    displayTestResult(testNumber, "A1-IN2", &adc1TestResults[1][testNumber]);
    displayTestResult(testNumber, "A1-IN4", &adc1TestResults[2][testNumber]);
}

/********************************************************************************
 * runSingleTestADC2  -  2 channels: SOC10(IN2), SOC11(IN4)
 *******************************************************************************/
void runSingleTestADC2(uint16_t testNumber)
{
    uint16_t ch;
    UART_writeString(" [ADC2] Sampling IN2, IN4...");

    for(ch = 0; ch < ADC2_NUM_CH; ch++)
    {
        adc2SampleCount[ch] = 0;
        adc2Index[ch]       = 0;
        adc2Complete[ch]    = 0;
    }

    GPIO_writePin(myBoardLED0_GPIO, 0);
    triggerSystemSync();

    while(   adc2SampleCount[0] < RESULTS_BUFFER_SIZE
          || adc2SampleCount[1] < RESULTS_BUFFER_SIZE)
    {
        pollChannel(myADC2_BASE, ADC_SOC_NUMBER10,
                    &adc2SampleCount[0], &adc2Complete[0],
                    "\r\nERROR: ADC2 ch0 timeout!\r\n");

        pollChannel(myADC2_BASE, ADC_SOC_NUMBER11,
                    &adc2SampleCount[1], &adc2Complete[1],
                    "\r\nERROR: ADC2 ch1 timeout!\r\n");

        DEVICE_DELAY_US(SAMPLE_DELAY_US);
    }

    GPIO_writePin(myBoardLED0_GPIO, 1);

    calculateStatistics(adc2Results[0], &adc2TestResults[0][testNumber]);
    calculateStatistics(adc2Results[1], &adc2TestResults[1][testNumber]);

    displayTestResult(testNumber, "A2-IN2",  &adc2TestResults[0][testNumber]);
    displayTestResult(testNumber, "A2-IN4",  &adc2TestResults[1][testNumber]);
}

/********************************************************************************
 * runSingleTestADC3  -  4 channels: SOC4(IN0), SOC5(IN1), SOC6(IN2), SOC7(IN3)
 *******************************************************************************/
void runSingleTestADC3(uint16_t testNumber)
{
    uint16_t ch;
    UART_writeString(" [ADC3] Sampling IN0, IN1, IN2, IN3...");

    for(ch = 0; ch < ADC3_NUM_CH; ch++)
    {
        adc3SampleCount[ch] = 0;
        adc3Index[ch]       = 0;
        adc3Complete[ch]    = 0;
    }

    GPIO_writePin(myBoardLED0_GPIO, 0);
    triggerSystemSync();

    while(   adc3SampleCount[0] < RESULTS_BUFFER_SIZE
          || adc3SampleCount[1] < RESULTS_BUFFER_SIZE
          || adc3SampleCount[2] < RESULTS_BUFFER_SIZE
          || adc3SampleCount[3] < RESULTS_BUFFER_SIZE)
    {
        pollChannel(myADC3_BASE, ADC_SOC_NUMBER4,
                    &adc3SampleCount[0], &adc3Complete[0],
                    "\r\nERROR: ADC3 ch0 timeout!\r\n");

        pollChannel(myADC3_BASE, ADC_SOC_NUMBER5,
                    &adc3SampleCount[1], &adc3Complete[1],
                    "\r\nERROR: ADC3 ch1 timeout!\r\n");

        pollChannel(myADC3_BASE, ADC_SOC_NUMBER6,
                    &adc3SampleCount[2], &adc3Complete[2],
                    "\r\nERROR: ADC3 ch2 timeout!\r\n");

        pollChannel(myADC3_BASE, ADC_SOC_NUMBER7,
                    &adc3SampleCount[3], &adc3Complete[3],
                    "\r\nERROR: ADC3 ch3 timeout!\r\n");

        DEVICE_DELAY_US(SAMPLE_DELAY_US);
    }

    GPIO_writePin(myBoardLED0_GPIO, 1);

    calculateStatistics(adc3Results[0], &adc3TestResults[0][testNumber]);
    calculateStatistics(adc3Results[1], &adc3TestResults[1][testNumber]);
    calculateStatistics(adc3Results[2], &adc3TestResults[2][testNumber]);
    calculateStatistics(adc3Results[3], &adc3TestResults[3][testNumber]);

    displayTestResult(testNumber, "A3-IN0", &adc3TestResults[0][testNumber]);
    displayTestResult(testNumber, "A3-IN1", &adc3TestResults[1][testNumber]);
    displayTestResult(testNumber, "A3-IN2", &adc3TestResults[2][testNumber]);
    displayTestResult(testNumber, "A3-IN3", &adc3TestResults[3][testNumber]);
}

/********************************************************************************
 * Window Average Calculators
 *******************************************************************************/

/* Generic helper - averages TESTS_PER_WINDOW entries for one channel */
static void averageChannel(WindowStats testArr[][TESTS_PER_WINDOW],
                           uint16_t ch,
                           WindowStats winArr[][NUM_WINDOWS],
                           uint16_t winIdx)
{
    uint16_t j;
    uint32_t sumMin = 0, sumMax = 0, sumAvg = 0, sumRange = 0;
    float    sumStd = 0.0f;

    for(j = 0; j < TESTS_PER_WINDOW; j++)
    {
        sumMin   += testArr[ch][j].min;
        sumMax   += testArr[ch][j].max;
        sumAvg   += testArr[ch][j].avg;
        sumRange += testArr[ch][j].range;
        sumStd   += testArr[ch][j].stdDev;
    }
    winArr[ch][winIdx].windowCycles = ACQ_WINDOW_START + winIdx;
    winArr[ch][winIdx].min    = (uint16_t)(sumMin   / TESTS_PER_WINDOW);
    winArr[ch][winIdx].max    = (uint16_t)(sumMax   / TESTS_PER_WINDOW);
    winArr[ch][winIdx].avg    = (uint16_t)(sumAvg   / TESTS_PER_WINDOW);
    winArr[ch][winIdx].range  = (uint16_t)(sumRange / TESTS_PER_WINDOW);
    winArr[ch][winIdx].stdDev = sumStd / TESTS_PER_WINDOW;
}

void calculateWindowAverageADC0(uint16_t windowIndex)
{
    uint16_t ch;
    for(ch = 0; ch < ADC0_NUM_CH; ch++)
        averageChannel(adc0TestResults, ch, adc0WindowResults, windowIndex);
}

void calculateWindowAverageADC1(uint16_t windowIndex)
{
    uint16_t ch;
    for(ch = 0; ch < ADC1_NUM_CH; ch++)
        averageChannel(adc1TestResults, ch, adc1WindowResults, windowIndex);
}

void calculateWindowAverageADC2(uint16_t windowIndex)
{
    uint16_t ch;
    for(ch = 0; ch < ADC2_NUM_CH; ch++)
        averageChannel(adc2TestResults, ch, adc2WindowResults, windowIndex);
}

void calculateWindowAverageADC3(uint16_t windowIndex)
{
    uint16_t ch;
    for(ch = 0; ch < ADC3_NUM_CH; ch++)
        averageChannel(adc3TestResults, ch, adc3WindowResults, windowIndex);
}

/********************************************************************************
 * Final Table Display  -  one table per channel per ADC
 *******************************************************************************/
void displayFinalTableADC0(void)
{
    uint16_t w;
    /* ADC0 ch0 - IN0 */
    printTableHeader("ADC0", "ADCIN0 (SOC0)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc0WindowResults[0][w]);
    /* ADC0 ch1 - IN2 */
    printTableHeader("ADC0", "ADCIN2 (SOC1)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc0WindowResults[1][w]);
    /* ADC0 ch2 - IN4 */
    printTableHeader("ADC0", "ADCIN4 (SOC2)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc0WindowResults[2][w]);
}

void displayFinalTableADC1(void)
{
    uint16_t w;
    printTableHeader("ADC1", "ADCIN0 (SOC3)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc1WindowResults[0][w]);
    printTableHeader("ADC1", "ADCIN2 (SOC8)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc1WindowResults[1][w]);
    printTableHeader("ADC1", "ADCIN4 (SOC9)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc1WindowResults[2][w]);
}

void displayFinalTableADC2(void)
{
    uint16_t w;
    printTableHeader("ADC2", "ADCIN2 (SOC10)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc2WindowResults[0][w]);
    printTableHeader("ADC2", "ADCIN4 (SOC11)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc2WindowResults[1][w]);
}

void displayFinalTableADC3(void)
{
    uint16_t w;
    printTableHeader("ADC3", "ADCIN0 (SOC4)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc3WindowResults[0][w]);
    printTableHeader("ADC3", "ADCIN1 (SOC5)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc3WindowResults[1][w]);
    printTableHeader("ADC3", "ADCIN2 (SOC6)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc3WindowResults[2][w]);
    printTableHeader("ADC3", "ADCIN3 (SOC7)");
    for(w = 0; w < NUM_WINDOWS; w++) printTableRow(ACQ_WINDOW_START + w, &adc3WindowResults[3][w]);
}

/********************************************************************************
 * main
 *******************************************************************************/
void main(void)
{
    uint16_t i, j;
    uint16_t currentWindow;

    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();
    Board_init();

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    EINT;
    ERTM;

    delayMs(500);

    UART_writeString("Press ANY KEY to start 12-channel ADC sweep test...\r\n\r\n");
    waitForKeyPress();
    UART_writeString("Starting sweep: ADC0(3ch) + ADC1(3ch) + ADC2(2ch) + ADC3(4ch)\r\n");
    UART_writeString("========================================================\r\n");

    for(i = 0; i < NUM_WINDOWS; i++)
    {
        currentWindow = ACQ_WINDOW_START + i;

        sprintf(uartBuffer, "\r\n=== Window %2u cycles (%uns) ===\r\n",
                currentWindow, currentWindow * 5);
        UART_writeString(uartBuffer);

        stopEPWMs();
        delayMs(5);

        /*
         * Set acquisition window for ALL four ADC modules at the start of
         * each window so that every channel is swept with the same setting.
         */
        setAcquisitionWindowADC0(currentWindow);
        setAcquisitionWindowADC1(currentWindow);
        setAcquisitionWindowADC2(currentWindow);
        setAcquisitionWindowADC3(currentWindow);
        delayMs(10);

        /* ---- Run TESTS_PER_WINDOW repetitions for every ADC ---- */
        for(j = 0; j < TESTS_PER_WINDOW; j++)
        {
            /* Each ADC module is tested sequentially.
             * triggerSystemSync() is called inside every runSingleTest,
             * so each module gets a clean EPWM-zero start.             */
            runSingleTestADC0(j);
            delayMs(20);

            runSingleTestADC1(j);
            delayMs(20);

            runSingleTestADC2(j);
            delayMs(20);

            runSingleTestADC3(j);
            delayMs(20);
        }

        /* Average the TESTS_PER_WINDOW runs for this window */
        calculateWindowAverageADC0(i);
        calculateWindowAverageADC1(i);
        calculateWindowAverageADC2(i);
        calculateWindowAverageADC3(i);

        delayMs(100);
    }

    stopEPWMs();

    /* ---- Print final summary tables ---- */
    UART_writeString("\r\n\r\n===== FINAL RESULTS =====\r\n");

    displayFinalTableADC0();
    displayFinalTableADC1();
    displayFinalTableADC2();
    displayFinalTableADC3();

    GPIO_writePin(myBoardLED0_GPIO, 1);

/* Phase 2 of the test */
    delayMs(500);

    UART_writeString("Press ANY KEY to start 12-channel ADC sweep test...\r\n\r\n");
    waitForKeyPress();
    UART_writeString("Starting sweep: ADC0(3ch) + ADC1(3ch) + ADC2(2ch) + ADC3(4ch)\r\n");
    UART_writeString("========================================================\r\n");

    for(i = 0; i < NUM_WINDOWS; i++)
    {
        currentWindow = ACQ_WINDOW_START + i;

        sprintf(uartBuffer, "\r\n=== Window %2u cycles (%uns) ===\r\n",
                currentWindow, currentWindow * 5);
        UART_writeString(uartBuffer);

        stopEPWMs();
        delayMs(5);

        /*
         * Set acquisition window for ALL four ADC modules at the start of
         * each window so that every channel is swept with the same setting.
         */
        ReconfigureandsetAcquisitionWindowADC0(currentWindow);
        ReconfigureandsetAcquisitionWindowADC1(currentWindow);
        ReconfigureandsetAcquisitionWindowADC2(currentWindow);
        ReconfigureandsetAcquisitionWindowADC3(currentWindow);
        delayMs(10);

        /* ---- Run TESTS_PER_WINDOW repetitions for every ADC ---- */
        for(j = 0; j < TESTS_PER_WINDOW; j++)
        {
            /* Each ADC module is tested sequentially.
             * triggerSystemSync() is called inside every runSingleTest,
             * so each module gets a clean EPWM-zero start.             */
            runSingleTestADC0(j);
            delayMs(20);

            runSingleTestADC1(j);
            delayMs(20);

            runSingleTestADC2(j);
            delayMs(20);

            runSingleTestADC3(j);
            delayMs(20);
        }

        /* Average the TESTS_PER_WINDOW runs for this window */
        calculateWindowAverageADC0(i);
        calculateWindowAverageADC1(i);
        calculateWindowAverageADC2(i);
        calculateWindowAverageADC3(i);

        delayMs(100);
    }

    stopEPWMs();

    /* ---- Print final summary tables ---- */
    UART_writeString("\r\n\r\n===== FINAL RESULTS =====\r\n");

    displayFinalTableADC0();
    displayFinalTableADC1();
    displayFinalTableADC2();
    displayFinalTableADC3();

    GPIO_writePin(myBoardLED0_GPIO, 1);


    while(1) { /* done */ }
}

/* EOF */
