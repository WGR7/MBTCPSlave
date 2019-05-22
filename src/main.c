
/* Includes */
//#include <_TaskSPITxRx.h>
#include <stddef.h>
#include "stm32f10x.h"
#include "custom_init.h"
#include "stm32f10x_rcc.h"
#include "stm32f1xx_it.h"
#include "string.h"
#include <stdio.h>

// freeRTOS stuff
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

// app specific stuff
#include "app_config.h"
#include "TaskDHCPClient.h"
#include "TaskTCP_CLIServer.h"
#include "TaskDiscoveryServer.h"
#include "w5500_spi.h"
#include "ssd1306.h"
#include "ssd1306_port_f103.h"
#include "TaskSSD1306.h"
#include "utils/Console/Console.h"
#include "utils/Console/TomThumb.h"
#include "utils/pindebug.h"
#include "utils/AT24Cxxx/AT24Cxxx.h"


// Wiznet stuff
#define _WIZCHIP_					W5500
#define _WIZCHIP_IO_MODE_           _WIZCHIP_IO_MODE_SPI_
#define _WIZCHIP_IO_BUS_WIDTH_		8
#include "wizchip_conf.h"
_WIZCHIP  WIZCHIP;



static void vTaskAlive(void *pvParameters);
//static void vTaskSPITxRx(void *pvParameters);

//extern sW5500Config W5500Conf;

// Display structure
sSSDdisplay display;
// Text console
char consolebuffer[8*25];
sConsole console;


// MUTEX for SPI access
SemaphoreHandle_t SPI_Mutex;

// temporary, for testing
extern void AT24Cxxx_HWSetup();
extern int16_t AT24Cxxx_RandomRead(uint8_t DeviceAddress, uint16_t Register);

int main(void)
{


	CustomInit();
	RCC_ClocksTypeDef zegary;
	RCC_GetClocksFreq(&zegary);

	PINDEBUG_CH_A_OFF;
	PINDEBUG_CH_B_OFF;

	// relevant for freertos
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	// WiznetIO lib config
	reg_wizchip_cris_cbfunc(NULL, NULL);
	reg_wizchip_cs_cbfunc(W5500_IF_Select, W5500_IF_DeSelect);
	// provide read/write functions both byte-wise and burst
	reg_wizchip_spiburst_cbfunc(W5500_IF_ReadBurst, W5500_IF_WriteBurst);
	reg_wizchip_spi_cbfunc(W5500_IF_ReadByte, W5500_IF_WriteByte);

	AT24Cxxx_HWSetup();

	uint8_t result = 0;
	result = UT_WriteReadTest();



//	AT24Cxxx_ByteWrite(0b00000011, 15, 150);
//	AT24Cxxx_ByteWrite(0b00000011, 16, 160);
//	AT24Cxxx_ByteWrite(0b00000011, 17, 170);

	//uint8_t readval[40];
	//memset(readval, 0x00, 40);
	//eResult result[4];
	//result[0] = AT24Cxxx_ReadToBuffer(0b00000011, 10, 10, readval);
	//result[1] = AT24Cxxx_ReadToBuffer(0b00000011, 10, 10, readval+10);
	//result[2] = AT24Cxxx_ReadToBuffer(0b00000011, 10, 10, readval+20);
	//result[3] = AT24Cxxx_ReadToBuffer(0b00000011, 10, 10, readval+30);

	/*
	// prepare IP config
	uint8_t ipaddr[] 	= {192, 168, 1, 40};
	uint8_t gateway[] 	= {192, 168, 1, 1};
	uint8_t subnet[]	= {255, 255, 255, 0};
	uint8_t mac[]		= {0xBC, 0xE1, 0x43, 0x04, 0xB2, 0x51};
	memcpy(W5500Conf.IP, ipaddr, sizeof(ipaddr));
	memcpy(W5500Conf.Gateway, gateway, sizeof(gateway));
	memcpy(W5500Conf.Subnet, subnet, sizeof(subnet));
	memcpy(W5500Conf.MAC, mac, sizeof(mac));
	W5500Conf.UseDHCP = 1;

	// prepare streams and message queue
	StreamBufferHandle_t socket0stream = xStreamBufferCreate(SOCKET_RXBUFFER_LEN, 5);
	if(socket0stream == NULL){
		while(1){}
	}
	QueueHandle_t  w5500controlmessagequeue = xQueueCreate(10, sizeof(sW5500ControlMessage));
	if(w5500controlmessagequeue == NULL){
		while(1){}
	}
	// bind it to W5500CB
	W5500CB.Sockets[0].RXStreamHandle = socket0stream;
	W5500CB.ControlMessageQueue = w5500controlmessagequeue;
	// bind it to parser for socket0
	static sParserInOutParameter parser0params;
	parser0params.inputStream = W5500CB.Sockets[0].RXStreamHandle;
	parser0params.outputMessageQueue = W5500CB.ControlMessageQueue;
	parser0params.sourceSocketNo = 0;

	// prepare modbus data model
	DataModelInit();

	//xTaskCreate(vTaskSPITxRx, (unsigned char*)"spi", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(vTaskW5500, (const char*)"w5500", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
	xTaskCreate(vTaskMBParser, (const char*)"MBp", configMINIMAL_STACK_SIZE+sizeof(sADUFrame), &parser0params, 3, NULL);
*/
	// Create semaphore for SPI access
	SPI_Mutex = xSemaphoreCreateMutex();

	// Display control stuff
	//SSDDMAConfig(display.PixBuffer, SSD1306_PIXELS_X * SSD1306_PAGES);
	SSD1306InitStruct(&display, &SSDSelectChip, &SSDDeSelectChip, &SSDSetDataMode, &SSDSetCommandMode, &SSDSendByte, &SSDSendDMA, &SSDResetActive, &SSDResetInactive, &SSDDelay);
	xTaskCreate(vTaskSSD1306, (const char*)"ssd", configMINIMAL_STACK_SIZE+300, (void*)&display, 8, NULL);

	// Console stuff
	ConsoleInit(&console, consolebuffer, 8, 25, 0, 6, 5, 8, TomThumb);
	ConsoleClear(&console);

	//xTaskCreate(vTaskDHCPClient, (const char*)"dhcpc", 2*configMINIMAL_STACK_SIZE, NULL, 6, NULL);

	sCLIConfig cliconf;
	cliconf.PortNumber = PORTNO_CLI;
	cliconf.SocketNumber = SOCKETNO_CLI;
	//xTaskCreate(vTaskTCP_CLIServer, (const char*)"cli", configMINIMAL_STACK_SIZE, (void*)&cliconf, 5, NULL);


	sDiscoveryConfig discoveryconf;
	discoveryconf.PortNumber = PORTNO_DISCOVERY;
	discoveryconf.SocketNumber = SOCKETNO_DISCOVERY;
	//xTaskCreate(vTaskDiscoveryServer, (const char*)"dis", 150, (void*)&discoveryconf, 7, NULL);

	CONSOLEPRINT(&console, "Starting FreeRTOS scheduler\n");
	vTaskStartScheduler();


  /* TODO - Add your application code here */

  /* Infinite loop */
  while (1)
  {

  }
}


//extern StreamBufferHandle_t SPIWriteStreamHandle;
/*
static void vTaskAlive( void *pvParameters ){
	while(1){
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		vTaskDelay(500/portTICK_PERIOD_MS);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		vTaskDelay(500/portTICK_PERIOD_MS);
		uint8_t rdbuff[] = {7, 8, 9};
		xStreamBufferSend(SPIWriteStreamHandle, &rdbuff, 3, pdMS_TO_TICKS(10));
	}
}
*/
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ){
	while(1){}
}
// Because fuck you! Compiler wants it any way despite it being switched off in FreeRTOSConfig...
void vApplicationMallocFailedHook( void ){
	while(1){}
}
void vApplicationIdleHook(void){}

void vApplicationTickHook(void){}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
  while(1)
  {}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr)
{
   __assert_func (file, line, NULL, failedexpr);
}

#ifdef USE_SEE
#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval sEE_FAIL.
  */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* Return with error code */
  return sEE_FAIL;
}
#endif
#endif /* USE_SEE */

