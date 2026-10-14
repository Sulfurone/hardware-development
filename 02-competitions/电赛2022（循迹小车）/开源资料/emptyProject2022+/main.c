#include "driverlib.h"
#include "ESD_OLED.h"
#include "MPU6050.h"
#include "SR04.h"
#include "stdio.h"
//*****************************************************************************
//
//Target frequency for MCLK in kHz
//
//*****************************************************************************
#define UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ   24000
//*****************************************************************************
//
//MCLK/FLLRef Ratio
//
//*****************************************************************************
#define UCS_MCLK_FLLREF_RATIO   6
//****************************************************************************
//
//Variable to store current Clock values
//
//*****************************************************************************
uint32_t clockValue = 0;
//*****************************************************************************
//
//Variable to store status of Oscillator fault flags
//
//*****************************************************************************
//*****************************************************************************
//
//XT1 Crystal Frequency being used
//
//*****************************************************************************
#define UCS_XT1_CRYSTAL_FREQUENCY    32768
#define COMPARE_VALUE 400
//*****************************************************************************
//
//XT2 Crystal Frequency being used
//
//*****************************************************************************
#define UCS_XT2_CRYSTAL_FREQUENCY   4000000
//*****************************************************************************
//
//Desired Timeout for XT1 initialization
//
//*****************************************************************************
#define UCS_XT1_TIMEOUT 50000
#define TIMER_PERIOD 999
#define DUTY_CYCLE  25*4    //初始占空25%
#define TIMER_PERIOD_BLDC 1999
#define DUTY_CYCLE_BLDC  25*20 //初始占空%25
#define UCS_XT2_TIMEOUT 50000
#define Num_of_Results   8
//ADC buffer
volatile uint16_t A0results[Num_of_Results];
volatile uint16_t A1results[Num_of_Results];
volatile uint16_t A2results[Num_of_Results];
volatile uint16_t A3results[Num_of_Results];

uint8_t SR04_Buffer[3];
unsigned long SR04Val = 0;
uint8_t SR04Busy = 1;

uint8_t XJ_MODE = 0; //寻迹模式
uint8_t BMX_N =0;  //停车线计数
uint8_t CAR_MODE = 0; //正常寻迹模式
uint16_t CAR_L = 10000; //寻迹路程

//速度控制参数
float SPEED_KP=30.0; //速度KP
float SPEED_KI=20.5; //速度KI

int L_SPD=0,R_SPD=0,L_V,R_V;//左右轮脉冲数
int CAR_SPEED=0; //实际速度
int SET_SPEED=0; //设定速度
int SPEED_ERR=0,SPEED_ERR_OLD=0; //速度偏差
int SPEED_PWM_OUT=0; //输出PWM输出
//方向控制参数
int DIR_KP=900;  //方向KP
int DIR_KD=18;  //方向KD

int DIR_ERR=0; //方向偏差
int DIR_PWM_OUT;//方向PWM输出
float GYRO_Z;   //Z角速度

unsigned int T0_n=0;

//DisplayBuffer
char displayBuffer[22];
//MPU6050 Buffer
uint8_t IR_LED[12];

uint8_t MPU_Buffer[14];
uint8_t returnValue = 0;
void UCS_init();
void PWM_init();
void ADC_init();
void PWM_Change_Duty(char pwmx, unsigned int duty);
void PWM_OUT(int L_PWM,int R_PWM);
bool UART_init(uint16_t baseAddress, uint32_t Baudrate);
bool P2INT_init();
void TIMERB_init();
void DIS_OLED(void);
//*****************************************************************************
//*******************************业务逻辑开始**************************************
void main (void){
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN2);  //蜂鸣器引脚初始化
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);  //板载LED引脚初始化
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN2);  //LED引脚初始化


    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN2);

    //Set VCore = 0-1-2-3 for 24MHz clock
    PMM_setVCore(PMM_CORE_LEVEL_0);
    PMM_setVCore(PMM_CORE_LEVEL_1);
    PMM_setVCore(PMM_CORE_LEVEL_2);
    PMM_setVCore(PMM_CORE_LEVEL_3);

    UCS_init();
    PWM_init();
    //ADC_init();
    // Enable global interrupt
    __bis_SR_register(GIE);

    //Verify if the Clock settings are as expected
    //clockValue = UCS_getMCLK();
    //clockValue = UCS_getACLK();
    //clockValue = UCS_getSMCLK();



    InitMPU6050();
    LCD_Init();
    LCD_Fill(0x00);  //初始清屏
    /*LCD_P6x8Str(0,1,"     [^] 00.00V      ");
    LCD_P6x8Str(0,3,"  CAR_ANGLE = +000   ");
    LCD_P6x8Str(0,5,"   RUN_TIME = 10s    ");*/

    PWM_OUT(0,0);
    //LCD_Dis_Main();
    P2INT_init();
    UART_init(USCI_A1_BASE, 115200);
    UART_init(USCI_A0_BASE, 115200);
    TIMERB_init();

    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN2); //关蜂鸣器
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0); //亮板载LED

    SET_SPEED = 0;//92;
    XJ_MODE = 0;
    while(1){
        //神奇的大循环



        if(SR04Busy==0){
            DIS_OLED();
            SR04Read(SR04_Buffer);
            SR04Val = SR04Cal(SR04_Buffer);
            SR04Start();
        }

        /*DIR_PWM_OUT =0;

        USCI_A_UART_transmitData(USCI_A1_BASE,0x03); //发送函数
        USCI_A_UART_transmitData(USCI_A1_BASE,~0x03); //发送函数

        USCI_A_UART_transmitData(USCI_A1_BASE,SET_SPEED); //发送函数
        USCI_A_UART_transmitData(USCI_A1_BASE,SET_SPEED>>8); //发送函数

        USCI_A_UART_transmitData(USCI_A1_BASE,CAR_SPEED); //发送函数
        USCI_A_UART_transmitData(USCI_A1_BASE,CAR_SPEED>>8); //发送函数

        USCI_A_UART_transmitData(USCI_A1_BASE,~0x03); //发送函数
        USCI_A_UART_transmitData(USCI_A1_BASE,0x03); //发送函数*/
    }
}


void DIS_OLED(void)
{
    LCD_Add_Int(0,0,L_V,4,1);

    LCD_Add_Int(80,0,R_V,4,1); // DIR_ERR

    LCD_Add_Int(40,0,DIR_ERR,4,1); // DIR_ERR

    LCD_Add_Int(0,2,IR_LED[0],1,0); // DIR_ERR
    LCD_Add_Int(10,2,IR_LED[1],1,0); // DIR_ERR
    LCD_Add_Int(20,2,IR_LED[2],1,0); // DIR_ERR
    LCD_Add_Int(30,2,IR_LED[3],1,0); // DIR_ERR
    LCD_Add_Int(40,2,IR_LED[4],1,0); // DIR_ERR
    LCD_Add_Int(50,2,IR_LED[5],1,0); // DIR_ERR
    LCD_Add_Int(60,2,IR_LED[6],1,0); // DIR_ERR
    LCD_Add_Int(70,2,IR_LED[7],1,0); // DIR_ERR
    LCD_Add_Int(80,2,IR_LED[8],1,0); // DIR_ERR
    LCD_Add_Int(90,2,IR_LED[9],1,0); // DIR_ERR
    LCD_Add_Int(100,2,IR_LED[10],1,0); // DIR_ERR
    LCD_Add_Int(110,2,IR_LED[11],1,0); // DIR_ERR

    LCD_Add_Int(0,3,SR04Val,4,0); // DIR_ERR
    //LCD_Add_Int(80,0,R_V,4,1);
}



void TIMERB_init(){
    //Start timer in continuous mode sourced by ACLK 32.786KHz
    Timer_B_initContinuousModeParam initContParam = {0};
    initContParam.clockSource = TIMER_B_CLOCKSOURCE_ACLK;
    initContParam.clockSourceDivider = TIMER_B_CLOCKSOURCE_DIVIDER_1;
    initContParam.timerInterruptEnable_TBIE = TIMER_B_TBIE_INTERRUPT_DISABLE;
    initContParam.timerClear = TIMER_B_DO_CLEAR;
    initContParam.startTimer = false;
    Timer_B_initContinuousMode(TIMER_B0_BASE, &initContParam);

     //Initiaze compare mode
    Timer_B_clearCaptureCompareInterrupt(TIMER_B0_BASE,
    TIMER_B_CAPTURECOMPARE_REGISTER_0);

    Timer_B_initCompareModeParam initCompParam = {0};
    initCompParam.compareRegister = TIMER_B_CAPTURECOMPARE_REGISTER_0;
    initCompParam.compareInterruptEnable = TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE;
    initCompParam.compareOutputMode = TIMER_B_OUTPUTMODE_OUTBITVALUE;
    initCompParam.compareValue = COMPARE_VALUE;
    Timer_B_initCompareMode(TIMER_B0_BASE, &initCompParam);

    Timer_B_startCounter( TIMER_B0_BASE,
        TIMER_B_CONTINUOUS_MODE
        );
}

//外部中断初始化函数
bool P2INT_init()
{

    //Enable P2.0 P2.2 internal resistance as pull-Up resistance
    GPIO_setAsInputPinWithPullUpResistor(//脉冲IO初始化
        GPIO_PORT_P2,
        GPIO_PIN0+GPIO_PIN2
        );
   GPIO_setAsInputPinWithPullUpResistor(//方向IO初始化
        GPIO_PORT_P3,
        GPIO_PIN0
        );
    GPIO_setAsInputPinWithPullUpResistor(  //方向IO初始化
        GPIO_PORT_P7,
        GPIO_PIN4
        );
    //P2.0 P2.2  interrupt enabled
    GPIO_enableInterrupt( //使能中断
        GPIO_PORT_P2,
        GPIO_PIN0+GPIO_PIN2
        );

    //P2.0 P2.2 Hi/Lo edge
    GPIO_selectInterruptEdge( //下降沿触发
        GPIO_PORT_P2,
        GPIO_PIN0+GPIO_PIN2,
        GPIO_HIGH_TO_LOW_TRANSITION
        );


    //P2.0 P2.2 IFG cleared
    GPIO_clearInterrupt(   //清中断标志
        GPIO_PORT_P2,
        GPIO_PIN0+GPIO_PIN2
        );


    return STATUS_SUCCESS;
}


//串口初始化函数
bool UART_init(uint16_t baseAddress, uint32_t Baudrate)
{
    float UART_Temp = 0;
    USCI_A_UART_initParam huart = {0};

    if(baseAddress == USCI_A0_BASE)         //P3.3, P3.4 = USCI_A0 TXD/RXD
    {
        GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN3);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN4);
    }
    else if(baseAddress == USCI_A1_BASE)    //P4.4, P4.5 = USCI_A1 TXD/RXD
    {
        GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4, GPIO_PIN4);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN5);
    }

    if(Baudrate <= 9600)
    {
        huart.selectClockSource = USCI_A_UART_CLOCKSOURCE_ACLK;
        UART_Temp = (float)UCS_getACLK()/Baudrate;
    }
    else
    {
        huart.selectClockSource = USCI_A_UART_CLOCKSOURCE_SMCLK;
        UART_Temp = (float)UCS_getSMCLK()/Baudrate;
    }

    if(UART_Temp < 16)
        huart.overSampling = USCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;
    else
    {
        huart.overSampling = USCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
        UART_Temp /= 16;
    }

    huart.clockPrescalar = (int)UART_Temp;

    if(huart.overSampling == USCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION)
    {
        huart.secondModReg = (int)((UART_Temp - huart.clockPrescalar) * 8);
    }
    else
    {
        huart.firstModReg = (int)((UART_Temp - huart.clockPrescalar) * 16);
    }

    huart.parity = USCI_A_UART_NO_PARITY;
    huart.msborLsbFirst = USCI_A_UART_LSB_FIRST;
    huart.numberofStopBits = USCI_A_UART_ONE_STOP_BIT;
    huart.uartMode = USCI_A_UART_MODE;

    if (STATUS_FAIL == USCI_A_UART_init(baseAddress, &huart))
    {
        return STATUS_FAIL;
    }

    //Enable UART module for operation
    USCI_A_UART_enable(baseAddress);

    //Enable Receive Interrupt
    USCI_A_UART_clearInterrupt(baseAddress, USCI_A_UART_RECEIVE_INTERRUPT);
    USCI_A_UART_enableInterrupt(baseAddress, USCI_A_UART_RECEIVE_INTERRUPT);

    return STATUS_SUCCESS;
}


void PWM_OUT(int L_PWM,int R_PWM)
{
    if(L_PWM>9900)  L_PWM=9900 ;  //PWM占空限幅
    if(L_PWM<-9900) L_PWM=-9900;
    if(R_PWM>9900)  R_PWM=9900;
    if(R_PWM<-9900) R_PWM=-9900;

    if(L_PWM<0)  //判断旋转方向并输出PWM
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5); //电机正转
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
    }
    else
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4); //电机反转
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);
    }
    if(R_PWM<0)  //判断旋转方向并输出PWM
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3); //电机正转
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN2);
    }
    else
    {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN2); //电机反转
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3);
    }

    PWM_Change_Duty(1, abs(R_PWM/10));
    PWM_Change_Duty(2, abs(L_PWM/10));
}

//**********************************业务逻辑结束************************************
//*****************************************************************************
void UCS_init(){
    //Initializes the XT1 and XT2 crystal frequencies being used
    UCS_setExternalClockSource(UCS_XT1_CRYSTAL_FREQUENCY,UCS_XT2_CRYSTAL_FREQUENCY);//设置外部时钟源的频率，没什么实际设定

    //Initialize XT1. Returns STATUS_SUCCESS if initializes successfully
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5,GPIO_PIN4 + GPIO_PIN5);//XT1口不作为普通IO
    returnValue = UCS_turnOnLFXT1WithTimeout(UCS_XT1_DRIVE_0,UCS_XCAP_3,UCS_XT1_TIMEOUT);//启动XT1

    //Startup HF XT2 crystal Port select XT2
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5,GPIO_PIN2 + GPIO_PIN3);//XT2口不作为普通IO

    //Initialize XT2. Returns STATUS_SUCCESS if initializes successfully
    returnValue = UCS_turnOnXT2WithTimeout(UCS_XT2_DRIVE_4MHZ_8MHZ,UCS_XT2_TIMEOUT);//启动XT2

    //Set DCO FLL reference = REFO
    UCS_initClockSignal(UCS_FLLREF,UCS_XT2CLK_SELECT,UCS_CLOCK_DIVIDER_1);//XT2作为FLL参考

    //Set Ratio and Desired MCLK Frequency  and initialize DCO
    UCS_initFLLSettle(UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ,UCS_MCLK_FLLREF_RATIO);//MCLK设置为24MHz

    //Set ACLK = REFO
    UCS_initClockSignal(UCS_ACLK,UCS_REFOCLK_SELECT,UCS_CLOCK_DIVIDER_1);//ACLK设置为32.768kHz
    UCS_initClockSignal(UCS_SMCLK,UCS_XT2CLK_SELECT,UCS_CLOCK_DIVIDER_1);//SMCLK设置为4MHz
}


void PWM_init(){
    //P1.4 1.5 as PWM output
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,GPIO_PIN4+GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN2 + GPIO_PIN3);  //电机方向引脚初始化
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4 + GPIO_PIN5);  //电机方向引脚初始化

    Timer_A_outputPWMParam param = {0};
    //第四路PWM P1.5
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = TIMER_PERIOD;
    param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;
    param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    param.dutyCycle = DUTY_CYCLE;
    Timer_A_outputPWM(TIMER_A0_BASE, &param);

    //第五路PWM P1.4
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = TIMER_PERIOD;
    param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;
    param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    param.dutyCycle = DUTY_CYCLE;
    Timer_A_outputPWM(TIMER_A0_BASE, &param);
}

void PWM_Change_Duty(char pwmx, unsigned int duty){
    if(duty>995){
        duty = 995;
    }
    switch(pwmx){
    case 1: Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_4, duty);break;
    case 2: Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_3, duty);break;
    default:;
    }
}

void ADC_init(){
    //Enable A/D channel inputs
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
        GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 +
        GPIO_PIN3 + GPIO_PIN4 + GPIO_PIN5 +
        GPIO_PIN6 + GPIO_PIN7
        );

    //Initialize the ADC12_A Module
    /*
     * Base address of ADC12_A Module
     * Use internal ADC12_A bit as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC12_A_init(ADC12_A_BASE,
        ADC12_A_SAMPLEHOLDSOURCE_SC,
        ADC12_A_CLOCKSOURCE_ADC12OSC,
        ADC12_A_CLOCKDIVIDER_1
        );

    ADC12_A_enable(ADC12_A_BASE);

    /*
     * Base address of ADC12_A Module
     * For memory buffers 0-7 sample/hold for 256 clock cycles
     * For memory buffers 8-15 sample/hold for 4 clock cycles (default)
     * Enable Multiple Sampling
     */
    ADC12_A_setupSamplingTimer(ADC12_A_BASE,
        ADC12_A_CYCLEHOLD_256_CYCLES,
        ADC12_A_CYCLEHOLD_4_CYCLES,
        ADC12_A_MULTIPLESAMPLESENABLE);

    //Configure Memory Buffers
    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 0
     * Map input A0 to memory buffer 0
     * Vref+ = AVcc
     * Vref- = AVss
     * Memory buffer 0 is not the end of a sequence
     */
    ADC12_A_configureMemoryParam param0 = {0};
    param0.memoryBufferControlIndex = ADC12_A_MEMORY_0;
    param0.inputSourceSelect = ADC12_A_INPUT_A0;
    param0.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
    param0.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    param0.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
    ADC12_A_configureMemory(ADC12_A_BASE ,&param0);

    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 1
     * Map input A1 to memory buffer 1
     * Vref+ = AVcc
     * Vref- = AVss
     * Memory buffer 1 is not the end of a sequence
     *
     */
    ADC12_A_configureMemoryParam param1 = {0};
    param1.memoryBufferControlIndex = ADC12_A_MEMORY_1;
    param1.inputSourceSelect = ADC12_A_INPUT_A1;
    param1.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
    param1.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    param1.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
    ADC12_A_configureMemory(ADC12_A_BASE ,&param1);
    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 2
     * Map input A2 to memory buffer 2
     * Vref+ = AVcc
     * Vref- = AVss
     * Memory buffer 2 is not the end of a sequence
     */
    ADC12_A_configureMemoryParam param2 = {0};
    param2.memoryBufferControlIndex = ADC12_A_MEMORY_2;
    param2.inputSourceSelect = ADC12_A_INPUT_A2;
    param2.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
    param2.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    param2.endOfSequence = ADC12_A_NOTENDOFSEQUENCE;
    ADC12_A_configureMemory(ADC12_A_BASE ,&param2);
    /*
     * Base address of the ADC12_A Module
     * Configure memory buffer 3
     * Map input A3 to memory buffer 3
     * Vr+ = AVcc
     * Vr- = AVss
     * Memory buffer 3 IS the end of a sequence
     */
    ADC12_A_configureMemoryParam param3 = {0};
    param3.memoryBufferControlIndex = ADC12_A_MEMORY_3;
    param3.inputSourceSelect = ADC12_A_INPUT_A3;
    param3.positiveRefVoltageSourceSelect = ADC12_A_VREFPOS_AVCC;
    param3.negativeRefVoltageSourceSelect = ADC12_A_VREFNEG_AVSS;
    param3.endOfSequence = ADC12_A_ENDOFSEQUENCE;
    ADC12_A_configureMemory(ADC12_A_BASE ,&param3);

    //Enable memory buffer 3 interrupt
    ADC12_A_clearInterrupt(ADC12_A_BASE,
        ADC12IFG3);
    ADC12_A_enableInterrupt(ADC12_A_BASE,
        ADC12IE3);

    //Enable/Start first sampling and conversion cycle
    /*
     * Base address of ADC12_A Module
     * Start the conversion into memory buffer 0
     * Use the repeated sequence of channels
     */
    ADC12_A_startConversion(ADC12_A_BASE,
        ADC12_A_MEMORY_0,
        ADC12_A_REPEATED_SEQOFCHANNELS);

}
//以下是中断处理程序
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC12_VECTOR)))
#endif
void ADC12ISR (void)
{
    static uint16_t index = 0;

    switch (__even_in_range(ADC12IV,34)){
        case  0: break;   //Vector  0:  No interrupt
        case  2: break;   //Vector  2:  ADC overflow
        case  4: break;   //Vector  4:  ADC timing overflow
        case  6: break;   //Vector  6:  ADC12IFG0
        case  8: break;   //Vector  8:  ADC12IFG1
        case 10: break;   //Vector 10:  ADC12IFG2
        case 12:          //Vector 12:  ADC12IFG3
            //Move A0 results, IFG is cleared
            A0results[index] =
                ADC12_A_getResults(ADC12_A_BASE,
                    ADC12_A_MEMORY_0);
            //Move A1 results, IFG is cleared
            A1results[index] =
                ADC12_A_getResults(ADC12_A_BASE,
                    ADC12_A_MEMORY_1);
            //Move A2 results, IFG is cleared
            A2results[index] =
                ADC12_A_getResults(ADC12_A_BASE,
                    ADC12_A_MEMORY_2);
            //Move A3 results, IFG is cleared
            A3results[index] =
                ADC12_A_getResults(ADC12_A_BASE,
                    ADC12_A_MEMORY_3);

            //Increment results index, modulo; Set BREAKPOINT here
            index++;

            if (index == 8){
                (index = 0);
            }
        case 14: break;   //Vector 14:  ADC12IFG4
        case 16: break;   //Vector 16:  ADC12IFG5
        case 18: break;   //Vector 18:  ADC12IFG6
        case 20: break;   //Vector 20:  ADC12IFG7
        case 22: break;   //Vector 22:  ADC12IFG8
        case 24: break;   //Vector 24:  ADC12IFG9
        case 26: break;   //Vector 26:  ADC12IFG10
        case 28: break;   //Vector 28:  ADC12IFG11
        case 30: break;   //Vector 30:  ADC12IFG12
        case 32: break;   //Vector 32:  ADC12IFG13
        case 34: break;   //Vector 34:  ADC12IFG14
        default: break;
    }
}


//******************************************************************************
//
//This is the USCI_A0 interrupt vector service routine.
//
//******************************************************************************


#pragma vector=USCI_A0_VECTOR //P33 P34
__interrupt void USCI_A0_ISR (void)
{
    uint8_t receivedData = 0;
    char i,j;
    switch (__even_in_range(UCA0IV,4))
    {
        //Vector 2 - RXIFG 接收中断
        case 2:
            receivedData = USCI_A_UART_receiveData(USCI_A0_BASE);

            if(receivedData & 0x80)  //读红外管数据
            {
                IR_LED[0] = (receivedData & 0X20)>>5;
                IR_LED[1] = (receivedData & 0X10)>>4;
                IR_LED[2] = (receivedData & 0X08)>>3;
                IR_LED[3] = (receivedData & 0X04)>>2;
                IR_LED[4] = (receivedData & 0X02)>>1;
                IR_LED[5] = receivedData & 0X01;
            }
            else
            {
                IR_LED[6] = (receivedData & 0X20)>>5;
                IR_LED[7] = (receivedData & 0X10)>>4;
                IR_LED[8] = (receivedData & 0X08)>>3;
                IR_LED[9] = (receivedData & 0X04)>>2;
                IR_LED[10] = (receivedData & 0X02)>>1;
                IR_LED[11] = receivedData & 0X01;
                //判断过起跑线
                if(CAR_MODE == 0 && CAR_L > 5000 && ((IR_LED[4] == 1 ||IR_LED[7] == 1)&&IR_LED[5] == 1&&IR_LED[6] == 1))
                {
                    BMX_N++; //斑马线计数
                    if(BMX_N%2 == 0)    XJ_MODE = 1; //寻迹模式=边缘寻迹
                    else XJ_MODE = 2;
                    CAR_L = 0;  //路程积分清0
                    CAR_MODE = 1; //行驶模式=岔路模式
                    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN2);

                }

                if(CAR_MODE == 1 && CAR_L > 4000)
                {
                    XJ_MODE = 0; //寻迹模式=中心寻迹
                    CAR_MODE = 0; //行驶模式=正常模式
                    GPIO_setOutputHighOnPin(GPIO_PORT_P6, GPIO_PIN2);
                    //SET_SPEED=0;
                }

                for(i=0;i<11;i++)
                {
                    if((IR_LED[i+1] != IR_LED[i]) || i==10)
                    {
                        if(i==10)   i=5;
                        break;
                    }
                }
                for(j=11;j>0;j--)
                {
                    if((IR_LED[j-1] != IR_LED[j]) || j==1)
                    {
                        if(j==1)   j=5;
                        break;
                    }
                }

                switch(XJ_MODE) //寻迹模式
                {
                    case 0: DIR_ERR = (i+j)-13;break;
                    case 1: DIR_ERR = (i-6)*2;break;
                    case 2: DIR_ERR = (j-8)*2;break;
                }
            }
            //USCI_A_UART_transmitData(USCI_A0_BASE,receivedData); //阻塞发送
            break;
        default:
            break;
    }
}

//******************************************************************************
//
//This is the USCI_A1 interrupt vector service routine.
//
//******************************************************************************
#pragma vector=USCI_A1_VECTOR   //P44 P45
__interrupt void USCI_A1_ISR (void)
{
    uint8_t receivedData = 0;
    switch (__even_in_range(UCA1IV,4))
    {
        //Vector 2 - 接收中断
        case 2:
            receivedData = USCI_A_UART_receiveData(USCI_A1_BASE);
            USCI_A_UART_transmitData(USCI_A1_BASE,receivedData); //发送函数
            break;
        default:
            break;
    }
}

//******************************************************************************
//
//This is the PORT2_VECTOR interrupt vector service routine
//
//******************************************************************************


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT2_VECTOR)))
#endif
void Port_2 (void)
{
    /*if(GPIO_getInputPinValue(GPIO_PORT_P7,GPIO_PIN4))
    {
        R_SPD++;
    }
    else
    {
        R_SPD--;
    }
    GPIO_clearInterrupt(GPIO_PORT_P2,GPIO_PIN0);//清中断标志*/

    if(GPIO_getInterruptStatus(GPIO_PORT_P2,GPIO_PIN0)){//P20 下降沿
        //USCI_A_UART_transmitData(USCI_A1_BASE,'0');
        if(GPIO_getInputPinValue(GPIO_PORT_P7,GPIO_PIN4))
        {
            L_SPD++;
        }
        else
        {
            L_SPD--;
        }
        GPIO_clearInterrupt(GPIO_PORT_P2,GPIO_PIN0);//清中断标志
    }
    if(GPIO_getInterruptStatus(GPIO_PORT_P2,GPIO_PIN2)){//P22下降沿
        if(GPIO_getInputPinValue(GPIO_PORT_P3,GPIO_PIN0))
        {
            R_SPD++;
        }
        else
        {
            R_SPD--;
        }
        GPIO_clearInterrupt(GPIO_PORT_P2,GPIO_PIN2);//清中断标志
    }
}


// Timer B0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMERB0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMERB0_VECTOR)))
#endif
void TIMERB0_ISR (void)
{
   // int i;
    char i,j;

    uint16_t compVal = Timer_B_getCaptureCompareCount(TIMER_B0_BASE,
            TIMER_B_CAPTURECOMPARE_REGISTER_0)
            + COMPARE_VALUE;

    T0_n++; //大约12.5ms一次

    Read_MPU6050(MPU_Buffer);

    GYRO_Z=(MPU_Buffer[12]*256+MPU_Buffer[13])/65.5; //获取陀螺仪Z角速度

    DIR_PWM_OUT = DIR_ERR * DIR_KP + GYRO_Z * DIR_KD;

    if(DIR_PWM_OUT>7500) DIR_PWM_OUT=7500; //输出限幅
    if(DIR_PWM_OUT<-7500) DIR_PWM_OUT=-7500;//输出限幅

    if(T0_n%4==0) //50ms
    {
        L_V = L_SPD;
        R_V = R_SPD;
        L_SPD =0; //清脉冲数
        R_SPD =0;

        CAR_SPEED = L_V + R_V; //计算车速


        if(CAR_L<20000) CAR_L+=abs(CAR_SPEED); //路程控制变量积分


        SPEED_ERR = SET_SPEED - CAR_SPEED; //计算偏差

        SPEED_PWM_OUT += SPEED_KP * (SPEED_ERR - SPEED_ERR_OLD)+SPEED_KI*SPEED_ERR;//PID计算

        SPEED_ERR_OLD = SPEED_ERR;  //迭代偏差

        if(SPEED_PWM_OUT>7500) SPEED_PWM_OUT=7500; //输出限幅
        if(SPEED_PWM_OUT<-7500) SPEED_PWM_OUT=-7500;//输出限幅

       /* USCI_A_UART_transmitData(USCI_A1_BASE,0x03); //发送函数
        USCI_A_UART_transmitData(USCI_A1_BASE,~0x03); //发送函数

        USCI_A_UART_transmitData(USCI_A1_BASE,SET_SPEED); //发送函数
        USCI_A_UART_transmitData(USCI_A1_BASE,SET_SPEED>>8); //发送函数

        USCI_A_UART_transmitData(USCI_A1_BASE,CAR_SPEED); //发送函数
        USCI_A_UART_transmitData(USCI_A1_BASE,CAR_SPEED>>8); //发送函数

        USCI_A_UART_transmitData(USCI_A1_BASE,~0x03); //发送函数
        USCI_A_UART_transmitData(USCI_A1_BASE,0x03); //发送函数*/

        //SPEED_PWM_OUT = 5000;
    }

    PWM_OUT(SPEED_PWM_OUT+DIR_PWM_OUT,SPEED_PWM_OUT-DIR_PWM_OUT); //输出PWM

    //PWM_OUT(2500,2500);




    if(T0_n%8==0) //100ms
    {
        SR04Busy = 0;



        GPIO_toggleOutputOnPin(
        GPIO_PORT_P1,
        GPIO_PIN0
        );
    }

   //if(T0_n%160==0)    SET_SPEED *= -1;  //2s换向
    //start SR04



    // Add Offset to CCR0 [Cont mode]
    Timer_B_setCompareValue(TIMER_B0_BASE,
            TIMER_B_CAPTURECOMPARE_REGISTER_0,
            compVal
            );
}







