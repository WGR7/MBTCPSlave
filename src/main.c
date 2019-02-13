
/* Includes */
//#include <_TaskSPITxRx.h>
#include <stddef.h>
#include "stm32f10x.h"
#include "custom_init.h"
#include "stm32f10x_rcc.h"
#include "stm32f1xx_it.h"
#include "string.h"

/*
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "TaskW5500.h"
#include "TaskMBParser.h"
*/

#include "w5500_spi.h"

// Wiznet stuff
#define _WIZCHIP_					W5500
#define _WIZCHIP_IO_MODE_           _WIZCHIP_IO_MODE_SPI_
#define _WIZCHIP_IO_BUS_WIDTH_		8

#include "wizchip_conf.h"
_WIZCHIP  WIZCHIP;

static void vTaskAlive(void *pvParameters);
//static void vTaskSPITxRx(void *pvParameters);

//extern sW5500Config W5500Conf;


int main(void)
{
	CustomInit();
	RCC_ClocksTypeDef zegary;
	RCC_GetClocksFreq(&zegary);

	// relevant for freertos
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	reg_wizchip_cris_cbfunc(NULL, NULL);
	reg_wizchip_cs_cbfunc(W5500_IF_Select, W5500_IF_DeSelect);
	// provide read/write functions both byte-wise and burst
	reg_wizchip_spiburst_cbfunc(W5500_IF_ReadBurst, W5500_IF_WriteBurst);
	reg_wizchip_spi_cbfunc(W5500_IF_ReadByte, W5500_IF_WriteByte);

	uint8_t version=0;
	version = getVERSIONR();

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

	vTaskStartScheduler();
*/

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

