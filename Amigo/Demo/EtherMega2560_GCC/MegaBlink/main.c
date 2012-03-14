////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////    main.c
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <avr/io.h>


/* Scheduler include files. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

/* Pololu derived include files. */
#include <digitalAnalog.h>

/* serial interface include file. */
#include <lib_serial.h>


/*-----------------------------------------------------------*/
/* Create a handle for the serial port. */
xComPortHandle xSerialPort;

/* v coverclock@diag.com 2012-03-04 */
static portTASK_FUNCTION(TaskBlinkRedLED, pvParameters); // Main Arduino Mega 2560, Freetronics EtherMega (Red) LED Blink

static portTASK_FUNCTION(TaskBlinkGreenLED, pvParameters); // Main Arduino Uno 328p (Green) LED Blink
/* ^ coverclock@diag.com 2012-03-04 */
/*-----------------------------------------------------------*/

/* Main program loop */
int main(void)
{
	// v coverclock@diag.com 2012-03-14
	MCUCR &= ~_BV(JTD); // Enable JTAG debugging.
	MCUCR &= ~_BV(IVSEL); // Take interrupt vectors.
	// ^ coverclock@diag.com 2012-03-14

    // turn on the serial port for debugging or for other USART reasons.
	xSerialPort = xSerialPortInitMinimal( 115200, 80, 16); //  serial port: WantedBaud, TxQueueLength, RxQueueLength (8n1)

	avrSerialPrint_P(PSTR("\r\n\n\nHello World!\r\n")); // Ok, so we're alive...

#if 1
    xTaskCreate(
		TaskBlinkRedLED
		,  (const signed portCHAR *)"RedLED" // Main Arduino Mega 2560, Freetronics EtherMega (Red) LED Blink
		,  168				// Tested 9 free
		,  NULL
		,  3
		,  NULL ); // */
#endif

#if 0
    xTaskCreate(
		TaskBlinkGreenLED
		,  (const signed portCHAR *)"GreenLED" // Main Arduino Uno 328p (Green) LED Blink
		,  168				// Tested 9 free
		,  NULL
		,  3
		,  NULL ); // */
#endif

	avrSerialPrintf_P(PSTR("\r\n\nFree Heap Size: %u\r\n"),xPortGetFreeHeapSize() ); // needs heap_1 or heap_2 for this function to succeed.

    vTaskStartScheduler();

	avrSerialPrint_P(PSTR("\r\n\n\nGoodbye... no space for idle task!\r\n")); // Doh, so we're dead...

    //and finally, to eliminate compiler warnings....
    return(0);
}

/*-----------------------------------------------------------*/


static void TaskBlinkRedLED(void *pvParameters) // Main Red LED Flash
{
    (void) pvParameters;;
    portTickType xLastWakeTime;
	/* The xLastWakeTime variable needs to be initialised with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();

    while(1)
    {

    	setDigitalOutput( IO_B7, 1);               // main EtherMega LED on
		vTaskDelayUntil( &xLastWakeTime, ( 1000 / portTICK_RATE_MS ) ); // < coverclock@diag.com 2012-03-12

		setDigitalOutput( IO_B7, 0);               // main EtherMega LED  off
		vTaskDelayUntil( &xLastWakeTime, ( 2000 / portTICK_RATE_MS ) ); // < coverclock@diag.com 2012-03-12

#if 0
		xSerialPrintf_P(PSTR("RedLED HighWater @ %u\r\n"), uxTaskGetStackHighWaterMark(NULL));
#endif
    }

}

/*-----------------------------------------------------------*/
static void TaskBlinkGreenLED(void *pvParameters) // Main Green LED Flash
{
    (void) pvParameters;;
    portTickType xLastWakeTime;
	/* The xLastWakeTime variable needs to be initialised with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();

    while(1)
    {

    	setDigitalOutput( IO_B5, 1);               // main Ardino LED on

		vTaskDelayUntil( &xLastWakeTime, ( 200 / portTICK_RATE_MS ) );

		setDigitalOutput( IO_B5, 0);               // main Ardino LED off

		vTaskDelayUntil( &xLastWakeTime, ( 100 / portTICK_RATE_MS ) );

		xSerialPrintf_P(PSTR("GreenLED HighWater @ %u\r\n"), uxTaskGetStackHighWaterMark(NULL));
    }

}
/*-----------------------------------------------------------*/

