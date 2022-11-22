
/* File Header
 *
 *  FileName:   SMT_API_TestApp.cpp
 *  Purpose:    This file is the test application - so I can build and test the library
 *               used by the SMT001 - Smart Street Light project - Cubik API interface library
 *  Created:    Initial creation by Graeme Jones, Principle Software Enginneer at Cubik Innovations
 *  When:       First draft written on 2020_11_20
 *  API Doc:    TN_SMT001_02_API_Issx.x.docx
 *
 */


/* File History
 *test
 * GrJo - 2020_11_20 - Initial Creation
 *
 * GrJo - 2020_11_25 - Extensive rework, added automated test suite to call API functions
 *                     Gives examples of how to initialise libs, use GPIO, Dali and BT functions (TX only)
 *
 * GrJo - 2020_12_07 - reworking test function and main loop, include device-device comms
 *
 * GrJo - 2020_12_10 - more changes, defined a MASTER controller to set the min/max light levels
 *                     changed 'neighbour trigger' ratio from 50% to 80% (I cant see a light level difference, but PSU shows it)
 *
 * GrJo - 2020_12_14 - adding ability to read IR sensor - as trigger input, instead of button
 *
 * GrJo - 2020_12_16 - increased main loop freuqency from 1000ms to 100ms (faster response)
 *
*/


/* Version details (common format, for inclusion in debug output) :
 *
 * Example only - update details in #defines below
 *
 * NAME:         main.c
 * VERSION:      0.1
 * DATE:         2020_12_18
 * DESCRIPTION:  main program runtime - API test application
 *
*/

/* NB: Prepend a FILE01_, FILE02_, etc - so I can automatically extract file details */

#define FILE01_FILENAME       "main.c"
#define FILE01_VERSION        "0.1"
#define FILE01_DATE           "2020_12_18"
#define FILE01_DESCRIPTION    " main program runtime - API test application"


/* Includes */
#include "SMT_Cubik_API_Data_Types.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <driver/uart.h>
#include <driver/gpio.h>
#include <math.h>
#include <string.h>


// Enable this for the master device (that can set the light levels)
//#define MASTER_WITH_POT

// Enable this for reading the IR sensor - its idle low (pull to ground) and trigger high (open relay)
#define TRIGGER_WITH_RELAY_LOW


/* Defined Values */
/* NB: when a trigger happens, the counter is set to this, its decremented in the loop
   This is NOT currently a time based (although it needs to function this way)
   Its a decrementing counter
*/
//#define TRIGGER_TIME 20
#define TRIGGER_TIME         200
#define MAIN_LOOP_PAUSE_TIME 100


/* Local Prototypes */

/* Main processing loop, performs event and rule processing */
void Process_Trigger_Status(bool bLocallyTriggered);

/* Output version information at startup */
void vDumpVersionInformation(void);

void UART(void* parameters);

void mainSmartlightLoop(void* parameters);

void blink(uint8_t lightLevel);

/*Smartlights functions for reading trigger pin level and activating the Light*/
//void smartlightDigitalTrigger(int pinNum);


/* Constant Values */

#ifdef ESP32_WROOM
  const int OnboardLedPin = 2; 
  const int buttonPin = 22;     // the number of the pushbutton pin
  const int potPin = 15;        // Pin the pot is connected to

	const int sensePin1 = 19;

#else
//  const int OnboardLedPin = 33; // This is supposed to be the Red Led on the bottom
  	const int OnboardLedPin = 4;  // This is (VERY BRIGHT) white LED on the top (the Flash)
// My dev board
//  const int buttonPin = 14;     // the number of the pushbutton pin
// The SMT prototype board - Iss2
	const int buttonPin = 16;     // the number of the pushbutton pin
	const int potPin = 15;        // Pin the pot is connected to
	static bool activateClause = false; 

	static SemaphoreHandle_t mutex;

	uint8_t newLightLevel = 0U;
	uint8_t prevLightLevel = 0U;
	int uart_buffer_size = 1024;
	QueueHandle_t uart_queue;
	uint8_t *data;

	const uart_port_t uart_num = UART_NUM_0;
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
	};
#endif



// void smartlightDigitalTrigger(int pinNum){
// 	int temp = gpio_get_level(pinNum);
// 	printf("%i",&temp);
// 	if(temp == 1U) {

// 		u8CubikControl_DaliLight_SetLevel(100);
// 		printf("DALI activated\n");

// 	}else {

// 		u8CubikControl_DaliLight_SetLevel(0);
// 		printf("DALI deactivated\n");

// 	}
// }

/* Main 'thread' for the test app */
void vMainTestAppThread(void)
{
  static uint8_t u8ButPressed = FALSE;

    // Main loop, does POC code
  
    /* Logic:
       Read the POT (store as the maximum value, store value/10 as minimum value)
       Read the button
        (if its NOT pressed - set lamp to minimum)
        (if it IS pressed - set lamp to maximum)
    */      
  
    /* Get the button pressed state // changed so pin is activated by other controller */
    u8CubikControl_GPIO_Pin_GetDig(potPin, &u8ButPressed);

#ifdef TRIGGER_WITH_RELAY_LOW
    /* Decide what to set the light to  - IR sensor relay is enabled when passive, shorts to ground - relay opens when triggered, internal pullup to HIGH */
    if(CA_PIN_VALUE_HIGH == u8ButPressed)
    {
      /* IR Sensor IS triggered - set light to max value */
      Process_Trigger_Status(true);
	  printf("Process Trigger High");
    }
    else
    {
      /* IR sensor is NOT triggered - set light to min value */
      Process_Trigger_Status(false);
	  printf("Process Trigger High");
    }
#else
    /* Decide what to set the light to  - internal pullup to HIGH, active is short to ground */
    if(CA_PIN_VALUE_LOW == u8ButPressed)
    {
      /* Button IS pressed - set light to max value */
      Process_Trigger_Status(true);
    }
    else
    {
      /* Button IS released - set light to min value */
      Process_Trigger_Status(false);
    }
#endif
}


/* Function (called by main loop), take all status values and event messages, decide what to do - using rules and timers */
void Process_Trigger_Status(bool bLocallyTriggered)
{
	static uint8_t u8MyCurrentState = CA_MY_STATUS_SLEEPING;
	static uint8_t u8MyNewState = CA_MY_STATUS_SLEEPING;
	static uint64_t u64LoopCounter = 0U;
	static uint64_t u64PostTriggerTimer = 0U;
	static uint64_t u64MyLastChangeTime = 0U;
	static bool bNeighbourTrigger = false;

	// Default these localy - have a 'master unit' set them different if needed (it will have the POT, slave units wont)
	static uint8_t u8MinLightLevel = 10U;
	static uint8_t u8MaxLightLevel = 100U;

	uint8_t u8TargetDaliLevel = 0U;
	uint8_t u8Result = 0U;
	uint8_t u8DeviceSlot = 0U;
	uint8_t u8OpCode = 0U;
	uint8_t u8Flags = 0U;
	uint64_t u64MsgAge = 0U;

	uint8_t u8RequestedMaxLightLevel = 0U;
	uint8_t u8RequestedMinLightLevel = 0U;
	uint8_t u8RequestedOperation = 0U;
	uint8_t u8RequestedFlags = 0U;

	bool bExitLoop = false;

#ifdef MASTER_WITH_POT
	static uint8_t u8NewLevelCounter = 0U;
	uint8_t u8PotMinValue = 0U;
	uint8_t u8PotMaxValue = 0U;

	uint8_t u8SenseInput1 = 0U;


// Changing this - I dont 'really' need adc input - and its a pain to get working
// So Im just configuring 1 digital input - so I can do max values of 0% or 100%

		u8PotMaxValue = 0U;
    u8CubikControl_GPIO_Pin_GetDig(sensePin1, &u8SenseInput1);

		if(u8SenseInput1 == CA_PIN_VALUE_LOW)
		{
			u8PotMaxValue = 0U;
		}
		else
		{
			u8PotMaxValue = 100U;
		}


    u8PotMinValue = (u8PotMaxValue / 10);
//printf("Read pot:%d, u8PotMinValue:%d, u8MinLightLevel:%d, u8PotMaxValue:%d, u8MaxLightLevel:%d\n", u8PotValue, u8PotMinValue, u8MinLightLevel, u8PotMaxValue, u8MaxLightLevel);

		if((u8PotMinValue != u8MinLightLevel) || (u8PotMaxValue != u8MaxLightLevel))
		{
printf("Set for broadcast new light levels\n");
			// Pot values have changed - broadcast new values (so slaves can pick them up)
			u8MyCurrentState = CA_MSG_MASTER_COMMAND_SET_LEVELS;
			u8MinLightLevel = u8PotMinValue;
			u8MaxLightLevel = u8PotMaxValue;

// Give the slaves chance to pick this up
			u8NewLevelCounter = 10U;
		}
		else
		{
			u8MyCurrentState = CA_MY_STATUS_AWAKE;
		}

		if(u8NewLevelCounter > 0U)
		{
			u8NewLevelCounter--;
			u8MyCurrentState = CA_MSG_MASTER_COMMAND_SET_LEVELS;
		}
#endif

	// Default to lowest level, unless rules below tell us otherwise
	u8TargetDaliLevel = u8MinLightLevel;


	/* Dump out the list of detected devices */
#if 0
	printf("Dumping detected devices list\n");
	while(false == bExitLoop)
	{
		u8Result = u8CubikControl_GetDetectedNeighbourList(u8NumberOfDevices, 6, &aucDeviceId[0U]);
		if(CA_RESULT_GOOD == u8Result)
		{
			// Dump out the name returned - increment to next device
			u8NumberOfDevices++;
			printf("Device: %02d: %02X:%02X:%02X:%02X:%02X:%02X\n", u8NumberOfDevices, aucDeviceId[0U], aucDeviceId[1U], aucDeviceId[2U], aucDeviceId[3U], aucDeviceId[4U], aucDeviceId[5U]);
		}
		{
			bExitLoop = true;
		}
	}
#endif

	/* Check for received events - from neighbour devices */
	bExitLoop = false;
	while(false == bExitLoop)
	{
		/* Go and get an event message */
		u8OpCode = 0U;
		u8Result = u8CubikControl_RX_GetStatusMsg(&u8DeviceSlot, &u8OpCode, &u8RequestedMinLightLevel, &u8RequestedMaxLightLevel, &u8Flags, &u64MsgAge);

		if(CA_RESULT_GOOD == u8Result)
		{
printf("Got an event msg:u8DeviceSlot:%d, u8OpCode:%d, u8RequestedMinLightLevel:%d, u8RequestedMaxLightLevel:%d, u8Flags:%d, u64MsgAge:%lld\n",
			u8DeviceSlot, u8OpCode, u8RequestedMinLightLevel, u8RequestedMaxLightLevel, u8Flags, u64MsgAge);

			/* Got something, process it */
			switch(u8OpCode)
			{
				case CA_MY_STATUS_SLEEPING:
					// This neighbour is sleeping, nothing needed
					break;

				case CA_MY_STATUS_AWAKE:
					// This neighbour is awake, nothing needed
					break;

				case CA_MY_STATUS_TRIGGERED:
					// This neighbour has triggered, but is not asking me to do anything
					break;

				case CA_MY_STATUS_POST_TRIGGERED:
					// This neighbour has previously triggered, but is not asking me to do anything
					break;

				case CA_MY_STATUS_REQUEST_NEIGHBOUR_TRIGGER:
					// This neighbour has triggered, and is requesting me to trigger as well
					u8RequestedOperation = CA_MY_STATUS_REQUEST_NEIGHBOUR_TRIGGER;
					break;

				case CA_MSG_MASTER_COMMAND_SET_LEVELS:
					// I am being told to set my min and max levels
					u8RequestedFlags = u8Flags;
					u8RequestedOperation = CA_MSG_MASTER_COMMAND_SET_LEVELS;
					break;

				case CA_MSG_MASTER_COMMAND_SET_OPERATION:
					// I am being told to set my operational status (light off/min/max/full)
					u8RequestedFlags = u8Flags;
					u8RequestedOperation = CA_MSG_MASTER_COMMAND_SET_OPERATION;
					break;
					
#if 0
Remove these, operation above makes more sense
				case CA_CONTROLLER_SET_MIN_LIGHT_LEVEL:
					// I am being told to set my min light level
					u8RequestedMinLightLevel = u8MinValue;
					u8RequestedOperation = CA_CONTROLLER_SET_MIN_LIGHT_LEVEL;
					break;

				case CA_CONTROLLER_SET_MAX_LIGHT_LEVEL:
					// I am being told to set my max light level
					u8RequestedMaxLightLevel = u8MaxValue;
					u8RequestedOperation = CA_CONTROLLER_SET_MAX_LIGHT_LEVEL;
					break;
#endif

				default:
					// Unknown - ignore
					break;

			}
		}
		else
		{
			/* Nothing more or error - exit */
			bExitLoop = true;
		}
	}

	/* Default to doing nothing, look for reasons to change */
	u8MyNewState = u8MyCurrentState;


// TODO: check the functionality around this - both TX and RX
	/* Processed all the RX events, now we go through our rules and decide what we need to do */
	if(CA_MSG_MASTER_COMMAND_SET_OPERATION == u8RequestedOperation)
	{
		/* If the light levels are the same then its an absolute level */
		if(u8RequestedMinLightLevel == u8RequestedMaxLightLevel)
		{
			u8TargetDaliLevel = u8RequestedMinLightLevel;
printf("Line:%d, Master operation - set target level to %d\n", __LINE__, u8TargetDaliLevel);

			/* If the flag is RE_BROADCAST, then copy the values into our TX status message */
			if((u8RequestedFlags & CA_MSG_FLAG_REBROADCAST) == CA_MSG_FLAG_REBROADCAST)
			{
				u8MyNewState = CA_MSG_MASTER_COMMAND_SET_OPERATION;
			}
			else
			{
				u8MyNewState = CA_MY_STATUS_AWAKE;
			}
printf("Line:%d, New state:%d\n", __LINE__, u8MyNewState);
		}
	}
	else if(CA_MY_STATUS_REQUEST_NEIGHBOUR_TRIGGER == u8RequestedOperation)
	{
printf("Line:%d, Neighbour trigger, my current state:%d\n", __LINE__, u8MyCurrentState);

		/* A neighbour wants us to trigger, what we do depends on what we are currently doing */
		if((CA_MY_STATUS_SLEEPING == u8MyCurrentState) || (CA_MY_STATUS_AWAKE == u8MyCurrentState) || (CA_MY_STATUS_REQUEST_NEIGHBOUR_TRIGGER == u8MyCurrentState) || (CA_MY_STATUS_POST_TRIGGERED == u8MyCurrentState))
		{
			u64MyLastChangeTime = u64LoopCounter;
			u64PostTriggerTimer = TRIGGER_TIME;
		  bNeighbourTrigger = true;

printf("Line:%d, Neighbour trigger, my current state:%d, my new state:%d\n", __LINE__, u8MyCurrentState, u8MyNewState);
		}

		if(CA_MY_STATUS_TRIGGERED == u8MyCurrentState)
		{
			/* Im currently active, do nothing */
		}

		if(CA_MY_STATUS_POST_TRIGGERED == u8MyCurrentState)
		{
			/* Im currently active - timer winddown, do nothing */
		}
	}
	else if(CA_MSG_MASTER_COMMAND_SET_LEVELS == u8RequestedOperation)
	{
printf("Setting new levels to Min:%d Max:%d\n", u8RequestedMinLightLevel, u8RequestedMaxLightLevel);
		u8MinLightLevel = u8RequestedMinLightLevel;
		u8MaxLightLevel = u8RequestedMaxLightLevel;
	}

	/* Are we switching from off to triggered ? */
	if(true == bLocallyTriggered)
	{
		/* Set target level to max value and reset timer */
		u8TargetDaliLevel = u8MaxLightLevel;
//		u8MyNewState = CA_MY_STATUS_TRIGGERED;
		u8MyNewState = CA_MY_STATUS_REQUEST_NEIGHBOUR_TRIGGER;
		u64PostTriggerTimer = TRIGGER_TIME;
  	u64MyLastChangeTime = u64LoopCounter;
		bNeighbourTrigger = false;

printf("Line:%d, Local trigger, my current state:%d, my new state:%d\n", __LINE__, u8MyCurrentState, u8MyNewState);
	}
	else
	{
		/* Check for any post trigger timers to expire */
		if(u64PostTriggerTimer > 0)
		{
			/* This is after a trigger, so post trigger state - either by us or from neighbour request */
			u8TargetDaliLevel = u8MaxLightLevel;
			if(true == bNeighbourTrigger)
			{
				u8TargetDaliLevel = (u8MaxLightLevel / 10U) * 8U;
			}

			u64PostTriggerTimer--;
			u8MyNewState = CA_MY_STATUS_POST_TRIGGERED;		
	  	u64MyLastChangeTime = u64LoopCounter;

//printf("Line:%d, Post trigger, my current state:%d, my new state:%d, u64PostTriggerTimer:%lld\n", __LINE__, u8MyCurrentState, u8MyNewState, u64PostTriggerTimer);

			/* Has it expired */
			if(0U == u64PostTriggerTimer)
			{
				/* Set target light level to min level */
				u8TargetDaliLevel = u8MinLightLevel;
			  u8MyNewState = CA_MY_STATUS_AWAKE;
	    	u64MyLastChangeTime = u64LoopCounter;

printf("Line:%d, Post trigger expired, my current state:%d, my new state:%d\n", __LINE__, u8MyCurrentState, u8MyNewState);
			}
		}
	}

	/* Set the dali light to the target value */
  u8CubikControl_DaliLight_SetLevel(u8TargetDaliLevel);

// For debug only - if light level is anove min (idle), set onboard led on
if(u8TargetDaliLevel > u8MinLightLevel)
{
      u8CubikControl_GPIO_Pin_ValueSet(OnboardLedPin, CA_PIN_SET_ON);
}
else
{
      u8CubikControl_GPIO_Pin_ValueSet(OnboardLedPin, CA_PIN_SET_OFF);
}

	/* Set our broadcast message */
  u8CubikControl_BT_SetBroadcastStatus(u8MyNewState, u8MinLightLevel, u8MaxLightLevel);

	u64LoopCounter++;
	u8MyCurrentState = u8MyNewState;

	// Dummy, null the compiler warning
	u64MyLastChangeTime = u64MyLastChangeTime;
}


/* Initial entry point into project */
#ifdef BUILD_IN_ARDUINO
void setup()
{
  // Setup up the debug serial port  
  Serial.begin(115200);
  Serial.println("Setting up");

  // Nothing needed in here, everything done in main code
}
#endif

#ifdef BUILD_IN_ESPRESSIF
void Cubik_API_Test_Setup(void)
{
	printf("Setting up\n");
}
#endif


#ifdef BUILD_IN_ARDUINO
void loop()
{
  // Main loop - simply loop round operations, calling as needed

  // Call the test app 'thread'
  vMainTestAppThread();

  // Call the cubik api 'thread'
  vCubikControl_Priv_Main();    
}
#endif

/* Output version information at startup */
void vDumpVersionInformation(void)
{
	bool bVersionInfoFound = false;

	/* We are FILE01_ should always exist */
	printf("Extracting version information\n");

	/* NB: This will ONLY work for THIS file and header files */

#ifdef FILE01_FILENAME
	bVersionInfoFound = true;
	printf("File 01\n");
	printf("Name:%s\n", FILE01_FILENAME);
	printf("Version:%s\n", FILE01_VERSION);
	printf("Date:%s\n", FILE01_VERSION);
	printf("Description:%s\n", FILE01_DESCRIPTION);
	printf("\n");
#endif

#ifdef FILE02_FILENAME
	printf("File 02\n");
	printf("Name:%s\n", FILE02_FILENAME);
	printf("Version:%s\n", FILE02_VERSION);
	printf("Date:%s\n", FILE02_VERSION);
	printf("Description:%s\n", FILE02_DESCRIPTION);
	printf("\n");
#endif

#ifdef FILE03_FILENAME
	printf("File 03\n");
	printf("Name:%s\n", FILE03_FILENAME);
	printf("Version:%s\n", FILE03_VERSION);
	printf("Date:%s\n", FILE03_VERSION);
	printf("Description:%s\n", FILE03_DESCRIPTION);
	printf("\n");
#endif

	if(false == bVersionInfoFound)
	{
		printf("NO VERSION INFO FOUND\n");
	}
	else
	{
		printf("\n\n");
	}

}
//will blink a number times depending on input / 10; if input = 0 then will do one long blink
void blink(uint8_t lightLevel){
	//changes not uploaded yet
	uint8_t blinks = lightLevel / 10U;
	uint8_t time = 50;
	blinks = round(blinks);
	if (blinks == 0) {
		time = 100;
		blinks = 1;
	} 
	printf("blinking %u times \n", blinks);
	for(blinks; blinks != 0U; blinks--){
		u8CubikControl_GPIO_Pin_ValueSet(OnboardLedPin, CA_PIN_SET_ON);
		SMT_Cubik_delay_function(time);
		u8CubikControl_GPIO_Pin_ValueSet(OnboardLedPin, CA_PIN_SET_OFF);
		SMT_Cubik_delay_function(time);
	}
}

//breaks when value 100 is sent, only changes light at 70,80,90,100 anything lower just sets it to min, cant turn light off 
//variables on line 106 
void UART(void* parameters){
	data = (uint8_t *) malloc(uart_buffer_size);
	uart_driver_install(UART_NUM_0, uart_buffer_size, 0, 10, &uart_queue, 0);
    uart_param_config(uart_num, &uart_config);
    uart_set_pin(UART_NUM_0, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	while(1){
		int len = uart_read_bytes(UART_NUM_0, data, uart_buffer_size, 20 / portTICK_RATE_MS);
		if (len > 0 && *data != prevLightLevel){
			xSemaphoreTake(mutex, portMAX_DELAY);
			if(*data < 10U) newLightLevel = 0U;
			else newLightLevel = *data;
			blink(newLightLevel);
			activateClause = true;
			xSemaphoreGive(mutex);
		}
		memset(data, 0, uart_buffer_size);
	}
}

void mainSmartlightLoop(void* parameters){
	while(1){
		uint8_t val = 0;
		if(activateClause){
			xSemaphoreTake(mutex, portMAX_DELAY);
			val = (uint8_t)(((uint16_t)newLightLevel * 255U) / 100U);
			printf("unsigned value should be %u at %u percent \n", val, newLightLevel);
            u8CubikControl_DaliLight_SetLevel(newLightLevel); 
			activateClause = false;
			xSemaphoreGive(mutex);
        }
		
		SMT_Cubik_delay_function(500);
	}
}

#ifdef BUILD_IN_ESPRESSIF
void app_main(void)
{
  /* Delay for 4 seconds, give chance for debug monitor to catch up */
  SMT_Cubik_delay_function(4000);

	// Dump out the file version information
	vDumpVersionInformation();

	// Call our setup functionSetting
	Cubik_API_Test_Setup();

	// Do normal configuration and go into main detection/control loop
	// NB: Should really be checking retcodes
  u8CubikControl_LibInit();
  u8CubikControl_DaliLight_Init(CA_REQUEST_IFACE_ENABLE, 0U, 0U);
  u8CubikControl_DaliLight_SetLevel(0);

#ifdef MASTER_WITH_POT
  u8CubikControl_GPIO_Pin_Config(sensePin1, CA_PIN_CONFIG_INPUT);
#endif
  
  u8CubikControl_GPIO_Pin_Config(OnboardLedPin, CA_PIN_CONFIG_OUTPUT);
  u8CubikControl_GPIO_Pin_ValueSet(OnboardLedPin, CA_PIN_SET_ON);
  u8CubikControl_GPIO_Pin_Config(buttonPin, CA_PIN_CONFIG_INPUT);

	mutex = xSemaphoreCreateMutex();



    
	xTaskCreatePinnedToCore(
		UART,
		"checks incoming messages on UART",
		2048,
		NULL,
		1,
		NULL,
		0
	);


	xTaskCreatePinnedToCore(
		mainSmartlightLoop,
		"DALI activation Loop",
		5120,
		NULL,
		1,
		NULL,
		1
	);

	SMT_Cubik_delay_function(2000);
	u8CubikControl_GPIO_Pin_ValueSet(OnboardLedPin, CA_PIN_SET_OFF);
  
}
#endif





