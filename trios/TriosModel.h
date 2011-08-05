#ifndef __TriosModel__ 
#define __TriosModel__
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/*! 
*  \file TriosModel.h
*  TriosBase
*
*  \author Guy Van Overtveldt 
*  \date 11/09/10
*  Copyright 2010 All rights reserved.
*
*/

/*!
 \struct Light
 \brief The Light structure
 
 \typedef TLight
 \brief The TLight type
 
 \typedef pTLight
 \brief The Pointer to TLight
 
 \struct Cortex
 \brief The Cortex structure 

 \typedef TCortex 
 \brief The TCortex type
 
 \typedef pTCortex
 \brief The Pointer to TCortex
 
 \struct Header
 \brief The Header structure
 
 \typedef TMessageHeader 
 \brief The TMessageHeader type
 
 \typedef pTMessageHeader
 \brief The Pointer to TMessageHeader
 
 \struct Message
 \brief The Messages structure
 
 \typedef TMessage 
 \brief The TMessages type
 
 \typedef pTMessage
 \brief The Pointer to TMessages
 
 \struct Data
 \brief The Data structure
 
 \typedef TData 
 \brief The TData type

 \typedef pTData
 \brief The Pointer to TData

 \struct LightModel
 \brief The Data structure
 
 \typedef TLightModel 
 \brief The TLightModel type
 
 \typedef pTLightModel
 \brief The Pointer to TLightModel

 \struct CortexModel
 \brief The Data structure
 
 \typedef TCortexModel 
 \brief The TCortextModel type
 
 \typedef pTCortexModel
 \brief The Pointer to TCortexModel
 
 \typedef EMSG
 Message enumeration for readbility
 \note use <i> eMSG1 </i> for index 0

 \typedef ECMD
 The command to execute for a message
 \note Only get and put implemented

 \typedef ECORTEXGETADR
 The I2C addresses to issue a get command

 \typedef ECORTEXPUTADR
 The I2C addresses to issue a put command
 
 \typedef ELIGHTS
 The light index to point to light struct
 
 \typedef ECORTEXES
 The cortexes that can be used as indexes
*/

#define AMOUNT_OF_CORTEXES	4								/*!< cortex blocks available */
#define MAXLIGHTS			6								/*!< lights available */
#define MESSAGELENGTH		128								/*!< fixed message length*/
#define DATABUFFERLENGTH	1024							/*!< multiple of 128 */
#define MESSAGES			DATABUFFERLENGTH/MESSAGELENGTH	/*!< 8 messages of 128 bytes */

#define FREESIZE			MESSAGELENGTH - \
							sizeof(TMessageHeader) - \
							sizeof(TCortex) - \
							(sizeof(TLight)*MAXLIGHTS)
/*!< Free bytes in a message needed to have the correct total size for struct */

#define LIGHTVALUEMIN		0								/*!< max light value */
#define LIGHTVALUEMAX		10000							/*!< min light value */
#define LIGHTSTEPMAX		1000.0F							/*!< dimmer step need float for calculation */


#define TRIOS_ERROR_OK		0								/*!< result OK */

/***********************TYPEDEFS*********************************************/

/*! shorter 16 bit force */
typedef unsigned short cortexint;

typedef enum{
	eMSG1,
	eMSG2,
	eMSG3,
	eMSG4,
	eMSG5,
	eMSG6,
	eMSG7,
	eMSG8
} EMSG;

typedef enum{
	eCORTEX1,
	eCORTEX2,
	eCORTEX3,
	eCORTEX4
} ECORTEXES;

typedef enum{	
	eCMDPUT=0x01,
	eCMDGET=0x02
} ECMD;

typedef enum{	
	eCORTEX1GETADR=0x10,
	eCORTEX2GETADR=0x30,
	eCORTEX3GETADR=0x50,
	eCORTEX4GETADR=0x70
} ECORTEXGETADR;

typedef enum{	
	eCORTEX1PUTADR=0x20,
	eCORTEX2PUTADR=0x40,
	eCORTEX3PUTADR=0X60,
	eCORTEX4PUTADR=0x80
} ECORTEXPUTADR;

typedef enum{	
	eLIGHT1,
	eLIGHT2,
	eLIGHT3,
	eLIGHT4,
	eLIGHT5,
	eLIGHT6
} ELIGHTS;

typedef struct Header{
	unsigned char command;/*!< @see ECMD */
	unsigned char address;/*!< @see ECORTEXGETADR,ECORTEXPUTADR */
	unsigned char commandlength;/*!< always 0 not used*/
	unsigned char datalength;/*!< light and cortex struct size*/
} TMessageHeader, *pTMessageHeader;

typedef struct Light{
	unsigned short value;/*!< value between 0 and 10000 */
	unsigned short minimum;/*!< */
	unsigned short maximum;/*!< */
	unsigned short step;/*!< */
	unsigned short pinmaskin;/*!< */
	unsigned short pinmaskout;/*!< */
} TLight,* pTLight;

typedef struct Cortex{
	unsigned short temperature;/*!< readonly*/
	unsigned short watchdogs;/*!< readonly*/
	unsigned short toggles;/*!< readonly*/
	unsigned short dimmers;/*!< readonly*/
	unsigned short hours;/*!< readonly*/
	unsigned short masks;/*!< read/write*/
} TCortex,* pTCortex;

typedef struct Message{
	TMessageHeader	header;/*!< */
	TLight			lights[MAXLIGHTS];/*!< @see MAXLIGHTS */
	TCortex			cortex;/*!< */
	unsigned char	free[FREESIZE];/*!< @see FREESIZE */ 
} TMessage , * pTMessage;

typedef struct Data{
	TMessage data[MESSAGES];/*!< @see MESSAGES */
} TData, * pTData;

typedef struct LightModel{
	TLight lights;/*!< light values */
	char * name;/*!< the light name*/
}TLightModel,* pTLightModel;

typedef struct CortexModel{
	TCortex cortexes[AMOUNT_OF_CORTEXES]; /*!< cortexes info > */
}TCortexModel,* pTCortexModel;

typedef TData TTriosDataBuffer;/*!< */
typedef pTData pTTriosDataBuffer;/*!< */
typedef unsigned char * pUCTriosDataBuffer;/*!< */

/*************************PUBLIC GLOBALS*************************************/
/*! 
 Public global variable with linear array to ease the UI callsr\n
 All values must be set in range 0-100% except port in/out \n
 Cortex 1 will have index 0-5\n
 Cortex 2 will have index 6-11\n
 Cortex 3 will have index 12-17\n
 Cortex 4 will have index 18-23\n
 @note Updates from UI or Trios should always end up using in this array
 */

extern TLightModel	gTriosLights[MAXLIGHTS*AMOUNT_OF_CORTEXES];

/*! 
 Public global variable with linear array to ease the UI calls\n 
 Cortex 1 will have index 0\n
 Cortex 2 will have index 1\n
 Cortex 3 will have index 2\n
 Cortex 4 will have index 3\n
 @note All fields are readonly except masks
 */

extern TCortexModel	gTriosCortexes;

/*! 
 Public global variable containing the lightpoint names
 @note must be changed to flat text file
 */

extern const char * gpCLightNames[24];

/****************************************************************************/

/*************************PUBLIC FUNCTIONS***********************************/
/*!
 Sets IP and PORT to communicate
 @param ip servers ip address
 @param port servers port number
*/

void TriosSetEthernet (char * ip , int port);
/****************************************************************************/
/*!
 Initialize the buffer with the first GET commands
*/

void TriosInitBuffer (void);
/****************************************************************************/
/*!
	Sends the messages with POST command in header
 */

int TriosSendPostBuffer (void);
/****************************************************************************/
/*!
 Sends the messages with GET command in header
 */ 

int TriosSendGetBuffer (void);
/****************************************************************************/

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif
