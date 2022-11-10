
/* File Header
 *
 *  FileName:   SMT_Cubik_API_Implementation.cpp
 *  Purpose:    This file is the main implementation of the Cubik API library
 *               used by the SMT001 - Smart Street Light project - Cubik API interface library
 *  Created:    Initial creation by Graeme Jones, Principle Software Enginneer at Cubik Innovations
 *  When:       First draft written on 2020_11_20
 *  API Doc:    TN_SMT001_02_API_Issx.x.docx
 *
 */


/* File History
 *
 * GrJo - 2020_11_20 - Initial Creation
 *
 * GrJo - 2020_11_25 - Extensive rework:
 *                     Added build define entries to split between Arduino/Espressif build
 *                     Converted Dali class to direct C functions (stripped out unused)
 *										 Moved all Arduino specific functions into new locals, with build environment specific implementations
 *
 * GrJo - 2020_11_25 - adding iBeacon sender/receiver code into project (from iBeacon examples - maybe I need Eddystone ?)
 *
 * GrJo - 2020_12_07 - porting working inter-device BLE code from beacon_adv_and_scan testpad
 *                     NB: taking ble comms example code from:
 *                     https://github.com/lucadentella/esp32-tutorial/blob/master/28_ble_scan_response/main/main.c
 *                     https://github.com/espressif/esp-idf/blob/178b122/components/bt/host/bluedroid/api/include/api/esp_gap_ble_api.h
 * 
 * GrJo - 2020_12_10 - modded some of the Dali code - send OFF instead of 0 and ON instead off 255
 *                     changed dali transmit to send message twice
 *
 * GrJo - 2020_12_16 - think that sometimes the BT stack is swapping at the 'wrong' time, during a Dali transmit cycle
 *                     Disabling interrupts (and task swapping) during the Dali transmit caused an immediate barrf
 *                     Changing to set a 'dali tx pending' flag in the current transmit section, and have BT event callback
 *                     function process it.
 *                     Changed the 'master controller' to use a single pin - either 100% or 0% max level
 *
 * GrJo - 2020_12_17 - there are more ranges of ESP MAC address than I thought - removed this part of the validation
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

#define FILE03_FILENAME       "SMT_Cubik_API_Implementation.c"
#define FILE03_VERSION        "0.1"
#define FILE03_DATE           "2020_12_18"
#define FILE03_DESCRIPTION    "Main API function file - include in main application"


/* includes */
#include "SMT_Cubik_API_Data_Types.h"

/* Includes files needed for BLE operation:
 *  NB: These may not be the same when using the espressif ide
 */
#ifdef BUILD_IN_ARDUINO
	#include "BLEDevice.h"
	#include "BLEUtils.h"
	#include "BLEBeacon.h"
#endif

#ifdef BUILD_IN_ESPRESSIF

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"

#include "esp_efuse.h"
#include "esp_efuse_table.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"


// See what I need of these
#if 0
	#include <stdio.h>
	#include <string.h>
	#include <rom/ets_sys.h>
	#include "nvs_flash.h"

	#include "freertos/FreeRTOS.h"
	#include "freertos/task.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_log.h"
#endif

#endif


/* Constant Values */

/* Defined Values */
/* This is needed when using the mikroE adaptor board - and the SMT PCB */
#define PIN_INVERSION_NEEDED

/* Used in counter loop */
#define MAX_64BIT_REGISTER_VALUE 0xFFFFFFFF 

/* How many received events can we hold */
#define MAX_RX_EVENT_MESSAGES 10


/* Default Beacon tx frequency */
#define BLE_BEACON_TX_FREQUENCY 100



/* Macros */


/* Typedefs */

/* Local prototypes */
uint8_t u8CubikControl_Priv_GPIO_Pin_Validate(uint8_t u8PinNum);

void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void vCubikControl_Priv_setBeacon(void);
void vCubikControl_Priv_Beacon_Start(void);
void vCubikControl_Priv_Beacon_Stop(void);
void vCubikControl_Priv_Setup(void);
void vCubikControl_Priv_Main(void);
bool bCubikControl_Priv_already_discovered(esp_bd_addr_t address);
void vCubikControl_Priv_dump_device_list(void);
void vCubikControl_Priv_addDevice(esp_bd_addr_t address, uint8_t *paucName, uint8_t u8Len);
void vCubikControl_Priv_store_device_data(esp_bd_addr_t bdaddr, esp_bt_dev_type_t addr_type, int rssi, uint8_t rsp_len, uint8_t *data);
bool vCubikControl_Priv_is_device_an_smt(struct ble_scan_result_evt_param *data_struct);


void vCubikControl_Priv_Check_For_BetweenScans_Work(void);
void SMT_Cubik_Dali_transmit_Actual(uint8_t cmd1, uint8_t cmd2);


/* Local variables */

/* Tables of RX event message data */
static uint8_t Cubik_Priv_aucRXEventDeviceId[MAX_RX_EVENT_MESSAGES];
static uint8_t Cubik_Priv_aucRXEventOpCode[MAX_RX_EVENT_MESSAGES];
static uint8_t Cubik_Priv_aucRXEventDataValue1[MAX_RX_EVENT_MESSAGES];
static uint8_t Cubik_Priv_aucRXEventDataValue2[MAX_RX_EVENT_MESSAGES];
static uint8_t Cubik_Priv_aucRXEventDataValue3[MAX_RX_EVENT_MESSAGES];
static uint64_t Cubik_Priv_au64RXEventMsgTimestamp[MAX_RX_EVENT_MESSAGES];
static uint8_t Cubik_Priv_u8NumberOfRxEvents;


/* GrJo - 2020_11_24 - I had created a local struct to hold all variables.
											 This worked perfectly in Arduino but Espressif compiler refuses to see variables in this struct
											 So Ive moved all variables to static locals (not happy about this, the struct method should work !) */

uint8_t Cubik_Priv_aucMyMAC[6]; /* Store for our own MAC address */

static bool Cubik_Priv_bLibActive = FALSE; /* Is our library initialised */
static bool Cubik_Priv_bDaliStackActive = FALSE; /* Is the Dali stack active */
static uint8_t Cubik_Priv_u8DaliTxPin = 0U; /* The currently allocated DALI TX pin */
static uint8_t Cubik_Priv_u8DaliRxPin = 0U; /* The currently allocated DALI RX pin */

static bool Cubik_Priv_bBTStackActive = FALSE; /* Is the BT stack active */
static bool Cubik_Priv_bBTBroadcastActive = FALSE; /* Is the BT broadcasting active */

static uint8_t Cubik_Priv_u8MyTriggeredStatus = 0U; /* What is my current status */
static uint8_t Cubik_Priv_u8MyMaxLightLevel = 0U; /* What is my maximum dali level */
static uint8_t Cubik_Priv_u8MyCurrentLightLevel = 0U; /* What is my current dali level */

/* Frequency that we send beacon messages at - set to defined default frequency */
static uint16_t Cubik_Priv_u16BeaconTxFrequency = BLE_BEACON_TX_FREQUENCY;


static uint8_t Cubik_Priv_speedFactor; /* Used in the Dali timing calculation */
static uint16_t Cubik_Priv_delay1; /* Used in the Dali timing calculation */
static uint16_t Cubik_Priv_delay2; /* Used in the Dali timing calculation */
static uint16_t Cubik_Priv_period; /* Used in the Dali timing calculation */

static gpio_config_t io_conf; /* Needed for use in gpio configuration - MUST be persistent */

static esp_bd_addr_t Cubik_Priv_Discovered_Devices[MAX_DISCOVERED_DEVICES];
static uint8_t Cubik_Priv_aucDiscovered_Names[MAX_DISCOVERED_DEVICES][20];
static uint8_t Cubik_Priv_Discovered_Devices_Num = 0;
static int Cubik_Priv_iRSSI[MAX_DISCOVERED_DEVICES];
static int Cubik_Priv_EventType[MAX_DISCOVERED_DEVICES];
static int Cubik_Priv_MinLight[MAX_DISCOVERED_DEVICES];
static int Cubik_Priv_MaxLight[MAX_DISCOVERED_DEVICES];
static int Cubik_Priv_Flags[MAX_DISCOVERED_DEVICES];
static uint64_t Cubik_Priv_u64TimeStamp[MAX_DISCOVERED_DEVICES];

static bool Cubik_Priv_adv_data_set = false;
static bool Cubik_Priv_scan_rsp_set = false;
static bool Cubik_Priv_scanning_triggered = false;
static uint64_t Cubik_Priv_u64ScanCounter = 0U;
static uint64_t Cubik_Priv_u64LoopCounter = 0U;

static bool Cubik_Priv_bDaliDataToSend = false;
static uint8_t Cubik_Priv_u8DaliTxByte1 = 0U;
static uint8_t Cubik_Priv_u8DaliTxByte2 = 0U;


// GrJo - 2020_12_17 - cant use this, there are more address ranges than I thought
#if 0
// Validate that this is a valid SMT device
// MAC address is of form:
// AC 67 B2 ... for WROOM devices
// 24 B2 AB ... for CAM32 devices
// Name is SMT:__:__:__:__
static uint8_t u8MacPattern1[3U] = { 0xAC, 0x67, 0xB2 };
static uint8_t u8MacPattern2[3U] = { 0x24, 0x62, 0xAB };
#endif


/* Debug flag, see if the BT events are kicking in during a Dali transmit */
bool bDaliTXInProgress = false;



// This is my modded version - its says SMT:01:02:03:04 - NB: 17 bytes of data sent (NULL for sprintf)
static uint8_t adv_raw_data[18] = {0x10, 0x09, 'S', 'M', 'T', ':', '0', '1', ':', '0', '2', ':', '0', '3', ':', '0', '4', 0x00};

// NB: Limited on length - max packet size is 31 bytes, so 8 in here is okay
static uint8_t scan_rsp_raw_data[8] = {0x07, 0xFF, 'S', ':', 0x01, 0x02, 0x03, 0x04};

// scan parameters
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};


static esp_ble_adv_params_t ble_adv_params = {

// These are 800ms between 'adverts' - 1280*0.625 = 800 (NB: The rfConnect doesnt report gaps >999ms)
  .adv_int_min = 1280,
  .adv_int_max = 1280,

//  .adv_type = ADV_SCAN_IND,
//  .adv_type = ADV_TYPE_NONCONN_IND, // With THIS the rfConnect DOESNT see the extended company ID data
  .adv_type = ADV_TYPE_IND,           // With THIS it does see the data - go figure ?

  .own_addr_type  = BLE_ADDR_TYPE_PUBLIC,
  .channel_map = ADV_CHNL_ALL,
  .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// This struct is what gets sent out initially
static esp_ble_adv_data_t adv_data = {

  .include_name = true,
  .include_txpower = true,
  .flag = ESP_BLE_ADV_FLAG_LIMIT_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT,

// Advertising myself as Streetlight
// https://specificationrefs.bluetooth.com/assigned-values/Appearance%20Values.pdf
  .appearance = (1408 | 17),
};

// This is Espressif ID, the rfConnect Android app shows this - but I override it with my custom data
static uint8_t manufacturer_data[6] = {0xE5,0x01,0x02,0x03,0x04,0x05};
static esp_ble_adv_data_t scan_rsp_data = 
{
  .set_scan_rsp = true,
  .manufacturer_len = 6,
  .p_manufacturer_data = manufacturer_data,
  .appearance = (1408 | 17), // Adding this doesnt change the icon in the nRF app
};


#ifdef BUILD_IN_ARDUINO
BLEBeacon oBeacon; /* Beacon instance */
BLEAdvertising *pAdvertising; /* Beacon advertising pointer */
#endif


/* Public Functions - can be called by other library */


/* Creating local functions here - Arduino versions and Espressif versions */
#ifdef BUILD_IN_ARDUINO
	void SMT_Cubik_delay_function(uint16_t u16MSToDelay)
	{
		delay(u16MSToDelay);
	}

	void SMT_Cubik_delayMicro_function(uint16_t u16MSToDelay)
	{
		delayMicroseconds(u16MSToDelay);
	}

	void SMT_Cubik_pinmode(uint8_t u8PinNum, uint8_t u8Mode)
	{
		pinMode(u8PinNum, u8Mode);
	}

	void SMT_Cubik_digitalWrite(uint8_t u8PinNum, uint8_t u8Value)
	{
		digitalWrite(u8PinNum, u8Value);
	}

	uint8_t SMT_Cubik_digitalRead(uint8_t u8PinNum)
	{
		return digitalRead(u8PinNum);
	}

	uint16_t SMT_Cubik_analogRead(uint8_t u8PinNum)
	{
		return analogRead(u8PinNum);
	}
#endif

#ifdef BUILD_IN_ESPRESSIF
	void SMT_Cubik_delay_function(uint16_t u16MSToDelay)
	{
		vTaskDelay(u16MSToDelay / portTICK_PERIOD_MS);
	}

	void SMT_Cubik_delayMicro_function(uint16_t u16MSToDelay)
	{
		ets_delay_us(u16MSToDelay);
	}

	void SMT_Cubik_pinmode(uint8_t u8PinNum, uint8_t u8Mode)
	{
//    gpio_config_t io_conf;
    esp_err_t eRetVal = 0U;

    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;

		// Set the modes
		if(u8Mode == OUTPUT)
		{
    	//set as output mode
	    io_conf.mode = GPIO_MODE_OUTPUT;
		}

		if(u8Mode == INPUT)
		{
    	//set as input mode
	    io_conf.mode = GPIO_MODE_INPUT;

	    //disable pull-up mode
			io_conf.pull_up_en = 0;
		}

		if(u8Mode == INPUT_PULLUP)
		{
    	//set as input mode
	    io_conf.mode = GPIO_MODE_INPUT;

	    //enable pull-up mode
			io_conf.pull_up_en = 1;
		}

    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1U << u8PinNum);

    //disable pull-down mode
    io_conf.pull_down_en = 0;

    //configure GPIO with the given settings

// NOTE: this call doesnt work unless you pull the retcode !!!
		eRetVal = gpio_config(&io_conf);
		eRetVal = eRetVal; /* Consume to avoid compiler warning */
	}

	void SMT_Cubik_digitalWrite(uint8_t u8PinNum, uint8_t u8Mode)
	{
		if(u8Mode == LOW)
		{
			gpio_set_level(u8PinNum, 0U);
		}
		if(u8Mode == HIGH)
		{
			gpio_set_level(u8PinNum, 1U);
		}
	}

	uint8_t SMT_Cubik_digitalRead(uint8_t u8PinNum)
	{
		uint8_t u8RetValue = 0U;
		int iGetVal = 0U;

		iGetVal = gpio_get_level(u8PinNum);

		if(iGetVal == 0U)
		{
			u8RetValue = LOW;
		}
		else
		{
			u8RetValue = HIGH;
		}

		return u8RetValue;
	}

	uint16_t SMT_Cubik_analogRead(uint8_t u8PinNum)
	{

// TODO: implement this, its more complex than in Arduino

		return 4095U; 
	}
#endif


/*
 * Main initialisation function for Cubik library
 * MUST be called before any other Cubik library functions are called
 *
 * Inputs:  none
 *
 * Outputs: none
 *
 * Return Values - CA_RESULT_GOOD               - library init succesful, thread started
 *                 CA_RESULT_UNSPECIFIED_ERROR  - library init failed, thread not started
 *
 * Actions: Start cubik library thread, initialise default values
 *
*/
uint8_t u8CubikControl_LibInit(void)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

	// Pull our own MAC address  
	esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &Cubik_Priv_aucMyMAC, sizeof(Cubik_Priv_aucMyMAC) * 8);
  printf("1. read MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", Cubik_Priv_aucMyMAC[0], Cubik_Priv_aucMyMAC[1], Cubik_Priv_aucMyMAC[2], Cubik_Priv_aucMyMAC[3], Cubik_Priv_aucMyMAC[4], Cubik_Priv_aucMyMAC[5]);

  /* Call setup function - enable periodic processing */
  vCubikControl_Priv_Setup();

  return u8RetCode;
}


/*
 * Get the ID of this device
 *
 * Inputs:  uint8_t u8MaxLength    - length of receive buffer
 *          uint8_t *pu8Length     - pointer to receive number of bytes returned
 *          uint8_t *pu8DataBuffer - pointer to buffer, to receive bytes
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR – one (or more) of the parameters is invalid
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Copy device ID into supplied buffer
 *
*/
uint8_t u8CubikControl_GetMyDeviceID(uint8_t u8MaxLength, uint8_t *pu8Length, uint8_t *pu8DataBuffer)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }
  else
  {
		// Copy it over
		if((pu8DataBuffer != NULL) && (pu8Length != NULL) && (u8MaxLength >= 6U))
		{
			memcpy(pu8DataBuffer, Cubik_Priv_aucMyMAC, 6U);
			*pu8Length = 6U;
		}
		else
		{
			u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
		}
	}

  return u8RetCode;
}


/*
 * Initialise/De-Initialise the DALI interface driver
 *
 * Inputs: uint8_t u8Opcode  - CA_REQUEST_IFACE_ENABLE (for init)
 *                             CA_REQUEST_IFACE_DISABLE (for de-init)
 *         uint8_t u8TXPin    - GPIO pin to transmit on
 *         uint8_t u8RXPin    - GPIO pin to receive on
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR – one (or more) of the parameters is invalid
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Initialise the DALI bus (and any controller on it, broadcast message to set all lights to OFF)
 *           De-initialise DALI bus (no message sent to DALI bus)
 *
*/
uint8_t u8CubikControl_DaliLight_Init(uint8_t u8Opcode, uint8_t u8TXPin, uint8_t u8RXPin)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;
  uint8_t u8TempValue = CA_RESULT_GOOD;

  /* Validate parameters */
  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }
  else
  {
    /* Get the pins */
    if(0U == u8TXPin)
    {
      Cubik_Priv_u8DaliTxPin = CUBIK_DALI_INTERFACE_TX_PIN;
    }
    else
    {
      u8TempValue = u8CubikControl_Priv_GPIO_Pin_Validate(u8TXPin);
      if(CA_RESULT_GOOD != u8TempValue)
      {
        u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
      }
      else
      {
        Cubik_Priv_u8DaliTxPin = u8TXPin;
      }
    }

    /* Get the pins */
    if(0U == u8RXPin)
    {
      Cubik_Priv_u8DaliRxPin = CUBIK_DALI_INTERFACE_RX_PIN;
    }
    else
    {
      u8TempValue = u8CubikControl_Priv_GPIO_Pin_Validate(u8RXPin);
      if(CA_RESULT_GOOD != u8TempValue)
      {
        u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
      }
      else
      {
        Cubik_Priv_u8DaliRxPin = u8TXPin;
      }
    }
  }

  if(CA_RESULT_GOOD == u8RetCode)
  {  
    /* Setup DALI interface - and turn off light */
    SMT_Cubik_Dali_setupTransmit(Cubik_Priv_u8DaliTxPin, Cubik_Priv_u8DaliRxPin);
    SMT_Cubik_Dali_initialisation();
    SMT_Cubik_Dali_transmit(BROADCAST_C, OFF_C);    

    Cubik_Priv_bDaliStackActive = TRUE;
  }

  return u8RetCode;   
}


/*
 * Set the DALI light to the given %age brightness
 *
 * Inputs: uint8_t u8Level   - a %age value (range 0% to 100%)
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR – one (or more) of the parameters is invalid
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Broadcast a message on the DALI bus, to set the light to the specified value
 *
*/
uint8_t u8CubikControl_DaliLight_SetLevel(uint8_t u8Level)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;
  uint8_t u8OutputValue = 0U;
  static uint8_t u8PrevLevel = 0U;

  /* Is the library active ? */
  if((FALSE == Cubik_Priv_bLibActive) || (FALSE == Cubik_Priv_bDaliStackActive))
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }
  else
  {
    /* Validate the level is a %age */
    if(100 < u8Level)
    {
      u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
    }
    else
    {
      u8OutputValue = (uint8_t)(((uint16_t)u8Level * 255U) / 100U);
//      if(u8OutputValue > 254U)
//      {
//        u8OutputValue = 254U;
//      }

      /* Dont send command unless its different to previous one */
      if(u8PrevLevel != u8OutputValue)
      {
				/* Light doesnt seem to respond to values below 10% (25), send OFF command instead */
				if(25U > u8OutputValue)
				{
			    SMT_Cubik_Dali_transmit(BROADCAST_C, OFF_C);
				}
				/* Light doesnt seem to respond to a specific value of 255, send ON command instead - for all values above 250U */
				else if(250U < u8OutputValue)
				{
			    SMT_Cubik_Dali_transmit(BROADCAST_C, ON_C);
				}
				else
				{
	        SMT_Cubik_Dali_transmit(BROADCAST_DP, u8OutputValue);
				}
        u8PrevLevel = u8OutputValue;
      }
    }
  }

  return u8RetCode;
}


/*
 * Request the status from the contoller on the DALI bus
 *
 * Inputs: none
 *
 * Return Values - CA_RESULT_LIB_RESULT_PENDING – this is the correct response, it means the request is queued for operation
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Send a message on the DALI bus, request the current device status
 *
*/
uint8_t u8CubikControl_DaliLight_QueryStatus(void)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

// TODO: Implement this function

  return u8RetCode;
}


/*
 * Configure a GPIO pin as input or output
 *
 * Inputs: uint8_t u8PinNum - the GPIO pin number
 *         uint8_t u8Opcode - CA_PIN_CONFIG_OUTPUT – configure the given pin as output
 *                            CA_PIN_CONFIG_INPUT – configure the given pin as input
 *                            CA_PIN_CONFIG_ANALOG_INPUT - configure the given pin as analog input
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Configure the specified GPIO pin for the specified mode
 *          Input will be set with weak pullup, Output will be set to off
 *
*/
uint8_t u8CubikControl_GPIO_Pin_Config(uint8_t u8PinNum, uint8_t u8OpCode)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }

  /* Is it a valid pin ? */
  if(CA_RESULT_GOOD != u8CubikControl_Priv_GPIO_Pin_Validate(u8PinNum))
  {
    u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
  }

  if(CA_RESULT_GOOD == u8RetCode)
  {  
    // Is it input mode
    if(CA_PIN_CONFIG_INPUT == u8OpCode)
    {
      SMT_Cubik_pinmode(u8PinNum, INPUT_PULLUP);
    }
    else if(CA_PIN_CONFIG_ANALOG_INPUT == u8OpCode)
    {
      SMT_Cubik_pinmode(u8PinNum, INPUT);      
    }
    else if(CA_PIN_CONFIG_OUTPUT == u8OpCode)
    {
      SMT_Cubik_pinmode(u8PinNum, OUTPUT);
      SMT_Cubik_digitalWrite(u8PinNum, LOW);
    }
    else
    {
      u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
    }
  }

  return u8RetCode;
}

/*
 * Set a HIGH/LOW value on a GPIO output pin
 *
 * Inputs: uint8_t u8PinNum - the GPIO pin number
 *         uint8_t u8Opcode - CA_PIN_SET_ON – set the given pin to ON
 *                            CA_PIN_SET_OFF – set the given pin to OFF
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Configure the specified GPIO pin for the specified mode
 *          Input will be set with weak pullup, Output will be set to off
 *
*/
uint8_t u8CubikControl_GPIO_Pin_ValueSet(uint8_t u8PinNum, uint8_t u8Value)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }

  /* Is it a valid pin ? */
  if(CA_RESULT_GOOD != u8CubikControl_Priv_GPIO_Pin_Validate(u8PinNum))
  {
    u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
  }

  if(CA_RESULT_GOOD == u8RetCode)
  {  
    // Is it input mode
    if(CA_PIN_SET_ON == u8Value)
    {
      SMT_Cubik_digitalWrite(u8PinNum, HIGH);
    }
    else if(CA_PIN_SET_OFF == u8Value)
    {
      SMT_Cubik_digitalWrite(u8PinNum, LOW);
    }
    else
    {
      u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
    }
  }

  return u8RetCode;
}


/*
 * Get the digital level of the specified GPIO pin
 *
 * Inputs: uint8_t u8PinNum - the GPIO pin number
 *         uint8_t *pu8RetValue - pointer to byte
 *
 * Output: Puts values CA_PIN_VALUE_LOW or CA_PIN_VALUE_HIGH into the byte location
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Read the digital level of the specified GPIO pin, populate into the supplied pointer byte location
 *
*/
uint8_t u8CubikControl_GPIO_Pin_GetDig(uint8_t u8PinNum, uint8_t *pu8RetValue)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }

  /* Is it a valid pin ? */
  if(CA_RESULT_GOOD != u8CubikControl_Priv_GPIO_Pin_Validate(u8PinNum))
  {
    u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
  }

  /* Is it a non-NULL pointer */
  if(NULL == pu8RetValue)
  {
    u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
  }
  
  if(CA_RESULT_GOOD == u8RetCode)
  {  
    // Get the digital value
    if(HIGH == SMT_Cubik_digitalRead(u8PinNum))
    {
      *pu8RetValue = CA_PIN_VALUE_HIGH;
    }
    else
    {
      *pu8RetValue = CA_PIN_VALUE_LOW;
    }
  }

  return u8RetCode;
}

/*
 * Get the analog level of the specified GPIO pin
 *
 * Inputs: uint8_t u8PinNum - the GPIO pin number
 *         uint8_t *pu8RetValue - pointer to byte
 *
 * Output: Puts value of 0-255 into the byte location
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Read the analog level of the specified GPIO pin, populate into the supplied pointer byte location
 *
*/
uint8_t u8CubikControl_GPIO_Pin_GetAna(uint8_t u8PinNum, uint8_t *pu8RetValue)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }

  /* Is it a valid pin ? */
  if(CA_RESULT_GOOD != u8CubikControl_Priv_GPIO_Pin_Validate(u8PinNum))
  {
    u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
  }

  /* Is it a non-NULL pointer */
  if(NULL == pu8RetValue)
  {
    u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
  }
  
  if(CA_RESULT_GOOD == u8RetCode)
  {  
    // Copy the analog value - divide from 4096 to 256 range
    *pu8RetValue = (SMT_Cubik_analogRead(u8PinNum) / 16U);
  }

  return u8RetCode;
}


/*
 * Enable/Disable the Bluetooth driver stack
 *
 * Inputs: uint8_t u8Operation - CA_STATUS_ENABLE  - enable the bluetooth interface
 *                               CA_STATUS_DISABLE - disable the bluetooth interface
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Enable/disable the BT module (save power when disabled)
 *
*/
uint8_t u8CubikControl_BT_SetDeviceEnable(uint8_t u8Operation)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }
  else
  {
    /* Is it the enable function */
    if(CA_STATUS_ENABLE == u8Operation)
    {
        /* Yes, call bluetooth device enable function */

 // Is this function needed at all ?

      /* Default to broadcast disable */
      u8RetCode = u8CubikControl_BT_SetBroadcastEnable(CA_STATUS_DISABLE);

      /* Set the internal flag */
      Cubik_Priv_bBTStackActive = TRUE;
    }
    /* Is it the disable function */
    else if(CA_STATUS_DISABLE == u8Operation)
    {
      /* Yes, call bluetooth broadcast disable function */
      u8RetCode = u8CubikControl_BT_SetBroadcastEnable(CA_STATUS_DISABLE);

      /* Set the internal flag */
      Cubik_Priv_bBTStackActive = FALSE;
    }
    /* This is an error */
    else
    {
      u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
    }
  }

  return u8RetCode;
}


/*
 * Enable/Disable the Bluetooth broadcasting mode
 *
 * Inputs: uint8_t u8Operation - CA_STATUS_ENABLE  - enable the bluetooth interface
 *                               CA_STATUS_DISABLE - disable the bluetooth interface
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Turns on/off the BT message broadcasting mode
 *
*/
uint8_t u8CubikControl_BT_SetBroadcastEnable(uint8_t u8Operation)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }
  else
  {
    /* Is it the enable function */
    if(CA_STATUS_ENABLE == u8Operation)
    {
      /* Start advertising */
#ifdef BUILD_IN_ARDUINO
      pAdvertising->start();
#endif

#ifdef BUILD_IN_ESPRESSIF
			vCubikControl_Priv_Beacon_Start();
#endif
      
      /* Set the internal flag */
      Cubik_Priv_bBTBroadcastActive = TRUE;
    }
    /* Is it the disable function */
    else if(CA_STATUS_DISABLE == u8Operation)
    {
      /* Clear the flag */
      Cubik_Priv_bBTBroadcastActive = FALSE;

      /* Stop advertising */
#ifdef BUILD_IN_ARDUINO
      pAdvertising->stop();
#endif

#ifdef BUILD_IN_ESPRESSIF
			vCubikControl_Priv_Beacon_Stop();
#endif
    }   
    /* This is an error */
    else
    {
      u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
    }
  }

  return u8RetCode;  
}


/*
 * Sets the current BT message broadcast mode (frequency and content)
 *
 * Inputs: uint8_t u8MyTriggeredStatus - CA_MY_STATUS_SLEEPING – I am in a low power mode, the light is off. This is the normal daytime operation.
 *                                                               The broadcast frequency will be set to a very low rate (e.g. once per every 10 minutes).
 *                                       CA_MY_STATUS_AWAKE – I am awake and performing active detection. The light is currently in the low power level and I have not triggered.
 *                                       CA_MY_STATUS_TRIGGERED – I am awake, the light is now in the high power state. I have triggered, but I DONT require my neighbours to also trigger.
 *                                       CA_MY_STATUS_REQUEST_NEIGHBOUR_TRIGGER – Same as TRIGGERED, but I request my neighbours to also trigger themselves
 *                                       CA_MY_STATUS_POST_TRIGGERED – The post TRIGGERED, timer state – the light is still in high power mode but 
 *                                                                     will be returned to low power after a timeout. My neighbours should also return to a low power state
 *                                                                     (unless they have self triggered or are receiving a trigger request).
 *         uint8_t u8MyMaxLightLevel - the maximum light level I am allowed to set
 *         uint8_t u8MyCurrentLightLevel - my current light level
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Sets the message broadcast frequency and content
 *
*/
uint8_t u8CubikControl_BT_SetBroadcastStatus(uint8_t u8MyTriggeredStatus, uint8_t u8MyMaxLightLevel, uint8_t u8MyCurrentLightLevel)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }
  else
  {
		/* Only do this if something has changed */
		if((u8MyTriggeredStatus != Cubik_Priv_u8MyTriggeredStatus) ||
       (u8MyMaxLightLevel != Cubik_Priv_u8MyMaxLightLevel) ||
       (u8MyCurrentLightLevel != Cubik_Priv_u8MyCurrentLightLevel))
		{
			/* Store the new data in the global struct */
			Cubik_Priv_u8MyTriggeredStatus = u8MyTriggeredStatus;
			Cubik_Priv_u8MyMaxLightLevel = u8MyMaxLightLevel;
	    Cubik_Priv_u8MyCurrentLightLevel = u8MyCurrentLightLevel;

  	  /* Set this figure based on the u8MyTriggeredStatus */
// TODO: do this based on a table    
	    Cubik_Priv_u16BeaconTxFrequency = (uint16_t)BLE_BEACON_TX_FREQUENCY;

	    /* Recall the setBeacon function, to pick up the new values - if broadcasting is enabled */
			if(TRUE == Cubik_Priv_bBTBroadcastActive)
			{
		    vCubikControl_Priv_setBeacon();
			}
		}
  }

  return u8RetCode;
}


/*
 * Get the neighbour list
 *
 * Inputs: uint8_t u8DeviceIdSlot - Which device id to get - 0-255 neighbour devices
 *         uint8_t u8MaxDeviceIdLen - The size (in bytes) of the receive buffer
 *         uint8_t *pau8DeviceId - pointer to the receive buffer
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_NO_VALUE_STORED - the specified id slot does not contain a detected device
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Returns the device_id from the specified slot (if a device has been detected)
 * NB: Device ID is ALWAYS 6 bytes - its the MAC address
 *
*/
uint8_t u8CubikControl_GetDetectedNeighbourList(uint8_t u8DeviceIdSlot, uint8_t u8MaxDeviceIdLen, uint8_t *pau8DeviceId)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }
  else
  {
//printf("u8CubikControl_GetDetectedNeighbourList(), u8DeviceIdSlot:%d, Cubik_Priv_Discovered_Devices_Num:%d\n", u8DeviceIdSlot, Cubik_Priv_Discovered_Devices_Num);
		if(u8DeviceIdSlot < Cubik_Priv_Discovered_Devices_Num)
		{
			// Copy over ID from this slot
			memcpy(pau8DeviceId, Cubik_Priv_Discovered_Devices[u8DeviceIdSlot], 6U);
		}
		else
		{
			u8RetCode = CA_RESULT_NO_VALUE_STORED;
		}
	}

  return u8RetCode;
}


/*
 * Get the oldest 'Receive' message - a status or command from another unit
 *
 * Inputs: uint8_t *pu8DeviceIdSlot - pointer to byte, will be populated with ID table number of the device which sent this message
 *         uint8_t *pu8OpCode - pointer to byte, will be populated with the message code received (see table in API document)
 *         uint8_t *pu8DataValue1 - pointer to byte, will be populated with the 1st data value (if applicable)
 *         uint8_t *pu8DataValue2 - pointer to byte, will be populated with the 2nd data value (if applicable)
 *         uint8_t *pu8DataValue3 - pointer to byte, will be populated with the 2nd data value (if applicable)
 *         uint64_t *pu64MsgAge - pointer to byte, will be populated with the 'age' of the message (in seconds)
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_NO_VALUE_STORED - there are no received messages in the rx queue
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Returns the device_id from the specified slot (if a device has been detected)
 *
*/
uint8_t u8CubikControl_RX_GetStatusMsg(uint8_t *pu8DeviceIdSlot, uint8_t *pu8OpCode, uint8_t *pu8DataValue1, uint8_t *pu8DataValue2, uint8_t *pu8DataValue3, uint64_t *pu64MsgAge)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;
	uint8_t u8Loop = 0U;

  /* Is the library active ? */
  if(FALSE == Cubik_Priv_bLibActive)
  {
    u8RetCode = CA_RESULT_LIB_NOT_ACTIVE;
  }
  else if((NULL == pu8DeviceIdSlot) || (NULL == pu8OpCode) || (NULL == pu8DataValue1) || (NULL == pu8DataValue2) || (NULL == pu8DataValue3) || (NULL == pu64MsgAge))
  {
		u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
	}
	else
	{
		// Return the event message in slot 0
		if(Cubik_Priv_u8NumberOfRxEvents > 0)
		{
			/* Move the message into the caller supplied buffers */
			*pu8DeviceIdSlot = Cubik_Priv_aucRXEventDeviceId[0];
			*pu8OpCode = Cubik_Priv_aucRXEventOpCode[0];
			*pu8DataValue1 = Cubik_Priv_aucRXEventDataValue1[0];
			*pu8DataValue2 = Cubik_Priv_aucRXEventDataValue2[0];
			*pu8DataValue3 = Cubik_Priv_aucRXEventDataValue3[0];
			*pu64MsgAge = (Cubik_Priv_u64LoopCounter - Cubik_Priv_au64RXEventMsgTimestamp[0]);

			/* Move all remaining entries down */
			for(u8Loop = 0U; u8Loop < (Cubik_Priv_u8NumberOfRxEvents - 1U); u8Loop++)
			{
				Cubik_Priv_aucRXEventDeviceId[u8Loop] = Cubik_Priv_aucRXEventDeviceId[u8Loop + 1U];
				Cubik_Priv_aucRXEventOpCode[u8Loop] = Cubik_Priv_aucRXEventOpCode[u8Loop + 1U];
				Cubik_Priv_aucRXEventDataValue1[u8Loop] = Cubik_Priv_aucRXEventDataValue1[u8Loop + 1U];
				Cubik_Priv_aucRXEventDataValue2[u8Loop] = Cubik_Priv_aucRXEventDataValue2[u8Loop + 1U];
				Cubik_Priv_aucRXEventDataValue3[u8Loop] = Cubik_Priv_aucRXEventDataValue3[u8Loop + 1U];
				Cubik_Priv_au64RXEventMsgTimestamp[u8Loop] = Cubik_Priv_au64RXEventMsgTimestamp[u8Loop + 1U];
			}

			/* Decrement count */
			Cubik_Priv_u8NumberOfRxEvents--;
		}
		else
		{
			u8RetCode = CA_RESULT_NO_VALUE_STORED;
		}
	}

  return u8RetCode;
}



/* Private Functions - internal use only */

/*
 * The private setup function, defaults variables, etc
 *
 * Inputs: none
 *
 * Return: none
 *
 * Actions: Performs API initialisation
 *
 * This function is called by the API init function
*/
void vCubikControl_Priv_Setup(void)
{

// TODO: anything we need to do in here

  /* Default all our flags */
  Cubik_Priv_bLibActive = TRUE;
  Cubik_Priv_bBTStackActive = FALSE;
  Cubik_Priv_bBTBroadcastActive = FALSE;
  Cubik_Priv_bDaliStackActive = FALSE;
  Cubik_Priv_u8MyTriggeredStatus = CA_MY_STATUS_SLEEPING;
  Cubik_Priv_u8MyMaxLightLevel = 0U;
  Cubik_Priv_u8MyCurrentLightLevel = 0U;

// NB: There is currently no Arduino code to populate these
	Cubik_Priv_Discovered_Devices_Num = 0;
	memset(&Cubik_Priv_Discovered_Devices, 0U, sizeof(Cubik_Priv_Discovered_Devices));
	memset(&Cubik_Priv_aucDiscovered_Names, 0U, sizeof(Cubik_Priv_aucDiscovered_Names));
	memset(&Cubik_Priv_iRSSI, 0U, sizeof(Cubik_Priv_iRSSI));
	memset(&Cubik_Priv_EventType, 0U, sizeof(Cubik_Priv_EventType));
	memset(&Cubik_Priv_MinLight, 0U, sizeof(Cubik_Priv_MinLight));
	memset(&Cubik_Priv_MaxLight, 0U, sizeof(Cubik_Priv_MaxLight));
	memset(&Cubik_Priv_Flags, 0U, sizeof(Cubik_Priv_Flags));
	memset(&Cubik_Priv_u64TimeStamp, 0U, sizeof(Cubik_Priv_u64TimeStamp));

	Cubik_Priv_adv_data_set = false;
	Cubik_Priv_scan_rsp_set = false;
	Cubik_Priv_scanning_triggered = false;
	Cubik_Priv_u64ScanCounter = 0U;
	Cubik_Priv_u64LoopCounter = 0U;

	Cubik_Priv_bDaliDataToSend = false;
	Cubik_Priv_u8DaliTxByte1 = 0U;
	Cubik_Priv_u8DaliTxByte2 = 0U;


#ifdef BUILD_IN_ARDUINO
  /* Create the BT device objects */
  BLEDevice::init("SmartStreetLight Control");

  /* Set up the advertising stack */
  pAdvertising = BLEDevice::getAdvertising();

  /* Enable the beacon - with default data */
  oBeacon = BLEBeacon();
  vCubikControl_Priv_setBeacon();
#endif

#ifdef BUILD_IN_ESPRESSIF
	// Nothing specific in here
#endif
}


/*
 * The private main function - called by the 'thread'
 *
 * Inputs: none
 *
 * Return: none
 *
 * Actions: Does all internal processing of flags, periodic processing, RX checks, etc
 *
 * NB: This function needs to be called by the main OS thread - every 1ms
*/
void vCubikControl_Priv_Main(void)
{
  /* Are we active */
  if(TRUE == Cubik_Priv_bLibActive)
  {
    /* If the beacon broadcast is enabled, TX messages at the configured frequency */
    if((TRUE == Cubik_Priv_bBTStackActive) && (TRUE == Cubik_Priv_bBTBroadcastActive) && (Cubik_Priv_u16BeaconTxFrequency > 0U))
    {
      // Dont do anything in here for now - broadcast is done automatically in the RTOS BT thread
    }

		// Increment the loop counter - allow for overflow wrap
		if(Cubik_Priv_u64LoopCounter == MAX_64BIT_REGISTER_VALUE)
		{
			Cubik_Priv_u64LoopCounter = 0U;
		}
		else
		{
			Cubik_Priv_u64LoopCounter++;
		}
  }
}


/*
 * Validates GPIO pin is valid
 *
 * Inputs: uint8_t u8PinNum - number of the pin
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *
 * Actions: Check that the given pin is valid on the current device
*/
uint8_t u8CubikControl_Priv_GPIO_Pin_Validate(uint8_t u8PinNum)
{
  uint8_t u8RetCode = CA_RESULT_GOOD;

  if((u8PinNum >= 1U) && (u8PinNum <= 39U))
  {
    u8RetCode = CA_RESULT_GOOD;
  }
  else
  {
    u8RetCode = CA_RESULT_PARAMETER_RANGE_ERROR;
  }

  return u8RetCode;
}


/* GrJo - 2020_11_24 - this section is the Dali implementation
 *                     based on original 'simple Dali controller', converted from Arduino class to 'C' functions
*/


/*
 * Set the TX and RX pins - store and configure values, set default levels
*/
void SMT_Cubik_Dali_setPins(uint8_t pinTx, uint8_t pinRx)
{
  Cubik_Priv_u8DaliTxPin = pinTx; // user sets the digital pin as output
  SMT_Cubik_pinmode(Cubik_Priv_u8DaliTxPin, OUTPUT);

#ifdef PIN_INVERSION_NEEDED
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, HIGH);
#else
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, LOW);
#endif

  Cubik_Priv_u8DaliRxPin = pinRx; // user sets the digital pin as input
  SMT_Cubik_pinmode(Cubik_Priv_u8DaliRxPin, INPUT);
}

/*
 * Setup the timing calculations - depends on H/W speed
 * NB: My target H/W is an ESP32, so always 240Mhz clock
*/
void SMT_Cubik_Dali_setupTransmit(uint8_t pinTx, uint8_t pinRx)
{
  printf("Dali:setupTransmit(%d, %d)\n", pinTx, pinRx);
  
  SMT_Cubik_Dali_setPins(pinTx, pinRx);

  Cubik_Priv_speedFactor = 2;

  //we don't use exact calculation of passed time spent outside of transmitter
  //because of high overhead associated with it, instead we use this 
  //emprirically determined values to compensate for the time loss

/* This is NOT used, removing */
#if 0
  #if F_CPU == 1000000UL
    uint16_t compensationFactor = 88; //must be divisible by 8 for workaround
  #elif F_CPU == 8000000UL
    uint16_t compensationFactor = 12; 
  #elif F_CPU == 16000000UL
    uint16_t compensationFactor = 4; 
  #else // 240 Mhz
    uint16_t compensationFactor = 3; 
  #endif  
#endif

	/* GrJo - 2020_11_17 - for ESP32 at 240Mhz */
  Cubik_Priv_delay1 = Cubik_Priv_delay2 = (HALF_BIT_INTERVAL >> Cubik_Priv_speedFactor) - 2;
  Cubik_Priv_period = Cubik_Priv_delay1 + Cubik_Priv_delay2;

/* Debug output only */
#if 0
 printf("compensationFactor:%d, delay1:%d, delay2:%d, period:%d\n", compensationFactor, Cubik_Priv_delay1, Cubik_Priv_delay2, Cubik_Priv_period);
#endif
}


/* GrJo - 2020_12_14:
   This is the original Dali TX function - now sets a flag, that the BT scan end
   calls
*/
void SMT_Cubik_Dali_transmit(uint8_t cmd1, uint8_t cmd2)
{
	/* If the BT stack is active, then store the values for it to send when its ready,
			otherwise call the send itself
  */
	if(true == Cubik_Priv_scanning_triggered)
	{
printf("Storing dali tx code\n");
		Cubik_Priv_bDaliDataToSend = true;
		Cubik_Priv_u8DaliTxByte1 = cmd1;
		Cubik_Priv_u8DaliTxByte2 = cmd2;
	}
	else
	{
printf("Sending direct dali message\n");
		/* Can send this directly */
		SMT_Cubik_Dali_transmit_Actual(cmd1, cmd2);
	}
}


/* GrJo - 2020_12_14:
   This is the original Dali TX function - now sets a flag, that the BT scan end
   calls
*/
void SMT_Cubik_Dali_transmit_Actual(uint8_t cmd1, uint8_t cmd2)
{
// GrJo - 2020_12_26
// I found something useful here:
// https://github.com/beegee-tokyo/DHTesp/blob/master/DHTesp.cpp
// Its for reading a DH22 temp/humidity sensor.
// It does this by bitbanging the input and uses:
#if 0
	#ifdef ESP32
		// ESP32 is a multi core / multi processing chip
		// It is necessary to disable task switches during the readings
		portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
		portENTER_CRITICAL(&mux);
	#else
		//   cli();
		noInterrupts();

	// Do time sensitive stuff here

	#ifdef ESP32
		portEXIT_CRITICAL(&mux);
	#else
		//   sei();
		interrupts();
	#endif
#endif
#endif

// I need to test this out but it looks like it will solve my problems with Dali comms being
//  interrupted by other activity



/* Debug output only */
/* GrJo - 2020_12_10 - seems unreliable on the test bench - send command twice */
  printf("Dali:transmit_Actual(%d, %d)\n", cmd1, cmd2);

  SMT_Cubik_Dali_sendBit(1);
  SMT_Cubik_Dali_sendByte(cmd1);
  SMT_Cubik_Dali_sendByte(cmd2);

#ifdef PIN_INVERSION_NEEDED
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, LOW);
#else 
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, HIGH);
#endif

	// Re transmit - help improve reliability
// NB: Really I need a master function to handle the DALI state
//  It will be called to set the dali light level and request responses to ensure correct change of state/level

  SMT_Cubik_delay_function(100);

  SMT_Cubik_Dali_sendBit(1);
  SMT_Cubik_Dali_sendByte(cmd1);
  SMT_Cubik_Dali_sendByte(cmd2);

#ifdef PIN_INVERSION_NEEDED
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, LOW);
#else 
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, HIGH);
#endif 
}


void SMT_Cubik_Dali_sendByte(uint8_t b)
{
  for (int i = 7; i >= 0; i--) 
  {
    SMT_Cubik_Dali_sendBit((b >> i) & 1);
  }
}

void SMT_Cubik_Dali_sendBit(int b)
{
 if (b) {
    SMT_Cubik_Dali_sendOne();
  }
  else {
    SMT_Cubik_Dali_sendZero();
  } 
}

void SMT_Cubik_Dali_sendZero(void)
{
#ifdef PIN_INVERSION_NEEDED
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, LOW);
  SMT_Cubik_delayMicro_function(Cubik_Priv_delay2);
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, HIGH);
  SMT_Cubik_delayMicro_function(Cubik_Priv_delay1);
#else
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, HIGH);
  SMT_Cubik_delayMicro_function(Cubik_Priv_delay2);
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, LOW);
  SMT_Cubik_delayMicro_function(Cubik_Priv_delay1);
#endif
}

void SMT_Cubik_Dali_sendOne(void)
{
#ifdef PIN_INVERSION_NEEDED
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, HIGH);
  SMT_Cubik_delayMicro_function(Cubik_Priv_delay2);
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, LOW);
  SMT_Cubik_delayMicro_function(Cubik_Priv_delay1);
#else
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, LOW);
  SMT_Cubik_delayMicro_function(Cubik_Priv_delay2);
  SMT_Cubik_digitalWrite(Cubik_Priv_u8DaliTxPin, HIGH);
  SMT_Cubik_delayMicro_function(Cubik_Priv_delay1);
#endif
}

void SMT_Cubik_Dali_initialisation()
{
  const int delaytime = 10; //ms

/* Debug output only */
  printf("Dali:initialisation()\n");

  SMT_Cubik_delay_function(delaytime);
  SMT_Cubik_Dali_transmit(BROADCAST_C, RESET);
  SMT_Cubik_delay_function(delaytime);
  SMT_Cubik_Dali_transmit(BROADCAST_C, RESET);
  SMT_Cubik_delay_function(delaytime);
  SMT_Cubik_Dali_transmit(BROADCAST_C, OFF_C);
  SMT_Cubik_delay_function(delaytime);
  SMT_Cubik_Dali_transmit(0b10100101, 0b00000000); //initialise
  SMT_Cubik_delay_function(delaytime);
  SMT_Cubik_Dali_transmit(0b10100101, 0b00000000); //initialise

  SMT_Cubik_Dali_transmit(0b10100001, 0b00000000);  //terminate
}



/* Bluetooth related private functions */

#ifdef BUILD_IN_ARDUINO
/* This function sets up the Beacon payload data and sets it advertising */
void vCubikControl_Priv_setBeacon(void)
{
  uint64_t chipid = 0U;
  char aucBCastName[20];

  static uint16_t u16Counter = 0U;

// TODO: sort this out properly  
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));

  // TODO: rework this
  oBeacon.setMajor(u16Counter / 100);
  oBeacon.setMinor(u16Counter % 100);

  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  
// TODO: sort this out properly  
  oAdvertisementData.setFlags(0x06); // GENERAL_DISC_MODE (0x02) | BR_EDR_NOT_SUPPORTED 0x04

  // Get our MAC Id
  chipid=ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).

  // Put it into the string
  sprintf(&aucBCastName[0], "SL:%02X%02X:", (uint8_t)((chipid & 0xff0000000000) >> 40), (uint8_t)((chipid & 0xff00000000) >> 32)); // This is 8 bytes
  oAdvertisementData.setName(aucBCastName);

  // This needs to be NULL, then we add extra data on as needed
  std::string strServiceData = "";
  strServiceData += (char)14;     // Len
  strServiceData += (char)0xFF;   // Type

  // Adding my own data here
  strServiceData += "I:";
  strServiceData += (char)((u16Counter & 0xff00) >> 8);
  strServiceData += (char)((u16Counter & 0x00ff));
  u16Counter++;  

  strServiceData += "S:";
  strServiceData += (char)(Cubik_Priv_u8MyTriggeredStatus);
  strServiceData += "M:";
  strServiceData += (char)(Cubik_Priv_u8MyMaxLightLevel);
  strServiceData += "C:";
  strServiceData += (char)(Cubik_Priv_u8MyCurrentLightLevel);
  
  oAdvertisementData.addData(strServiceData);


// Understanding - the max payload is 31 bytes,
// Look here: https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2017/02/10/bluetooth_advertisin-hGsf
//
// So:
// 3 bytes for the 'setFlags'
// 2 bytes + data for the 'setName'
// 2 bytes + data for the 'addData' (actual payload)

  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
}
#endif


#ifdef BUILD_IN_ESPRESSIF
// These functions are from the scanning
// Check if the device was already discovered
bool bCubikControl_Priv_already_discovered(esp_bd_addr_t address)
{
  bool found = false;

  for(int i = 0; i < Cubik_Priv_Discovered_Devices_Num; i++) {

    for(int j = 0; j < ESP_BD_ADDR_LEN; j++)
      found = (Cubik_Priv_Discovered_Devices[i][j] == address[j]);

    if(found) break; // Yuk - hate these (but its ported code and it works)
  }

  return found;
}

# if 0
// TODO: Remove this, its debug only
void vCubikControl_Priv_dump_device_list(void)
{
  printf("Dumping device list: %02d devices\n", Cubik_Priv_Discovered_Devices_Num);

  for(int i = 0; i < Cubik_Priv_Discovered_Devices_Num; i++)
  {
    printf("%02d - ID:%02X:%02X:%02X;%02X:%02X:%02X - Name:%s, rssi:%02d, Cubik_Priv_EventType:%02x, Cubik_Priv_MinLight:%02x, Cubik_Priv_MaxLight:%02x, Cubik_Priv_Flags:%02x, Cubik_Priv_u64TimeStamp:%08lld, age:%08lld\n", i,
      Cubik_Priv_Discovered_Devices[i][0],
      Cubik_Priv_Discovered_Devices[i][1],
      Cubik_Priv_Discovered_Devices[i][2],
      Cubik_Priv_Discovered_Devices[i][3],
      Cubik_Priv_Discovered_Devices[i][4],
      Cubik_Priv_Discovered_Devices[i][5],
      Cubik_Priv_aucDiscovered_Names[i],
			Cubik_Priv_iRSSI[i],
			Cubik_Priv_EventType[i],
			Cubik_Priv_MinLight[i],
			Cubik_Priv_MaxLight[i],
			Cubik_Priv_Flags[i],
			Cubik_Priv_u64TimeStamp[i],
			(Cubik_Priv_u64LoopCounter - Cubik_Priv_u64TimeStamp[i]) // TODO: move this to an elapsed time function
			);
  }
}
#endif

// add a new device to the list
void vCubikControl_Priv_addDevice(esp_bd_addr_t address, uint8_t *paucName, uint8_t u8Len)
{
  if(Cubik_Priv_Discovered_Devices_Num < MAX_DISCOVERED_DEVICES)
	{
	  memcpy(Cubik_Priv_Discovered_Devices[Cubik_Priv_Discovered_Devices_Num], address, ESP_BD_ADDR_LEN);
	  memcpy(Cubik_Priv_aucDiscovered_Names[Cubik_Priv_Discovered_Devices_Num], paucName, u8Len);
	  Cubik_Priv_Discovered_Devices_Num++;
	}
}

// store the rx event data in our event store
void vCubikControl_Priv_store_device_data(esp_bd_addr_t bdaddr, esp_bt_dev_type_t addr_type, int rssi, uint8_t rsp_len, uint8_t *data)
{
	// Find the device in our table
  for(int i = 0; i < Cubik_Priv_Discovered_Devices_Num; i++)
	{
		if(!memcmp(Cubik_Priv_Discovered_Devices[i], bdaddr, ESP_BD_ADDR_LEN))
		{
			/* If any of the data has changed, store this as a new event */
			/* Also need to store this if the opcode is CA_MY_STATUS_REQUEST_NEIGHBOUR_TRIGGER */
			if((Cubik_Priv_EventType[i] != data[21]) || (Cubik_Priv_MinLight[i] != data[22]) || (Cubik_Priv_MaxLight[i] != data[23]) || (Cubik_Priv_Flags[i] != data[24]) ||
           (data[21] == CA_MY_STATUS_REQUEST_NEIGHBOUR_TRIGGER))
			{
				if(Cubik_Priv_u8NumberOfRxEvents < MAX_RX_EVENT_MESSAGES)
				{
					Cubik_Priv_aucRXEventOpCode[Cubik_Priv_u8NumberOfRxEvents] = data[21];
					Cubik_Priv_aucRXEventDataValue1[Cubik_Priv_u8NumberOfRxEvents] = data[22];
					Cubik_Priv_aucRXEventDataValue2[Cubik_Priv_u8NumberOfRxEvents] = data[23];
					Cubik_Priv_aucRXEventDataValue3[Cubik_Priv_u8NumberOfRxEvents] = data[24];
					Cubik_Priv_aucRXEventDeviceId[Cubik_Priv_u8NumberOfRxEvents] = i;
					Cubik_Priv_au64RXEventMsgTimestamp[Cubik_Priv_u8NumberOfRxEvents] = Cubik_Priv_u64LoopCounter;

					Cubik_Priv_u8NumberOfRxEvents++;

			    Cubik_Priv_iRSSI[i] = rssi;
			    Cubik_Priv_EventType[i] = data[21];
			    Cubik_Priv_MinLight[i] = data[22];
			    Cubik_Priv_MaxLight[i] = data[23];
			    Cubik_Priv_Flags[i] = data[24];
			    Cubik_Priv_u64TimeStamp[i] = Cubik_Priv_u64LoopCounter;
				}
			}
		}
  }
}

// There is a LOT of devices in the office - so this is vital
bool vCubikControl_Priv_is_device_an_smt(struct ble_scan_result_evt_param *data_struct)
{
	bool bRetCode = true; // Default to valid, detect in-valid
  uint8_t *adv_name = NULL;
  uint8_t adv_name_len = 0;

	// Check the payload length
	if(17U == data_struct->adv_data_len)
	{
		// Check the name matches the pattern
    adv_name = esp_ble_resolve_adv_data(data_struct->ble_adv, ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
    if((adv_name) && (adv_name_len == 15U))
		{
			// Name exists, check pattern
			if((adv_name[0] != 'S') ||
			   (adv_name[1] != 'M') ||
			   (adv_name[2] != 'T') ||
			   (adv_name[3] != ':') ||
    		 (adv_name[6] != ':') ||
			   (adv_name[9] != ':') ||
			   (adv_name[12] != ':'))
			{
				// Invalid name pattern
				bRetCode = false;
			}
		}
		else
		{
			// Invalid name length or not present
			bRetCode = false;
		}
	}
	else
	{
		// Invalid payload length
		bRetCode = false;
	}

// GrJo - 2020_12_17 - cant use this, there are more address ranges than I thought
#if 0
	if(true == bRetCode)
	{
		if((memcmp(data_struct->bda, u8MacPattern1, 3U)) &&
		   (memcmp(data_struct->bda, u8MacPattern2, 3U)))
		{
			// 1st 3 bytes of MAC does not match
			bRetCode = false;
		}
	}
#endif

	return bRetCode;
}


// GAP callback
void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	uint8_t *adv_name = NULL;
  uint8_t adv_name_len = 0;

if(true == bDaliTXInProgress)
{
	printf("bDaliTXInProgress during ble cb event !!!!!\n");
}

    switch (event)
		{
			case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT: 
				Cubik_Priv_adv_data_set = true;
				if(Cubik_Priv_scan_rsp_set) esp_ble_gap_start_advertising(&ble_adv_params);
				break;
			
			case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT: 
				Cubik_Priv_adv_data_set = true;
				if(Cubik_Priv_scan_rsp_set) esp_ble_gap_start_advertising(&ble_adv_params);
				break;

			case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
				Cubik_Priv_scan_rsp_set = true;
				if(Cubik_Priv_adv_data_set) esp_ble_gap_start_advertising(&ble_adv_params);
				break;
			
			case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
				Cubik_Priv_scan_rsp_set = true;
				if(Cubik_Priv_adv_data_set) esp_ble_gap_start_advertising(&ble_adv_params);
				break;
		
			case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
				if(param->adv_start_cmpl.status == ESP_BT_STATUS_SUCCESS)
			  {
					// Now start the scanning going
					if(Cubik_Priv_scanning_triggered == false)
					{
						Cubik_Priv_scanning_triggered = true;
 						// configure scan parameters
					  esp_ble_gap_set_scan_params(&ble_scan_params);
					}
				}
				else
				{
// TODO: something with this, its a broadcasting error ?
					printf("Unable to start advertising process, error code %d\n\n", param->scan_start_cmpl.status);
				}
			  break;

			// These cases are from the scanning
			case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
	      if(param->scan_param_cmpl.status == ESP_BT_STATUS_SUCCESS)
				{
					// Increment scan counter
					Cubik_Priv_u64ScanCounter++;
        	esp_ble_gap_start_scanning(1);
	      }
      	else
				{
// TODO: something with this, its a broadcasting error ?
					 printf("Unable to set scan parameters, error code %d\n\n", param->scan_param_cmpl.status);
				}
	      break;

			case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
      	if(param->scan_start_cmpl.status == ESP_BT_STATUS_SUCCESS)
				{
//        printf("Scan started\n\n");
	      }
	      else
				{
// TODO: something with this, its a broadcasting error ?
					printf("Unable to start scan process, error code %d\n\n", param->scan_start_cmpl.status);
				}
  	    break;

    case ESP_GAP_BLE_SCAN_RESULT_EVT:
      if(param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT)
			{
				// Filter to only our devices
				if(true == vCubikControl_Priv_is_device_an_smt(&param->scan_rst))
				{
// TODO: remove this, its debug
#if 0
					printf("Got message from :");
					for(int i = 0; i < ESP_BD_ADDR_LEN; i++)
					{
					  printf("%02X", param->scan_rst.bda[i]);
					  if(i != ESP_BD_ADDR_LEN -1)
							printf(":");
					}
					printf("\n");
#endif

        	if(!bCubikControl_Priv_already_discovered(param->scan_rst.bda))
				  {
// TODO: remove this, its debug
#if 1
	          printf("ESP_GAP_BLE_SCAN_RESULT_EVT\n");
	          printf("Device found: ADDR=");
	          for(int i = 0; i < ESP_BD_ADDR_LEN; i++)
						{
	            printf("%02X", param->scan_rst.bda[i]);
	            if(i != ESP_BD_ADDR_LEN -1) printf(":");
	          }
#endif

  	        // try to read the complete name
	          adv_name = esp_ble_resolve_adv_data(param->scan_rst.ble_adv, ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
	          if(adv_name) {
// TODO: remove this, its debug
#if 1
	            printf("\nFULL NAME=");
	            for(int i = 0; i < adv_name_len; i++) printf("%c", adv_name[i]);
#endif
	            vCubikControl_Priv_addDevice(param->scan_rst.bda, adv_name, adv_name_len);
	          }

// TODO: remove this, its debug
#if 1
	          printf("\n\n");
#endif
	      	}

					// This gives me the name and the custom data (entire advertised packet - 1 bytes)
					vCubikControl_Priv_store_device_data(param->scan_rst.bda, param->scan_rst.ble_addr_type, param->scan_rst.rssi,
                                               (param->scan_rst.adv_data_len + param->scan_rst.scan_rsp_len), param->scan_rst.ble_adv);
				}
			}
      else if(param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT)
     	{
// TODO: remove this, its for debug
//				vCubikControl_Priv_dump_device_list();


				// GrJo - 2020_12_16 - see if we have anything to do between scans
				vCubikControl_Priv_Check_For_BetweenScans_Work();

				// Delay between scans - avoid watchdog barfff
				// Keep this low, to avoid excessive 'neighbour trigger delays'
				vTaskDelay(100 / portTICK_PERIOD_MS);

				esp_ble_gap_set_scan_params(&ble_scan_params);
      }
      break;

		default:
// TODO: remove this, its for debug
			printf("Event %d unhandled\n\n", event);
			break;
	}
}

/* Do we need to do anything between the BT scans */
void vCubikControl_Priv_Check_For_BetweenScans_Work(void)
{
	if(true == Cubik_Priv_bDaliDataToSend)
	{
		SMT_Cubik_Dali_transmit_Actual(Cubik_Priv_u8DaliTxByte1, Cubik_Priv_u8DaliTxByte2);

		Cubik_Priv_u8DaliTxByte1 = 0U;
		Cubik_Priv_u8DaliTxByte2 = 0U;
		Cubik_Priv_bDaliDataToSend = false;
	}
}


/* This function sets up the Beacon payload data and sets it advertising */
void vCubikControl_Priv_setBeacon(void)
{
	uint64_t u64TargetValue = 0U;

	// Set up my advertising frequency, based on the configured value - configured value is (ms * 0.625)
	u64TargetValue = ((Cubik_Priv_u16BeaconTxFrequency * 1000) / 625);
	ble_adv_params.adv_int_min = u64TargetValue;
	ble_adv_params.adv_int_max = u64TargetValue;

  ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&adv_data));
  ESP_ERROR_CHECK(esp_ble_gap_config_adv_data_raw(adv_raw_data, 17));

  // configure the scan response data
  ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&scan_rsp_data));

	scan_rsp_raw_data[4] = Cubik_Priv_u8MyTriggeredStatus;
	scan_rsp_raw_data[5] = Cubik_Priv_u8MyMaxLightLevel;
	scan_rsp_raw_data[6] = Cubik_Priv_u8MyCurrentLightLevel;
  ESP_ERROR_CHECK(esp_ble_gap_config_scan_rsp_data_raw(scan_rsp_raw_data, 8));
}

/* Turn on the beacon tx */
void vCubikControl_Priv_Beacon_Start(void)
{
	static bool bBTStackIsEnabled = false;
printf("vCubikControl_Priv_Beacon_Start()\n");

	// Guard against multiple starts
	if(false == bBTStackIsEnabled)
	{
printf("vCubikControl_Priv_Beacon_Start() - main\n");
		bBTStackIsEnabled = true;
		Cubik_Priv_scanning_triggered = false;

	  // set components to log only errors
	  esp_log_level_set("*", ESP_LOG_ERROR);

	  // initialize nvs
	  ESP_ERROR_CHECK(nvs_flash_init());
	  printf("- NVS init ok\n");

	  // release memory reserved for classic BT (not used)
	  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
	  printf("- Memory for classic BT released\n");

 	  // initialize the BT controller with the default config
	  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
 	  printf("- BT controller init ok\n");

 	  // enable the BT controller in BLE mode
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
 	  printf("- BT controller enabled in BLE mode\n");

    // initialize Bluedroid library
    esp_bluedroid_init();
    esp_bluedroid_enable();
    printf("- Bluedroid initialized and enabled\n");

    // register GAP callback function
    ESP_ERROR_CHECK(esp_ble_gap_register_callback(esp_gap_cb));
    printf("- GAP callback registered\n");

		// Set transmit power level
		if(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P9) == OK)
		{
			printf("BLE TX Power level set\n");
		}


    // configure the adv data
    ESP_ERROR_CHECK(esp_ble_gap_set_device_name("ESP32_ScanRsp")); // This seems to be ignored ?

    // Populate the last 4 bytes of my mac address into the name field
    sprintf((char *)(adv_raw_data + 6), "%02X:%02X:%02X:%02X", Cubik_Priv_aucMyMAC[2], Cubik_Priv_aucMyMAC[3], Cubik_Priv_aucMyMAC[4], Cubik_Priv_aucMyMAC[5]);

 	  /* Start advertising, with my custom data */
  	vCubikControl_Priv_setBeacon();
	}
}

/* Turn off the beacon tx */
void vCubikControl_Priv_Beacon_Stop(void)
{
	esp_bt_controller_disable();
}
#endif


