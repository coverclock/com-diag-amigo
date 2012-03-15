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

#include <stdint.h> // < coverclock 2012-03-14


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
	uint8_t mcusr = MCUSR;
	MCUSR = 0x0;
	MCUCR &= ~( _BV(JTD) | _BV(IVSEL) ); // Enable JTAG debugging. Take interrupt vectors.
	MCUSR &= ~( _BV(WDRF) ); // Reset Watchdog.
	WDTCSR |= ( _BV(WDCE) | _BV(WDE) ); // Enable Watchdog Change. Disable Watchdog Reset.
	WDTCSR = 0x0; // Disable Watchdog.
	// ^ coverclock@diag.com 2012-03-14

    // turn on the serial port for debugging or for other USART reasons.
	xSerialPort = xSerialPortInitMinimal( 115200, 80, 16); //  serial port: WantedBaud, TxQueueLength, RxQueueLength (8n1)

	avrSerialPrintf_P(PSTR("\r\n\n\nHello World! 0x%x\r\n"), mcusr); // Ok, so we're alive...

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

	avrSerialPrintf_P(PSTR("\r\n\nFree Heap Size: %u\r\n"), xPortGetFreeHeapSize() ); // needs heap_1 or heap_2 for this function to succeed.

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
		vTaskDelayUntil( &xLastWakeTime, ( 1000 / portTICK_RATE_MS ) ); // < coverclock@diag.com 2012-03-14

		setDigitalOutput( IO_B7, 0);               // main EtherMega LED  off
		vTaskDelayUntil( &xLastWakeTime, ( 4000 / portTICK_RATE_MS ) ); // < coverclock@diag.com 2012-03-14

		xSerialPrintf_P(PSTR("RedLED HighWater @ %u\r\n"), uxTaskGetStackHighWaterMark(NULL));
    }

}

#if 0
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
#endif

#if 1
#include <avr/interrupt.h>
static volatile uint64_t fake_isr = 0;
#define FAKE_ISR(_N_) ISR(_VECTOR(_N_)) { fake_isr |= (1ULL << _N_); }
FAKE_ISR(1)
FAKE_ISR(2)
FAKE_ISR(3)
FAKE_ISR(4)
FAKE_ISR(5)
FAKE_ISR(6)
FAKE_ISR(7)
FAKE_ISR(8)
FAKE_ISR(9)
FAKE_ISR(10)
FAKE_ISR(11)
FAKE_ISR(12)
FAKE_ISR(13)
FAKE_ISR(14)
FAKE_ISR(15)
FAKE_ISR(16)
FAKE_ISR(17)
FAKE_ISR(18)
FAKE_ISR(19)
FAKE_ISR(20)
FAKE_ISR(21)
FAKE_ISR(22)
FAKE_ISR(23)
FAKE_ISR(24)
//FAKE_ISR(25)
//FAKE_ISR(26)
FAKE_ISR(27)
FAKE_ISR(28)
FAKE_ISR(29)
FAKE_ISR(30)
FAKE_ISR(31)
//FAKE_ISR(32)
FAKE_ISR(33)
FAKE_ISR(34)
FAKE_ISR(35)
FAKE_ISR(36)
FAKE_ISR(37)
FAKE_ISR(38)
FAKE_ISR(39)
FAKE_ISR(40)
FAKE_ISR(41)
FAKE_ISR(42)
FAKE_ISR(43)
FAKE_ISR(44)
FAKE_ISR(45)
FAKE_ISR(46)
FAKE_ISR(47)
FAKE_ISR(48)
FAKE_ISR(49)
FAKE_ISR(50)
FAKE_ISR(51)
FAKE_ISR(52)
FAKE_ISR(53)
FAKE_ISR(54)
FAKE_ISR(55)
FAKE_ISR(56)
#endif
