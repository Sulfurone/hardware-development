/******************* (C) COPYRIGHT 2012 acam messelectronic GmbH ***************
******************** (C) COPYRIGHT 2008 STMicroelectronics *********************
 * File Name          : main.c
 * Author             : acam Support Team
 * Version            : 
 * Date               : 24-10-2012
 * Description        : Simple demonstration program to run PCap01/ SPI/ Slot_1
 *******************************************************************************
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH AN EXAMPLE CODING INFORMATION REGARDING OUR PRODUCTS. THE ACAM
 * RECOMMENDATIONS ARE BELIEVED USEFUL AND OPERABLE, NEVERTHELESS IT IS OF THE
 * CUSTOMER'S SOLE RESPONSIBILITY TO MODIFY, TEST AND VALIDATE THEM BEFORE
 * SETTING UP ANY PRODUCTION PROCESS. AS A RESULT, ACAM SHALL NOT BE HELD LIABLE
 * FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS
 * ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF
 * THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
typedef enum {FALSE = 0, TRUE = !FALSE} bool;
int n;
void *Bus_Type=SPI1;
/* Private variables ---------------------------------------------------------*/
#define LOOP_DLY_100US    2000
#define LOOP_DLY_250ns    2
#define NUMBER_OF_SAMPLES 100
#define FOUR_MILLISECONDS_TIMEOUT 4300
#define TWENTY_MILLISECONDS_TIMEOUT 20300

uint32_t   CriticalSecCntr;

bool     configured_true = FALSE;
uint8_t  Rx_data1;
uint8_t  Rx_data2;
uint8_t  Rx_data3;

float     *sram_float_memory  = ((float *)(SRAM_BASE + 0xB00));
uint32_t  *sram_uint32_t_memory = ((uint32_t *)(SRAM_BASE + 0xB00));
uint32_t  sram_mem_offset     = 0x0;

uint32_t  Dummy_var = 0;

/* PCap01 Standard Config (Reg = 0xC0 .. 0xD4) -------------------------------*/
uint32_t PCap01_standard_config_reg[] = {
           0x4200FF,  // Config Register 0
          0x201022,  // Config Register 1
          0xFF1E0B,  // Config Register 2
          0x0D0064,  // Config Register 3
          0x080000,  // Config Register 4
          0x000000,  // Config Register 5
          0x000040,  // Config Register 6
          0x1F0000,  // Config Register 7
          0x800030,  // Config Register 8
          0xFF000F,  // Config Register 9
          0x180087,  // Config Register 10
          0x000000,  // Config Register 11
          0x000000,  // Config Register 12
          0x000000,  // Config Register 13
          0x000000,  // Config Register 14
          0x000000,  // Config Register 15
          0x000000,  // Config Register 16
          0x000000,  // Config Register 17
          0x000000,  // Config Register 18
          0x000000,  // Config Register 19
          0x000001}; // Config Register 20


/* PCap01 Address + Opcode ---------------------------------------------------*/
#define   PCap01_Device_Address   0xA0
#define   PCap01_Power_Reset      0x88
#define   PCap01_Partial_Reset    0x8A
#define   PCap01_Start_Meas       0x8C
#define   PCap01_Start_Temp_Meas  0x8E

/* PCap01 variables ----------------------------------------------------------*/
uint32_t  Res0_content;
float  Res1_content;
					float  Res6_content;
float     Res4_content;
	float 				Res7_content ;
float     Res11_content;
float     Capacitance_ratio;
float     R_ratio;
float     Temperature;
uint32_t  Status_content;

/* For mathematical calculations ---------------------------------------------*/
int         i;
int         j;
void *Bus_Type;
/* Private functions ---------------------------------------------------------*/
void Dly100us(void *arg);
void Simple_delay_750ns(void *arg);
void Embed_SRAM_Init(void);
void Write_Emb_SRAM_float(float ESRAM_24bit_data);
void Write_Emb_SRAM_uint32_t(uint32_t ESRAM_24bit_data);
float find_variance_float(uint16_t sample_count);
uint32_t find_variance_uint32_t(uint16_t sample_count);
void Ext_Interrupt_Init(void);
void GPIOs_LED_Test(void);

/* Bus functions -------------------------------------------------------------*/
void SPIx_I2Cx_GPIOs_Init(void* bus_type);
void SPIx_I2Cx_Interface_Init(void* bus_type);

/* Device functions, write standard config -----------------------------------*/
void SPIx_I2Cx_write_standard_config(void *bus_type, uint8_t device_address);

/* Device functions, write config reg ----------------------------------------*/
void SPIx_I2Cx_write_config_reg(void *bus_type, uint8_t device_address, uint8_t write_address, uint32_t config_reg_data);

/* Device functions, send 1byte ----------------------------------------------*/
void SPIx_I2Cx_send_1byte(void *bus_type, uint8_t device_address, uint8_t opcode_byte);

/* Device functions, read 3bytes ---------------------------------------------*/
float SPIx_I2Cx_read_3bytes(void *bus_type, uint8_t device_address, uint8_t read_opcode, uint8_t read_addr, uint8_t fractional_bits);

/* Device functions, send opcode addr ----------------------------------------*/
void I2Cx_send_opcode_addr(void *bus_type, uint8_t device_addr, uint8_t opcode_addr, bool SEND_STOP_BIT);

/*******************************************************************************
 * Function Name  : main
 * Description    : Main program.
 * Input          : None
 * Output         : None
 * Return         : None
 ******************************************************************************/
int main(void)
{
 
  /* Setup STM32 system (clock, PLL and Flash configuration) */
  SystemInit();


  
  // Choose your Slot (SPI1, SPI2, I2C1, I2C2)
    

  /* Infinite loop */
  while (Dummy_var!=1000)// Dummy_var!=15 // To Control the loop 
  {
    if(configured_true==FALSE)
    {  
      configured_true = TRUE;
      Embed_SRAM_Init();
      SPIx_I2Cx_GPIOs_Init(Bus_Type);
      SPIx_I2Cx_Interface_Init(Bus_Type);

/***************************/
      SPIx_I2Cx_send_1byte(Bus_Type, PCap01_Device_Address, PCap01_Power_Reset);
  Dly100us((void*)20);

      SPIx_I2Cx_write_standard_config(Bus_Type, PCap01_Device_Address);
  
      SPIx_I2Cx_send_1byte(Bus_Type, PCap01_Device_Address, PCap01_Partial_Reset);
  Dly100us((void*)20);
 
/***************************/    
    SPIx_I2Cx_send_1byte(Bus_Type, PCap01_Device_Address, PCap01_Start_Meas);
 Dly100us((void*)20);
    }
    
    if (Bus_Type==SPI1 |Bus_Type==I2C1) while (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)==1) {} // wait for INTERRUPT STM_GPIO3_INT_SLOT1
    if (Bus_Type==SPI2 |Bus_Type==I2C2) while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)==1) {} // wait for INTERRUPT STM_GPIO3_INT_SLOT2

/***************************/
   Res0_content= SPIx_I2Cx_read_3bytes(Bus_Type, PCap01_Device_Address,  0x40, 0x00, 0); // Argument = opcode + addr = 'b01 + {addr 000000}
    Res1_content= SPIx_I2Cx_read_3bytes(Bus_Type, PCap01_Device_Address, 0x40, 0x01, 0); // Argument = opcode + addr = 'b01 + {addr 000001}
			Res6_content= SPIx_I2Cx_read_3bytes(Bus_Type, PCap01_Device_Address, 0x40, 0x06, 0);
    Res7_content=SPIx_I2Cx_read_3bytes(Bus_Type, PCap01_Device_Address, 0x40, 0x07, 0);

    Status_content   = SPIx_I2Cx_read_3bytes(Bus_Type, PCap01_Device_Address, 0x40, 0x08, 0); // Argument = opcode + addr = 'b01 + {addr 001000}
    //Res4_content     = SPIx_I2Cx_read_3bytes(Bus_Type, PCap01_Device_Address, 0x40, 0x04, 0); // Argument = opcode + addr = 'b01 + {addr 000100}
    
    //Capacitance_ratio = Res4_content/(4194304); //4194304 = 2^22

    Capacitance_ratio = SPIx_I2Cx_read_3bytes(Bus_Type, PCap01_Device_Address, 0x40, 0x01, 21); // Argument = opcode + addr = 'b01 + {addr 000001}
    
//    printf("\n RES1(Cap)= %.0f / Cap_ratio= %f", Res1_content, Capacitance_ratio);
/***************************/
//    Write_Emb_SRAM_float(Capacitance_ratio); // Storing the Capacitance ratio RES4 from PCap01

/***************************/                   
/*   if((Dummy_var==0)|(Dummy_var==500) | (Dummy_var==1000))
    {
      SPIx_I2Cx_send_1byte(Bus_Type, PCap01_Device_Address, PCap01_Start_Temp_Meas);
      Res11_content = SPIx_I2Cx_read_3bytes(Bus_Type, PCap01_Device_Address, 0x00, 0x0E, 0);
    
      R_ratio = SPIx_I2Cx_read_3bytes(Bus_Type, PCap01_Device_Address, 0x00, 0x0E, 21);
        
      Temperature = R_ratio * 351.26 + (-270.57);
    
      printf("\n R_ratio(intern)= %f / Temp= %3.2f?", R_ratio, Temperature);
    }
*/
/***************************/
    Dummy_var++;
  } // --> while with Dummy_var
 
  find_variance_float(1000); //Dummy_var-1
                             // Taking 2000 meas values, using values 1000-2000 for variance for best results
} // --> main

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Function Name: Dly100us
 * Parameters: u32 Dely
 *
 * Return: none
 *
 * Description: Delay Dly * 100us
 *
 ******************************************************************************/
void Dly100us(void *arg)
{
u32 Dely = (u32)arg;
 while(Dely--)
 {
   for( i = LOOP_DLY_100US; i; i--);
 }
}

/*******************************************************************************
 * Function Name: Simple_delay_750ns
 * Parameters: u32 Dely
 *
 * Return: none
 *
 * Description: Delay Dely * 750us
 *
 ******************************************************************************/
void Simple_delay_750ns(void *arg) // With arg 1, gives 750ns delay
{
u32 Dely = (u32)arg;
   for( i = Dely; (i!=0); i--);

}

/*******************************************************************************
 * Function Name  : Embed_SRAM_Init
 * Description    : Embedded SRAM initialization
 * Input          : None
 * Output         : None
 * Return         : None
 ******************************************************************************/
void Embed_SRAM_Init(void)
{
 // Releasing reset for Embedded SRAM
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SRAM, ENABLE);
}

/*******************************************************************************
 * Function Name  : Write_Emb_SRAM_float
 * Description    : Write floating point data to embedded SRAM
 * Input          : None
 * Output         : None
 * Return         : None
 ******************************************************************************/
void Write_Emb_SRAM_float(float ESRAM_24bit_data)
{
  // NOTE: While checking the SRAM contents, please note that the address of
  // consecutively stored results differs by 4. So, for 100 values, address range
  // is from 20000B00 to 20000B00+'d(100*4) = 20000C90
  sram_float_memory[sram_mem_offset] = ESRAM_24bit_data;
  sram_mem_offset++; // offset increments by 4 every time 
}

/*******************************************************************************
 * Function Name  : Write_Emb_SRAM_uint32_t
 * Description    : Write unsigned 32-bit integer data to embedded SRAM
 * Input          : None
 * Output         : None
 * Return         : None
 ******************************************************************************/
void Write_Emb_SRAM_uint32_t(uint32_t ESRAM_24bit_data)
{
  // NOTE: While checking the SRAM contents, please note that the address of
  // consecutively stored results differs by 4. So, for 100 values, address range
  // is from 20000B00 to 20000B00+'d(100*4) = 20000C90
  sram_uint32_t_memory[sram_mem_offset] = ESRAM_24bit_data;
  sram_mem_offset++; // offset increments by 4 every time 
}

/*******************************************************************************
 * Function Name: find_variance_float
 * Parameters: Calculates variance (S.D^2)
 *
 * Return: none
 *
 * Description: 
 *
 ******************************************************************************/
float find_variance_float(uint16_t sample_count)
{
  double      sum = 0;
  float       mean = 0; 
  float       diff;
  float       square;
  float       numerator = 0;
  float       variance;
  float       data;

  sram_mem_offset = 1000;  // Using values 1000-2000 for Variance calculation
  
  for(i=0;i<sample_count;i++)
  {
    data = sram_float_memory[sram_mem_offset]; 
    sum = sum + data;
    sram_mem_offset ++; // For the sram_memory, the address increments by 4 everytime
  }
  
  mean = sum/sample_count;
  sram_mem_offset = 1000; // Using values 1000-2000 for Variance calculation
  
  for(j=0;j<sample_count;j++)
  {
    data = sram_float_memory[sram_mem_offset];
    diff = data - mean;
    square = diff * diff;
    numerator += square;
    sram_mem_offset ++;
  }
  
  //variance = numerator/(sample_count-1); // Sample variance
  variance = numerator/(sample_count); // Population variance variance
  return variance;
}

/*******************************************************************************
 * Function Name: find_variance_uint32_t
 * Parameters: Calculates variance (S.D^2)
 *
 * Return: none
 *
 * Description: 
 *
 ******************************************************************************/
uint32_t find_variance_uint32_t(uint16_t sample_count)
{
  uint64_t    sum = 0;
  uint32_t    mean = 0; 
  uint32_t    diff;
  uint32_t    square;
  uint32_t    numerator = 0;
  uint32_t    variance;
  uint32_t    data;

  sram_mem_offset = 1000;  // Using values 1000-2000 for Variance calculation
  
  for(i=0;i<sample_count;i++)
  {
    data = sram_uint32_t_memory[sram_mem_offset]; 
    sum = sum + data;
    sram_mem_offset ++; // For the sram_memory, the address increments by 4 everytime
  }
  
  mean = sum/sample_count;
  sram_mem_offset = 1000; // Using values 1000-2000 for Variance calculation
  
  for(j=0;j<sample_count;j++)
  {
    data = sram_uint32_t_memory[sram_mem_offset];
    diff = data - mean;
    square    = diff * diff;
    numerator += square;
    sram_mem_offset    ++;
  }
  
  //variance = numerator/(sample_count-1); // Sample variance
  variance = numerator/(sample_count); // Population variance variance
  return variance;
}

/*******************************************************************************
 * Function Name  : Ext_Interrupt_Init
 * Return: none
 *
 * Description    : External interrupt/event controller (EXTI)
 * The external interrupt/event controller consists of 20 edge detector lines
 * used to generate interrupt/event requests. Each line can be independently
 * configured to select the trigger event (rising edge, falling edge, both) and
 * can be masked independently. A pending register maintains the status of the
 * interrupt requests. The EXTI can detect an external line with a pulse width
 * shorter than the Internal APB2 clock period. Up to 80 GPIOs can be connected
 * to the 16 external interrupt lines.
 *
 ******************************************************************************/
void Ext_Interrupt_Init (void)
{    EXTI_InitTypeDef  EXTI_InitStructure;
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource9);
   
   // EXTI_InitTypeDef defined in library

  // Enabling clock and releasing reset
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_AFIO, DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
  EXTI_InitStructure.EXTI_Line      = EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode      = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger   = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd   = ENABLE;
 
  EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
 * Function Name: GPIOs_LED_Test
 * Return: none
 *
 * Description: Init GPIOs to turn on LEDs on PC10, PC11 and PC12 on the test platform
 *
 ******************************************************************************/
void GPIOs_LED_Test(void)
{
  GPIO_InitTypeDef GPIO_InitStructure; // GPIO_InitTypeDef defined in library

  // Test GPIO for driving LEDs
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_WriteBit(GPIOC, GPIO_Pin_10, Bit_RESET);
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_WriteBit(GPIOC, GPIO_Pin_11, Bit_RESET);

  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_RESET);
}

/*******************************************************************************
 * Bus Functions
 ******************************************************************************/

/*******************************************************************************
 * Function Name: SPIx_I2Cx_GPIOs_Init
 * Parameters: Int32U Clk, Int32U Width
 * Return: none
 *
 * Description: Init GPIOs used in SPIx or I2Cx interface
 *
 ******************************************************************************/
void SPIx_I2Cx_GPIOs_Init(void* bus_type)
{

  GPIO_InitTypeDef GPIO_InitStructure; // GPIO_InitTypeDef defined in library

  // Enable GPIO clock and release reset
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | 
                         RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,  DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,  ENABLE);
  
  // SPIx BUS_TYPE -------------------------------------------------------------
  if (bus_type==SPI1 | bus_type==SPI2)
  {
  // Configure   SPI1_CLK  - PA5
  //             SPI1_MOSI - PA7
  //             SPI1_MISO - PA6
  // Chip select SPI1_NSS  - PA4
  // External Interrupt Input line PD4

  // Configure   SPI2_CLK  - PB13
  //             SPI2_MOSI - PB15
  //             SPI2_MISO - PB14
  // Chip select SPI2_NSS  - PB12
  // External Interrupt Input line PE11  
  
// SPI1_NSS
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  if (bus_type==SPI1) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
                        GPIO_Init(GPIOA, &GPIO_InitStructure);
                      } // SPI1 - PA4
  if (bus_type==SPI2) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
                        GPIO_Init(GPIOB, &GPIO_InitStructure);
                      } // SPI2 - PB12

//  SPI1_CLK
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  if (bus_type==SPI1) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
                        GPIO_Init(GPIOA, &GPIO_InitStructure);
                      } // SPI1 - PA5
  if (bus_type==SPI2) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
                        GPIO_Init(GPIOB, &GPIO_InitStructure);
                      } // SPI2 - PB13

//   SPI1_MISO
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  if (bus_type==SPI1) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
                        GPIO_Init(GPIOA, &GPIO_InitStructure);
                      } // SPI1 - PA6
  if (bus_type==SPI2) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
                        GPIO_Init(GPIOB, &GPIO_InitStructure);
                      } // SPI2 - PB14
  
//   SPI1_MOSI
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  if (bus_type==SPI1) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
                        GPIO_Init(GPIOA, &GPIO_InitStructure);
                      } // SPI1 - PA7
  if (bus_type==SPI2) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
                        GPIO_Init(GPIOB, &GPIO_InitStructure);
                      } // SPI1 - PB15
 
  SPI_I2S_DeInit(bus_type);

// External Interrupt Input
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                 // _IPD at GP22
  if (bus_type==SPI1) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
                        GPIO_Init(GPIOD, &GPIO_InitStructure);
                      } // SPI1 - PD4
  if (bus_type==SPI2) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
                        GPIO_Init(GPIOE, &GPIO_InitStructure);
                      } // SPI2 - PE11
  
// SPI ENABLE Output for the evaluation kit
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  if (bus_type==SPI1) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
                        GPIO_Init(GPIOD, &GPIO_InitStructure);
                      } // SPI1 - PD3
  if (bus_type==SPI2) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
                        GPIO_Init(GPIOE, &GPIO_InitStructure);
                      } // SPI2 - PE10

  // SPIx Enable = 0 for SPI Mode // SPIx Enable = RSN for GP22
  if (bus_type==SPI1) GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET); // SPI1 - PD3
  if (bus_type==SPI2) GPIO_WriteBit(GPIOE, GPIO_Pin_10, Bit_RESET); // SPI2 - PE10
  }
  
  // I2Cx BUS_TYPE -------------------------------------------------------------
  if (bus_type==I2C1 | bus_type==I2C2)
  {
  // Configure I2C1_SCL  - PB6
  //           I2C1_SDA  - PB7
  // External Interrupt Input line on PD4
  
  // Configure I2C2_SCL  - PB11
  //           I2C2_SDA  - PB10
  // External Interrupt Input line on PE11
  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 
  if (bus_type==I2C1) { GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
                      } // I2C1 - PB6 PB7
  if (bus_type==I2C2) { GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_10;
                      } // I2C2 - PB11 PB10
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  I2C_DeInit(bus_type);

// External Interrupt Input
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  if (bus_type==I2C1) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
                        GPIO_Init(GPIOD, &GPIO_InitStructure);
                      } // I2C1 - PD4
  if (bus_type==I2C2) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
                        GPIO_Init(GPIOE, &GPIO_InitStructure);
                      } // I2C2 - PE11

// SPI ENABLE SLOT1 Output for the evaluation kit
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  if (bus_type==I2C1) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
                        GPIO_Init(GPIOD, &GPIO_InitStructure);
                      } // I2C1 - PD3
  if (bus_type==I2C2) { GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
                        GPIO_Init(GPIOE, &GPIO_InitStructure);
                      } // I2C2 - PE10
  
  // SPIx Enable = 1 for I2C Mode
  if (bus_type==I2C1) GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET); // I2C1 - PD3
  if (bus_type==I2C2) GPIO_WriteBit(GPIOE, GPIO_Pin_10, Bit_SET); // I2C2 - PE10
  }

}

/*******************************************************************************
 * Function Name: SPIx_I2Cx_Interface_Init
 * Parameters: Int32U Clk, Int32U Width
 * Return: none
 *
 * Description: Init SPIx or I2Cx Interface
 *
 ******************************************************************************/
void SPIx_I2Cx_Interface_Init(void* bus_type)
{
  // SPIx BUS_TYPE -------------------------------------------------------------
  if (bus_type==SPI1 | bus_type==SPI2)
  {
  // Initialising the SPIx interface
  SPI_InitTypeDef SPI_InitStructure;
  
    /* Configures the system clock (SYSCLK) */
  RCC_SYSCLKConfig (RCC_SYSCLKSource_HSI); // Source-freq. 8.000MHz
  //RCC_SYSCLKConfig (RCC_SYSCLKSource_HSE); // Source-freq. 20.000MHz
  //RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK); // Source-freq. 57.6MHz ( (72MHz/25MHz) * HSE) )
  
    /* Adjusts the Internal High Speed oscillator (HSI) calibration value.
    * @param  HSICalibrationValue: specifies the calibration trimming value.
    *   This parameter must be a number between 0 and 0x1F. */
  //RCC_AdjustHSICalibrationValue(0x10); //0x00..0x0F // 3.8..4.2MHZ

    /* Configures the AHB clock (HCLK) */
  RCC_HCLKConfig (RCC_SYSCLK_Div1);


  // Clock Enable and Reset release
  if (bus_type==SPI1)
  {
    RCC_APB2PeriphResetCmd (RCC_APB2Periph_SPI1, DISABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);
  }

  if (bus_type==SPI2)
  {
    RCC_APB1PeriphResetCmd (RCC_APB1Periph_SPI2, DISABLE);
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_SPI2, ENABLE);
    RCC_PCLK1Config (RCC_HCLK_Div1); // in order to adapt the clock frequenz
  }
  
  // All are defined in stm32f10x_spi.h
  SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;    
  SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
  // SPI frequence devider
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;

  SPI_Init(bus_type, &SPI_InitStructure);
  SPI_Cmd(bus_type, ENABLE); // Enabling the SPIx Interface

  // Enabling the NSS Output during transmission
  SPI_SSOutputCmd (bus_type, ENABLE); 
  // SPIx - SSN to Device - Set to High for reset
  if (bus_type==SPI1) GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
  if (bus_type==SPI2) GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
  }

  // I2Cx BUS_TYPE -------------------------------------------------------------
  if (bus_type==I2C1 | bus_type==I2C2)
  {
  // Initialising the I2C interface
  I2C_InitTypeDef  I2C_InitStructure;

    /* Configures the system clock (SYSCLK) */
  RCC_SYSCLKConfig (RCC_SYSCLKSource_HSI); // I2C-freq. ClockSpeed kHz
  //RCC_SYSCLKConfig (RCC_SYSCLKSource_HSE); // I2C-freq. ClockSpeed/3 kHz
  //RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK); // I2C-freq. ClockSpeed/3.12012 kHz
  
    /* Adjusts the Internal High Speed oscillator (HSI) calibration value.
    * @param  HSICalibrationValue: specifies the calibration trimming value.
    *   This parameter must be a number between 0 and 0x1F. */
  //RCC_AdjustHSICalibrationValue(0x10); //0x00..0x0F // -5..+5%

  // Clock Enable and Reset release
  if (bus_type==I2C1)
  {
  RCC_APB1PeriphResetCmd(  RCC_APB1Periph_I2C1, DISABLE);
  RCC_APB1PeriphClockCmd(  RCC_APB1Periph_I2C1, ENABLE);
  }
  if (bus_type==I2C2)
  {
  RCC_APB1PeriphResetCmd(  RCC_APB1Periph_I2C2, DISABLE);
  RCC_APB1PeriphClockCmd(  RCC_APB1Periph_I2C2, ENABLE);
  }
    
  // I2Cx configuration
  I2C_InitStructure.I2C_Mode        = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle   = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_Ack         = I2C_Ack_Enable;
  I2C_InitStructure.I2C_OwnAddress1 = 0xAA;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  // I2C frequence
  I2C_InitStructure.I2C_ClockSpeed  = 100000;

  // Apply I2Cx configuration after enabling it
  I2C_Init(bus_type, &I2C_InitStructure);
  // I2Cx Peripheral Enable
  I2C_Cmd(bus_type, ENABLE);
    
  }  
}

/*******************************************************************************
 * Device Functions, write standard config
 ******************************************************************************/

/*******************************************************************************
 * Function Name: SPIX_I2Cx_write_standard_config
 * Parameters: none
 *
 * Return: none
 *
 * Description: Writes the standard config. specified in Device with the data 
 *
 ******************************************************************************/
void SPIx_I2Cx_write_standard_config(void *bus_type, uint8_t device_address)
{
  if(device_address==0xA0) // configuration for PCap01
    {
      uint8_t reg=0x00; // Config Address (Register 0..20)
      for (  n = 0; n < 21; ++n) {       
        SPIx_I2Cx_write_config_reg(bus_type, device_address, reg, PCap01_standard_config_reg[n]);
        ++reg;
      }
    }

 // else printf("\n There is no standard config loaded");
}

/*******************************************************************************
 * Device Functions, write config reg
 ******************************************************************************/

/*******************************************************************************
 * Function Name: SPIx_I2Cx_write_config_reg
 *
 * Parameters: Opcode = b'11 + <addr5.....0> = 8 bit opcode_addr
 *
 * Command      | Byte 3               | Byte 2  | Byte 1  | Byte 0  |
 * Write Config | 1 1 Cf_Address<5..0> | Registry Parameter<23..0>   |
 *
 * Return: none
 *
 * Description: Writes the config.reg. specified in Device with the data 
 *
 ******************************************************************************/
void SPIx_I2Cx_write_config_reg(void *bus_type, uint8_t device_address, uint8_t write_address, uint32_t config_reg_data)
{
   uint8_t Data_Byte_Lo   = config_reg_data;
   uint8_t Data_Byte_Mid  = config_reg_data>>8;
   uint8_t Data_Byte_Hi   = config_reg_data>>16;
   
   uint8_t opcode_write_config = 0xC0;
   
   uint8_t write_opcode_address = opcode_write_config | write_address;

  // SPIx BUS_TYPE -------------------------------------------------------------
  if (bus_type==SPI1 | bus_type==SPI2)
  {
   
// Deactivating Reset
   if (bus_type==SPI1) GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET); 
   if (bus_type==SPI2) GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
      
      while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_TXE)==0) {} 
      SPI_I2S_SendData(bus_type, write_opcode_address);  // CFG WR ADDRESS

      while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_TXE)==0) {}
      SPI_I2S_SendData(bus_type, Data_Byte_Hi);  // DATA BYTE HIGH 

      while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_TXE)==0) {}
       SPI_I2S_SendData(bus_type, Data_Byte_Mid);  // DATA MID 

      while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_TXE)==0) {}
       SPI_I2S_SendData(bus_type, Data_Byte_Lo);  // DATA LOW

      while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_TXE)==0) {}
// Delay have to be checked with different Slot_x
  Simple_delay_750ns((void*)160); // For 1 MHz SPI clock only
      
      if (bus_type==SPI1) GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
      if (bus_type==SPI2) GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
  //Simple_delay_750ns((void*)1);
  }
  
  // I2Cx BUS_TYPE -------------------------------------------------------------
  if (bus_type==I2C1 | bus_type==I2C2)
  {
   I2Cx_send_opcode_addr(bus_type, device_address, write_opcode_address, FALSE); 
    //Sends START + {Dev.address + WRITE Bit} + {opcode,cfg_address} 

    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_TXE)==0) {}
    I2C_SendData(bus_type, Data_Byte_Hi);              // DATA BYTE HIGH 

 Simple_delay_750ns((void*)10);
    
    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_TXE)==0) {}
    I2C_SendData(bus_type, Data_Byte_Mid);              // DATA BYTE MID 

 Simple_delay_750ns((void*)10);
    
    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_TXE)==0) {}
    I2C_SendData(bus_type, Data_Byte_Lo);                // DATA BYTE LO 

 Simple_delay_750ns((void*)10);
    
    while ( (I2C_GetFlagStatus(bus_type, I2C_FLAG_TXE)==0) || (I2C_GetFlagStatus(bus_type, I2C_FLAG_BTF)==0) )
    {} 
    I2C_GenerateSTOP(bus_type, ENABLE);           // Stop bit

  Simple_delay_750ns((void*)1);
  }  
      
}

/*******************************************************************************
 * Device Functions, send 1byte
 ******************************************************************************/

/*******************************************************************************
 * Function Name: SPIx_I2Cx_send_1byte
 * Parameters: Opcode byte
 *
 * Return: none
 *
 * Description: Writes the Opcode to device
 *
 ******************************************************************************/
void SPIx_I2Cx_send_1byte(void *bus_type, uint8_t device_address, uint8_t opcode_byte)
{
  // SPIx BUS_TYPE -------------------------------------------------------------
  if (bus_type==SPI1 | bus_type==SPI2)
  {
     // Deactivating Reset SPIx
     if (bus_type==SPI1) GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
     if (bus_type==SPI2) GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
     
     while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_TXE)==0) {} 
     SPI_I2S_SendData(bus_type, opcode_byte);     // OPCODE TO Device 
     while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_TXE)==0) {} 
  Simple_delay_750ns((void*)10); // important delay (16) at SPI freq.=750kHz

     // Reset to device SPIx
     if (bus_type==SPI1) GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
     if (bus_type==SPI2) GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
  }
  // I2Cx BUS_TYPE -------------------------------------------------------------
  if (bus_type==I2C1 | bus_type==I2C2)
  {
    I2C_GenerateSTART(bus_type, ENABLE); // Start
  Dly100us((void*)2);                 // COMPULSORY HOLD TIME FOR THE START BIT
    
    I2C_GetFlagStatus(bus_type, I2C_FLAG_SB);
  Simple_delay_750ns((void*)1);
    I2C_SendData(bus_type, device_address);        // Device address of device

    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_ADDR)==0) {} // SR1
    I2C_GetFlagStatus(bus_type, I2C_FLAG_TRA);               // SR2

    //__no_operation();
    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_TXE)==0) {} 
    I2C_SendData(bus_type, opcode_byte);                     // OPCODE TO device
    while ( (I2C_GetFlagStatus(bus_type, I2C_FLAG_TXE)==0) || (I2C_GetFlagStatus(bus_type, I2C_FLAG_BTF)==0) )
      {} 
    I2C_GenerateSTOP(bus_type, ENABLE);                      // Stop
  }
}

/*******************************************************************************
 * Device Functions, read 3bytes
 ******************************************************************************/

/*******************************************************************************
 * Function Name: SPIx_I2Cx_read_3bytes
 *
 * Parameters: Opcode = b'01 + <addr5.....0> = 8 bit opcode_addr
 *
 * Command      | Byte 3               | Byte 2  | Byte 1  | Byte 0  |
 * Read Results | 0 1 Rs_Address<5..0> | Measurement Results<23..0>  |
 *
 * Return: 3 bytes from the specified read address
 *
 * Description: Reads 3 bytes from an address in device
 *
 ******************************************************************************/
float SPIx_I2Cx_read_3bytes(void *bus_type, uint8_t device_address, uint8_t read_opcode, uint8_t read_addr, uint8_t fractional_bits)
{
  uint8_t     n_bytes = 3;
  
  uint32_t    Result_read = 0;
  float       Result = 0;
  uint8_t     read_opcode_addr = read_opcode | read_addr;

  //.............. Result = n Byte = n x 8 bits......................

  // SPIx BUS_TYPE -------------------------------------------------------------

  if (bus_type==SPI1 | bus_type==SPI2) 
  {
       // Deactivating Reset SPIx
       if (bus_type==SPI1) GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
       if (bus_type==SPI2) GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
      
       SPI_I2S_SendData(bus_type, read_opcode_addr);  // READ OPCODE + Address
       
       while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_TXE)==RESET) {};
  Simple_delay_750ns((void*)10); // important delay (16) at SPI freq.=750kHz
       
       //Compulsory reads to DR and SR to clear OVR,
       //so that next incoming data is saved
       SPI_I2S_ReceiveData(bus_type);                     // To clear OVR
       SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_TXE); // To clear OVR

         //Reading byte1
         SPI_I2S_SendData(bus_type, 0x00FF);  // DUMMY WRITE
         // Wait until RX buffer is not empty, then read the received data
         while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_RXNE)==0) {}
         Result_read = SPI_I2S_ReceiveData(bus_type); //  Read

       for ( n = 1; n < n_bytes; n++)
       {       
         //Reading byte2 .. byte.n
         SPI_I2S_SendData(bus_type, 0x00FF);  // DUMMY WRITE
         // Wait until RX buffer is not empty, then read the received data
         while (SPI_I2S_GetFlagStatus(bus_type, SPI_I2S_FLAG_RXNE)==0) {}

         Result_read = Result_read<<8;
         Result_read |= SPI_I2S_ReceiveData(bus_type); //  Read
       }

       // Reset to device SPIx
       if (bus_type==SPI1) GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
       if (bus_type==SPI2) GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
  }

  // I2Cx BUS_TYPE -------------------------------------------------------------
  
  if (bus_type==I2C1 | bus_type==I2C2) 
  {
  // DEVICE_ADDR, {RD_OPCODE, ADDR}, STOP BIT
  I2Cx_send_opcode_addr(bus_type, device_address, read_opcode_addr, TRUE); 

   //Sends Re-START + {Dev.address + READ Bit} + 
   //Read ACK + HIGH_Byte + Send ACK + Read MIDDLE_Byte + Send ACK +
   //Read LOW_Byte + Send NACK + STOP BIT
  
    Dly100us((void*)1);                             // Delay b/w Master transmit to Master receive switch

    I2C_GenerateSTART(bus_type, ENABLE);                // Re-Start
    Dly100us((void*)1);                             // COMPULSORY HOLD TIME FOR THE START BIT
    
    I2C_GetFlagStatus(bus_type, I2C_FLAG_SB);
    I2C_SendData(bus_type, device_address+1);                     // Device address of device + (READ BIT = 1)

  Simple_delay_750ns((void*)3);
    
    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_ADDR)==0) {}  // I2Cx_SR1 read
    I2C_GetFlagStatus(bus_type, I2C_FLAG_TRA);                // I2Cx_SR2 read

// Here 2 bytes are read on completion of SR1 and SR2
    
    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_RXNE)!=1 ||
           I2C_GetFlagStatus(bus_type, I2C_FLAG_BTF) !=1) {} // Waiting till first 2 bytes are got

    Result_read |= I2C_ReceiveData(bus_type);                  // Getting the PCap01's BYTE1
    Result_read = Result_read<<8;         

    I2C_AcknowledgeConfig(bus_type, DISABLE);                 // Program NACK for last byte
    I2C_GenerateSTOP(bus_type, ENABLE);                       // Program Stop to occur after last byte

    Result_read |= I2C_ReceiveData(bus_type);                  // Getting the PCap01's BYTE2
    Result_read = Result_read<<8;         

    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_RXNE)!=1) {}  // Wait till 3rd byte is completely received
    Result_read |= I2C_ReceiveData(bus_type);                  // Getting the PCap01's BYTE3

   Dly100us((void*)5);                                    // This delay is compulsory so that the STOP is sent correctly after the 3rd byte
 
   I2C_AcknowledgeConfig(bus_type, ENABLE);                  // Enabling ACK again for further transactions    

  }
  
  Result = Result_read;

  return Result;
}     

/*******************************************************************************
 * Device Functions, send opcode addr
 ******************************************************************************/

/*******************************************************************************
 * Function Name: I2Cx_send_opcode_addr
 * Parameters:
 *
 * Return:
 *
 * Description: This function sends START BIT + 
 * (DEVICE_ADDR + WRITE_BIT) + {OPCODE,ADDRESS} + Optional STOP BIT TO PCap01
 * Sequence followed as described in i2c pgm.pdf
 *
 ******************************************************************************/
void I2Cx_send_opcode_addr(void *bus_type, uint8_t device_addr, uint8_t opcode_addr, bool SEND_STOP_BIT)
{
    I2C_GenerateSTART(bus_type, ENABLE);                        // Start
    Dly100us((void*)1);                                     // COMPULSORY HOLD TIME FOR THE START BIT
    
    I2C_GetFlagStatus(bus_type, I2C_FLAG_SB);
    I2C_SendData(bus_type, device_addr);               // Device address of device

  Simple_delay_750ns((void*)10);

    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_ADDR)==0) {}  // SR1
    I2C_GetFlagStatus(bus_type, I2C_FLAG_TRA);                // SR2

    while (I2C_GetFlagStatus(bus_type, I2C_FLAG_TXE)==0) {} 
    I2C_SendData(bus_type, opcode_addr);                // {opcode,address} TO device

  Simple_delay_750ns((void*)10);

  if (SEND_STOP_BIT==1)
    {
       while ( (I2C_GetFlagStatus(bus_type, I2C_FLAG_TXE)==0) || (I2C_GetFlagStatus(bus_type, I2C_FLAG_BTF)==0) )
         {} 
       I2C_GenerateSTOP(bus_type, ENABLE);           // Stop
    }
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
/******************* (C) COPYRIGHT 2012 acam messelectronic GmbH **************/


