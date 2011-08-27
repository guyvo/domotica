/*! \file TriosModel.c
 *  TriosBase
 *
 *  Created by Guy Van Overtveldt on 11/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 * 
 */

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> // for close socket
#include "TriosModel.h"

/**
* \defgroup Golbals Global Variables
*/

/*@{*/

/** 
 The global data buffer hidden in this module \n
 All functions are provided in the headerfile \n
 to work with converted flat arrays
 \note size is limited to 1k
*/ 

TTriosDataBuffer	gData;
TLightModel			gTriosLights[MAXLIGHTS*AMOUNT_OF_CORTEXES];
TCortexModel		gTriosCortexes;

/*! servers ip address */
char 				gIpAddress[20];

/*! servers port number */
int					gPort;

/*! light names */
const char * gpCLightNames[24] = {
	"toilet beneden",
	"berging",
	"vide draad brug",
	"garage",
	"vrij",
	"living voor",
	"living midden",
	"slaapkamer beneden",
	"slaapkamer 1",
	"slaapkamer 4",
	"wand trap",
	"hal passage",
	"wand living",
	"living achter",
	"keuken tafel",
	"keuken werkblad",
	"vrij",
	"terras buitendeur",
	"badkamer beneden",
	"zolder",
	"badkamer boven",
	"slaapkamer 2",
	"slaapkamer 3",
	"vrij"
};

/*@}*/


/****************************************************************************/
/*!
 Sets the header to execute a get command for a given cortex
 @param msg the message index
 @param adr the cortex address
 */ 

static void TriosPrepareGetInMessage (EMSG msg , ECORTEXGETADR adr){
	gData.data[msg].header.command = eCMDGET;
	gData.data[msg].header.commandlength = 0;
	gData.data[msg].header.address = adr;
	gData.data[msg].header.datalength = (sizeof(TLight) * MAXLIGHTS) + sizeof (TCortex);
}

/****************************************************************************/
/*!
 Sets the header to execute a put command for a given cortex
 @param msg the message index
 @param adr the cortex address
 */ 

static void TriosPreparePutInMessage (EMSG msg , ECORTEXPUTADR adr){
	gData.data[msg].header.command = eCMDPUT;
	gData.data[msg].header.commandlength = 0;
	gData.data[msg].header.address = adr;
	gData.data[msg].header.datalength = (sizeof(TLight) * MAXLIGHTS) + sizeof (TCortex);
}

/****************************************************************************/
/* to 0-10000 @note float needed on step intermediate is smaller than 1 */
static void TriosLightFromArray (void){
	int light;
	
	for (light = 0; light < AMOUNT_OF_CORTEXES * MAXLIGHTS; light++) {
		
		gData.data[light / MAXLIGHTS].lights[light % MAXLIGHTS] = gTriosLights[light].lights;
		
		gData.data[light / MAXLIGHTS].lights[light % MAXLIGHTS].value =
		LIGHTVALUEMAX - (LIGHTVALUEMAX * gData.data[light / MAXLIGHTS].lights[light % MAXLIGHTS].value) / 100;
		
		gData.data[light / MAXLIGHTS].lights[light % MAXLIGHTS].minimum	=
		LIGHTVALUEMAX - (LIGHTVALUEMAX * gData.data[light / MAXLIGHTS].lights[light % MAXLIGHTS].minimum) / 100;
		
		gData.data[light / MAXLIGHTS].lights[light % MAXLIGHTS].maximum =
		LIGHTVALUEMAX - (LIGHTVALUEMAX * gData.data[light / MAXLIGHTS].lights[light % MAXLIGHTS].maximum) / 100;
		
		gData.data[light / MAXLIGHTS].lights[light % MAXLIGHTS].step =
		(gData.data[light / MAXLIGHTS].lights[light % MAXLIGHTS].step / 100.0) * LIGHTSTEPMAX;
		
	}

}

/****************************************************************************/
/* to 0-100% @note float needed on step intermediate is smaller than 1 */
static void TriosLightToArray (void){
	EMSG eMsg;
	ELIGHTS eLight;
	
	for ( eMsg = eMSG1; eMsg < AMOUNT_OF_CORTEXES; eMsg++) {
		for ( eLight = eLIGHT1; eLight < MAXLIGHTS; eLight++){
			
						
			gTriosLights[eLight+(eMsg*MAXLIGHTS)].lights = gData.data[eMsg].lights[eLight];
			
			gTriosLights[eLight+(eMsg*MAXLIGHTS)].lights.value = 
			(LIGHTVALUEMAX - gTriosLights[eLight+(eMsg*MAXLIGHTS)].lights.value) / 100;
			
			gTriosLights[eLight+(eMsg*MAXLIGHTS)].lights.minimum =
			(LIGHTVALUEMAX - gTriosLights[eLight+(eMsg*MAXLIGHTS)].lights.minimum) / 100;
			
			gTriosLights[eLight+(eMsg*MAXLIGHTS)].lights.maximum = 
			(LIGHTVALUEMAX - gTriosLights[eLight+(eMsg*MAXLIGHTS)].lights.maximum) / 100;
			
			gTriosLights[eLight+(eMsg*MAXLIGHTS)].lights.step = 
			(gTriosLights[eLight+(eMsg*MAXLIGHTS)].lights.step / LIGHTSTEPMAX)*100.0;
		}
	}
}

/****************************************************************************/

static void TriosCortexFromArray (void){
	int cortex;
	
	for (cortex = 0; cortex < AMOUNT_OF_CORTEXES; cortex++){
		gData.data[cortex].cortex = gTriosCortexes.cortexes[cortex];
	}
}

/****************************************************************************/

static void TriosCortextoArray (void){
	EMSG eMsg;
	
	for ( eMsg = eMSG1; eMsg < AMOUNT_OF_CORTEXES; eMsg++) {
		gTriosCortexes.cortexes[eMsg] = gData.data[eMsg].cortex;
	}
}

/****************************************************************************/

static void TriosInitBufferWithGet (void){
	TriosPrepareGetInMessage(eMSG1, eCORTEX1GETADR);
	TriosPrepareGetInMessage(eMSG2, eCORTEX2GETADR);
	TriosPrepareGetInMessage(eMSG3, eCORTEX3GETADR);
	TriosPrepareGetInMessage(eMSG4, eCORTEX4GETADR);
}

/****************************************************************************/

static void TriosInitBufferWithPost (void){
	TriosPreparePutInMessage(eMSG1,eCORTEX1PUTADR);
	TriosPreparePutInMessage(eMSG2,eCORTEX2PUTADR);
	TriosPreparePutInMessage(eMSG3,eCORTEX3PUTADR);
	TriosPreparePutInMessage(eMSG4,eCORTEX4PUTADR);
}

/****************************************************************************/
/*!
 Gets the buffer size
 @return the buffer size in bytes
 */
static int TriosGetBufferSize (void){
	return sizeof(gData);
}

/****************************************************************************/
/*!
 Clears the data buffer with zero
 */ 

static void TriosClearBuffer (void){
	memset(&gData,0,sizeof(gData));
}

/****************************************************************************/
/*!
 Gets the begin address of the buffer
 @return buffer address
 */

static pTTriosDataBuffer TriosGetBuffer (void){
	return &gData;
}

/****************************************************************************/
/*!
 Gets a pointer to the given msg index
 @param msg the message index @see EMSG
 @return the pointer to the message
 \note Using return pointer modifies buffer directly !
 */
static pTMessage TriosGetMessage (EMSG msg){
	return &gData.data[msg];
}

/****************************************************************************/
/*!
 Gets a pointer to the a light struct
 @param light The light index 
 @param msg The message index to look into 
 @return The pointer to the light struct
 \note Using return pointer modifies buffer directly !
 */
static pTLight TriosGetLightFromMessage (ELIGHTS light, EMSG msg){
	return &gData.data[msg].lights[light];
}

/****************************************************************************/

/*!
 Sets the light value pointed to by light index for a given message
 @param value the light value
 @param light the light index
 @param msg the message index
 */

static void TriosSetLightValueInMessage (cortexint value , ELIGHTS light, EMSG msg ){
	gData.data[msg].lights[light].value = value;
	/*
	if ( ( value > LIGHTVALUEMIN ) && ( value < LIGHTVALUEMAX )){
		gData.data[msg].lights[light].value = value;
	}
	*/
}

/****************************************************************************/
/*!
 Sends the buffer and receives the answer using TCP berkeley socket blocking
 @param ip ip address as C string
 @param port TCP port number
 @return	The errorcode using the system global errno value (errno.h) \n
 0 is OK (not defined in errno) @see TRIOS_ERROR_OK 
 \note	Buffer must have 1k byte size
 */

static int TriosTransmitBuffer (char * ip , int port){

	pUCTriosDataBuffer	pUCData;
	struct sockaddr_in	xSocketAddress;
	int					iClientSocket;
	int					iReallyReceived;
	int					iToReceive;
	int					iBufferSize;
	
	/* calculate only once buffer size */
	iBufferSize = TriosGetBufferSize();
	
	/* address spec given in by args */
	xSocketAddress.sin_family = AF_INET;
	xSocketAddress.sin_port = htons(port); /* must be arg */
	xSocketAddress.sin_addr.s_addr = inet_addr(ip); /* must be arg */
	
	/* realiable socket connection */
	iClientSocket = socket(PF_INET, SOCK_STREAM, 0);
	
	/* establisch connection with server end point */
	if ( connect( iClientSocket,(struct sockaddr *)&xSocketAddress, sizeof(xSocketAddress)) == -1 ){
		return errno;
	}
	
	if ( send(iClientSocket, &gData, iBufferSize, 0 ) == -1){
		return errno;
	}
	
	/* start of buffer to recv */
	pUCData = (pUCTriosDataBuffer)&gData;
	
	/* first try to recv */
	iReallyReceived = recv(iClientSocket, pUCData , iBufferSize, MSG_WAITALL);
	
	/* issue error already no need to continue */
	if (iReallyReceived == -1) {
		return errno;
	}
	
	/* prepare next write location */
	pUCData += iReallyReceived;
	
	/* calculate the recv bytes so far */
	iToReceive = iBufferSize - iReallyReceived;
	
	/* check if have a complete buffer continue to recv if needed */
	while ( iToReceive != 0 ) {
		
		/* get next chunk of bytes */
		iReallyReceived = recv(iClientSocket, pUCData , iToReceive , 0);
		
		/* issue error no need to continue so close socket */
		if (iReallyReceived == -1) {
			close(iClientSocket);
			return errno;
		}		
		
		/* prepare next position */
		pUCData += iReallyReceived;
		
		/* calculate the recv bytes so far */
		iToReceive -= iReallyReceived;
	}
	
	printf("%d\n",iReallyReceived);
	
	/* if reach this point all went well so close socket */
	close(iClientSocket);
	
	/* return OK */
	return TRIOS_ERROR_OK;
}

/****************************************************************************/

static void TriosAssignLightNames (void){
	int light;
	
	
	for (light = 0; light < AMOUNT_OF_CORTEXES * MAXLIGHTS; light++) {
		
		gTriosLights[light].name = (char*) gpCLightNames[light];
	}
}

/****************************************************************************/

/***************************PUBLIC FUNCTIONS*********************************/

void TriosSetEthernet (char * ip , int port){
	strcpy( gIpAddress , ip);
	gPort = port;
}

/****************************************************************************/

void TriosInitBuffer (void){
	TriosClearBuffer();
	TriosInitBufferWithGet();
	TriosAssignLightNames();
/*
	// Unit testing not used in release
	TriosSetLightValueInMessage(1000, eLIGHT1, eMSG1);
	TriosSetLightValueInMessage(2000, eLIGHT2, eMSG1);
	TriosSetLightValueInMessage(3000, eLIGHT3, eMSG1);
	TriosSetLightValueInMessage(4000, eLIGHT4, eMSG1);
	TriosSetLightValueInMessage(5000, eLIGHT5, eMSG1);
	TriosSetLightValueInMessage(6000, eLIGHT6, eMSG1);

	TriosSetLightValueInMessage(1000, eLIGHT1, eMSG2);
	TriosSetLightValueInMessage(2000, eLIGHT2, eMSG2);
	TriosSetLightValueInMessage(3000, eLIGHT3, eMSG2);
	TriosSetLightValueInMessage(4000, eLIGHT4, eMSG2);
	TriosSetLightValueInMessage(5000, eLIGHT5, eMSG2);
	TriosSetLightValueInMessage(6000, eLIGHT6, eMSG2);

	TriosSetLightValueInMessage(10000, eLIGHT1, eMSG3);
	TriosSetLightValueInMessage(0	, eLIGHT2, eMSG3);
	TriosSetLightValueInMessage(1111, eLIGHT3, eMSG3);
	TriosSetLightValueInMessage(9999, eLIGHT4, eMSG3);
	TriosSetLightValueInMessage(4125, eLIGHT5, eMSG3);
	TriosSetLightValueInMessage(1500, eLIGHT6, eMSG3);

	TriosSetLightValueInMessage(10000, eLIGHT1, eMSG4);
	TriosSetLightValueInMessage(0	, eLIGHT2, eMSG4);
	TriosSetLightValueInMessage(1111, eLIGHT3, eMSG4);
	TriosSetLightValueInMessage(9999, eLIGHT4, eMSG4);
	TriosSetLightValueInMessage(4125, eLIGHT5, eMSG4);
	TriosSetLightValueInMessage(1500, eLIGHT6, eMSG4);
*/
	
}

/****************************************************************************/

int TriosSendGetBuffer (void){
	int err;
	TriosInitBufferWithGet();
	err = TriosTransmitBuffer(gIpAddress, gPort);
	TriosLightToArray();
	TriosCortextoArray();
	return err;

}

/****************************************************************************/

int TriosSendPostBuffer (void){
	int err;
	TriosLightFromArray();
	TriosCortexFromArray();
	TriosInitBufferWithPost();
	err = TriosTransmitBuffer(gIpAddress, gPort);
	return err;
}

/****************************************************************************/

