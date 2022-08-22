
/* File Header
 *
 *  FileName:   SMT_Cubik_API_Data_Types.h
 *  Purpose:    This file contains all the datatypes, #defines, macros, function prototypes, constants
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
 * GrJo - 2020_11_24 - Added analog input gpio type
 *                     Added BT Beacon broadcast function (no RX yet)
 *                     Added pin config #defines for WROOM and CAM board types
 * GrJo - 2020_11_25 - Added build define entries to split between Arduino/Espressif build
 *                     Converted Dali class to direct C functions
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

#define FILE02_FILENAME       "SMT_Cubik_API_Data_Types.h"
#define FILE02_VERSION        "0.1"
#define FILE02_DATE           "2020_12_18"
#define FILE02_DESCRIPTION    "Header file - contains shared data types and public prototypes"



/* Set these according to whether we are using Arduino or espressif build */
//#define BUILD_IN_ARDUINO
#define BUILD_IN_ESPRESSIF

/* Define these for the board and compiler environment */
/* Set these according to the board we are on (can I autodetect this ?) */
//#define ESP32_WROOM
#define ESP32_CAM


/* includes */

#ifdef BUILD_IN_ARDUINO
	#include "arduino.h"
#endif	

#ifdef BUILD_IN_ESPRESSIF
	#include <stdio.h>
	#include <driver/gpio.h>

	#include "esp_gap_ble_api.h"
	#include "esp_gattc_api.h"
#endif


/* Constant Values */

/* How many nieghbour devices can we cope with */
#define MAX_DISCOVERED_DEVICES 20


#ifdef ESP32_WROOM
  /* This is the default pin used for transmission to the DALI bus */
  #define CUBIK_DALI_INTERFACE_TX_PIN 16

  /* This is the default pin used for reception from the DALI bus */
  #define CUBIK_DALI_INTERFACE_RX_PIN 17
#endif


#ifdef ESP32_CAM
// GrJo - 2020_11_17 - these are the pins I used on my dev breadboard buildup
#if 0
  /* This is the default pin used for transmission to the DALI bus */
  #define CUBIK_DALI_INTERFACE_TX_PIN 13

  /* This is the default pin used for reception from the DALI bus */
  #define CUBIK_DALI_INTERFACE_RX_PIN 12
#else
  /* This is the default pin used for transmission to the DALI bus */
  #define CUBIK_DALI_INTERFACE_TX_PIN 12

  /* This is the default pin used for reception from the DALI bus */
  #define CUBIK_DALI_INTERFACE_RX_PIN 4
#endif

#endif



/* Defined Values */
#ifdef BUILD_IN_ARDUINO
	#define FALSE false
	#define TRUE true
#endif

#ifdef BUILD_IN_ESPRESSIF
	#define FALSE 0
	#define TRUE  1

	#define INPUT					1
	#define INPUT_PULLUP  2
	#define OUTPUT				3
	#define LOW						4
	#define HIGH					5
#endif


/* DALI coomands */
#define BROADCAST_DP 0b11111110	/* Broadcast address, sends 'set light level' to all devices */
#define BROADCAST_C  0b11111111 /* Broadcast address, sends 'command following' to all devices */
#define ON_C 0b00000101         /* Generic command - turn on to max level */
#define OFF_C 0b00000000        /* Generic command - turn off to 0% level */
#define RESET 0b00100000        /* Generic command - tell device(s) to perform reset */
#define HALF_BIT_INTERVAL 1666  /* Used in timing calcualtion */


/* These are not currently used in my implementation */
#if 0
#define ON_DP 0b11111110				
#define OFF_DP 0b00000000
#define QUERY_STATUS 0b10010000
#endif



/* the function returned success */
#define CA_RESULT_GOOD                         1

/* some unknown error was returned */
#define CA_RESULT_UNSPECIFIED_ERROR            2

/* a supplied parameter was out of range */
#define CA_RESULT_PARAMETER_RANGE_ERROR        3

/* the requested operation was invalid */
#define CA_RESULT_REQUEST_INVALID              4
  
/* the cubik library is not currently active */
#define CA_RESULT_LIB_NOT_ACTIVE               5

/* the requested operation is queued and will be executed in the cubik operation thread */
#define CA_RESULT_LIB_RESULT_PENDING           6

/* the requested operation returned no data */
#define CA_RESULT_NO_VALUE_STORED              7


/* A dummy opcode, used as a placeholder in case of code logic error */
#define CA_REQUEST_DUMMY_OPCODE                10

/* Request to enable the interface */
#define CA_REQUEST_IFACE_ENABLE                11

/* Request to disable the interface */
#define CA_REQUEST_IFACE_DISABLE               12

/* Request to enable broadcast mode */
#define CA_REQUEST_MSG_BCAST_ENABLE            13

/*  Request to disable broadcast mode */
#define CA_REQUEST_MSG_BCAST_DISABLE           14

/* Request to send a message (e.g. a status message to another device) */
#define CA_REQUEST_MSG_SEND                    15

/* Request to set the light to full brightness (fully on) */
#define CA_REQUEST_SET_LIGHT_ON                16

/* Request to set the light to zero brightness (fully off) */
#define CA_REQUEST_SET_LIGHT_OFF               17

/* Request to set the light to specified brightness */
#define CA_REQUEST_SET_LIGHT_LEVEL             18

/* Request to set the status of a device (e.g. the DALI device status) */
#define CA_REQUEST_SET_QUERY_STATUS            19


/* A return message, indicating a status response from a previous request */
#define CA_STATUS_RESPONSE                     20


/* Set a GPIO pin as output */  
#define CA_PIN_CONFIG_OUTPUT                   30

/* Set a GPIO pin as input */
#define CA_PIN_CONFIG_INPUT                    31

/* Set a GPIO pin as analog input - no internal pullup */
#define CA_PIN_CONFIG_ANALOG_INPUT             32


/* Set a (preconfigured) GPIO pin to on */
#define CA_PIN_SET_ON                          40

/* Set a (preconfigured) GPIO pin to off */
#define CA_PIN_SET_OFF                         41

/* Get the current logic level of the GPIO pin */
#define CA_PIN_GET_DIGITAL_STATE               42

/* Get the current analogue voltage level of the GPIO pin */
#define CA_PIN_GET_ANALOG_LEVEL                43


/* The gpio level is LOW (off) */
#define CA_PIN_VALUE_LOW                       50
  
/* The gpio level is HIGH (on) */
#define CA_PIN_VALUE_HIGH                      51


/* Enable the specified interface */
#define CA_STATUS_ENABLE                       60

/* Disable the specified interface */
#define CA_STATUS_DISABLE                      61


/* Device is entering/entered ‘sleep’ state (low transmit frequency, lights off – normal daylight operation) */
#define CA_MY_STATUS_SLEEPING                  70

/* Device is awake, light is active (in low power mode) */
#define CA_MY_STATUS_AWAKE                     71

/* Device is awake, light is actively triggered (in high power mode) */
#define CA_MY_STATUS_TRIGGERED                 72

/* Device is awake, light is in high power mode, post trigger timer is active */
#define CA_MY_STATUS_POST_TRIGGERED            73

/* Device is active, I want neighbours to trigger */
#define CA_MY_STATUS_REQUEST_NEIGHBOUR_TRIGGER 74

/*  The system controller is setting a new maximum light level */
#define CA_CONTROLLER_SET_MAX_LIGHT_LEVEL      75

/* The system controller is setting a new minimum light level */
#define CA_CONTROLLER_SET_MIN_LIGHT_LEVEL      76

/* The DALI detection function has detected a DALI controller on the DALI bus */
#define CA_MYSELF_DALI_DEVICE_DETECTED         77

/* The DALI detection function has NOT detected a DALI controller on the DALI bus */
#define CA_MYSELF_DALI_NO_DEVICE_FOUND         78

/* A status code has been received from the DALI light unit */
#define CA_MSG_RX_DALI_STATUS                  79

/* A 'master controller' command - such as set you min/max light levels, global turn on/off */
#define CA_MSG_MASTER_COMMAND_SET_LEVELS       80
#define CA_MSG_MASTER_COMMAND_SET_OPERATION    81


/* Message flags - bitwise */
#define CA_MSG_FLAG_DO_NOTHING								 1U
#define CA_MSG_FLAG_REBROADCAST                2U




/* Macros */


/* Typedefs */
typedef unsigned char uint8_t;


/* Public Function Prototypes - can be called by other library */

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
uint8_t u8CubikControl_LibInit(void);

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
uint8_t u8CubikControl_GetMyDeviceID(uint8_t u8MaxLength, uint8_t *pu8Length, uint8_t *pu8DataBuffer);

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
uint8_t u8CubikControl_DaliLight_Init(uint8_t u8Opcode, uint8_t u8TXPin, uint8_t u8RXPin);

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
uint8_t u8CubikControl_DaliLight_SetLevel(uint8_t u8Level);


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
uint8_t u8CubikControl_DaliLight_QueryStatus(void);

/*
 * Configure a GPIO pin as input or output
 *
 * Inputs: uint8_t u8PinNum - the GPIO pin number
 *         uint8_t u8Opcode - CA_PIN_CONFIG_OUTPUT – configure the given pin as output
 *                            CA_PIN_CONFIG_INPUT – configure the given pin as input
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Configure the specified GPIO pin for the specified mode
 *          Input will be set with weak pullup, Output will be set to off
 *
*/
uint8_t u8CubikControl_GPIO_Pin_Config(uint8_t u8PinNum, uint8_t u8OpCode);

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
uint8_t u8CubikControl_GPIO_Pin_ValueSet(uint8_t u8PinNum, uint8_t u8Value);

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
uint8_t u8CubikControl_GPIO_Pin_GetDig(uint8_t u8PinNum, uint8_t *pu8RetValue);

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
uint8_t u8CubikControl_GPIO_Pin_GetAna(uint8_t u8PinNum, uint8_t *pu8RetValue);

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
uint8_t u8CubikControl_BT_SetDeviceEnable(uint8_t u8Operation);

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
uint8_t u8CubikControl_BT_SetBroadcastEnable(uint8_t u8Operation);

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
uint8_t u8CubikControl_BT_SetBroadcastStatus(uint8_t u8MyTriggeredStatus, uint8_t u8MyMaxLightLevel, uint8_t u8MyCurrentLightLevel);

/*
 * Get the neighbour list
 *
 * Inputs: uint8_t u8DeviceIdSlot - Which device id to get - 0 = us, 1 = master control, 2-255 neighbour devices
 *         uint8_t u8MaxDeviceIdLen - The size (in bytes) of the receive buffer
 *         uint8_t *pau8DeviceId - pointer to the receive buffer
 *
 * Return Values - CA_RESULT_GOOD - the function returned success
 *                 CA_RESULT_PARAMETER_RANGE_ERROR - a supplied parameter was out of range
 *                 CA_RESULT_NO_VALUE_STORED - the specified id slot does not contain a detected device
 *                 CA_RESULT_LIB_NOT_ACTIVE – the cubik library is not currently active
 *
 * Actions: Returns the device_id from the specified slot (if a device has been detected)
 *
*/
uint8_t u8CubikControl_GetDetectedNeighbourList(uint8_t u8DeviceIdSlot, uint8_t u8MaxDeviceIdLen, uint8_t *pau8DeviceId);

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

uint8_t u8CubikControl_RX_GetStatusMsg(uint8_t *pu8DeviceIdSlot, uint8_t *pu8OpCode, uint8_t *pu8DataValue1, uint8_t *pu8DataValue2, uint8_t *pu8DataValue3, uint64_t *pu64MsgAge);



/* Private Function Prototypes - internal use only */

/*
 * The private setup function, defaults variables, etc
 *
 * Inputs: none
 *
 * Return: none
 *
 * Actions: Performs API initialisation
 *
*/
void vCubikControl_Priv_Setup(void);

/*
 * The private main function - called by the 'thread'
 *
 * Inputs: none
 *
 * Return: none
 *
 * Actions: Does all internal processing of flags, periodic processing, RX checks, etc
 *
*/
void vCubikControl_Priv_Main(void);

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
uint8_t u8CubikControl_Priv_GPIO_Pin_Validate(uint8_t u8PinNum);


/*
 * Enables the beacon, pre-populates with default parameters
 *
 * Inputs: none
 *
 * Return Values: none
 *
 * Actions: sets up the beacon, with the data to transmit
*/
void vCubikControl_Priv_setBeacon(void);

  
/* Generic delay function  - private implementation but available (unnoficialy) to main application */
void SMT_Cubik_delay_function(uint16_t u16MSToDelay);

/* Function prototypes for C implementation of Dali library */
void SMT_Cubik_Dali_setPins(uint8_t pinTx, uint8_t pinRx);
void SMT_Cubik_Dali_setupTransmit(uint8_t pinTx, uint8_t pinRx);
void SMT_Cubik_Dali_transmit(uint8_t cmd1, uint8_t cmd2);
void SMT_Cubik_Dali_sendByte(uint8_t b);
void SMT_Cubik_Dali_sendBit(int b);
void SMT_Cubik_Dali_sendZero(void);
void SMT_Cubik_Dali_sendOne(void);
void SMT_Cubik_Dali_initialisation();

/* Private functions - for Espressif build iBeacon */
void vCubikControl_Priv_Beacon_Start(void);
void vCubikControl_Priv_Beacon_Stop(void);



