/**
 * Copyright (c) 2000 - 2012 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Contact: Arijit Sen <arijit.sen@samsung.com>
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of SAMSUNG ELECTRONICS ("Confidential Information").
 * You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability
 * of the software, either express or implied, including but not
 * limited to the implied warranties of merchantability, fitness for a particular purpose, or non-infringement.
 * SAMSUNG shall not be liable for any damages suffered by licensee as
 * a result of using, modifying or distributing this software or its derivatives.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <glib.h>

#include "tcore.h"

#include "hal.h"
#include "plugin.h"
#include "user_request.h"
#include "server.h"
#include "mux.h"
#include "core_object.h"

/*For enabling MUX we need to set g_cmux_enable*/
int g_cmux_enable = 1;

/* Maximum Core objects per Logical HAL (indirectly per Channel) */
#define CMUX_MAX_CORE_OBJECTS		3

#define MAX_CMUX_BUFFER_SIZE		1024
#define MAX_CHANNEL_SUPPORTED		8


#define SABM  0x2F   
#define UA      0x63    
#define DM      0x0F    
#define DISC   0x43   
#define UIH     0xEF    
#define UI       0x03    



/* MUX CONTROL COMMANDS 
  * We are supporting only MSC command for phase 1
  */
#define  MSC	0xE3  	//Modem Status Command
#define  CLD  0xC3      //Multiplexer close down

/*                  
#define  PN	        	//parameter negotiation
#define  PSC      		//Power Saving Control
#define TEST     		//Test Command
#define  NSC   		//Non Supported Command Response
#define  RPN     		//Remote Port Negotiation Command
#define  RLS     		//Remote Line Status Command
#define  SNC   		//Service Negotiation Command  */



/* CMUX Channels [0-7] -
  * Channel 0 - Control Channel for CMUX
  * Channel 1 - CALL
  * Channel 2 - SIM 
  * Channel 3 - SAT
  * Channel 4 - SMS
  * Channel 5 - SS
  * Channel 6 - NETWORK
  * Channel 7 - MODEM & PS
  */
typedef enum CMUX_Channels {
	CMUX_CHANNEL_0,
	CMUX_CHANNEL_1,
	CMUX_CHANNEL_2,
	CMUX_CHANNEL_3,
	CMUX_CHANNEL_4,
	CMUX_CHANNEL_5,
	CMUX_CHANNEL_6,
	CMUX_CHANNEL_7
} CMUX_Channels;

typedef enum MuxChannelState {
	MUX_CHANNEL_CLOSED,
	MUX_CHANNEL_SABM_SEND_WAITING_FOR_UA,
	MUX_CHANNEL_ESTABLISHED,
	MUX_CHANNEL_UA_NOT_RECEIVED_RETRY,
	MUX_CHANNEL_DM_RECEIVED_CLOSING,
       MUX_CHANNEL_DM_SEND,
	MUX_CHANNEL_DISC_RECEIVED_CLOSING,
       MUX_CHANNEL_DISC_SEND_WAITING_FOR_UA,
       MUX_CHANNEL_UA_SEND_CLOSING,
	MUX_CHANNEL_UA_RECEIVED,
       MUX_CHANNEL_UA_SENDING,
	
} MuxChannelState;

typedef enum MuxState {
	MUX_NOT_INITIALIZED,
	MUX_INITIALIZED,
	MUX_CLOSED
} MuxState;

#define FIND_LENGTH(buf, hlen, Tlen)  do{ \
											if (*buf & 0x01) { \
												Tlen = *buf++ >>1; \
												hlen=6; \
												} \
												else { \
													Tlen = *(buf + 0x01) << 7; \
													Tlen = Tlen | (*buf & 0x7E) >> 1; \
													hlen = 7; \
													} \
										} while(0)

/*================= CRC TABLE=========================*/
const unsigned char crctable[256] = { //reversed, 8-bit, poly=0x07
	0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
	0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
	0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
	0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
	0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
	0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
	0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
	0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
	0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
	0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
	0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
	0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
	0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
	0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
	0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
	0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};
/*================= CRC TABLE=========================*/

typedef struct cmux_channel {   
	GSList *co; 
	TcoreHal *hal;
	
	MuxChannelState state; 
	CMUX_Channels ChannelId;
	int  FrameType;
	unsigned char  ExtBit;
	unsigned char  CRBit;
	unsigned char  pollFinalBit;

#if 0
	int  Len;

	/*Information field*/
	unsigned char *InfoField;

#endif

} CHANNEL;

typedef gboolean (*mux_cb_func)(CHANNEL *channel_ptr);

typedef struct cmux {
	MuxState state;

	CHANNEL *channelInfo[MAX_CHANNEL_SUPPORTED];
	int isWaiting;
	int pMsgLen;
	int curMBufLen;

	/* Plugin */
	TcorePlugin *p;
	
	/* Physical HAL */
	TcoreHal *phy_hal;

	/* Modem Core Object */
	CoreObject *modem_co;

	mux_cb_func  cb_func;

	/* Resolve Crash issue*/
	int  Len;
	/*Information field*/
	unsigned char *InfoField;

	unsigned char *muxTempBuffer;
	unsigned char *muxMainBuffer;
} MUX;

/* Global pointer MUX Object pointer */
MUX *MuxObjPtr = NULL;

/* Global buffers for CMUX processing */
//unsigned char Mux_buffer[MAX_CMUX_BUFFER_SIZE];
//unsigned char muxTempBuffer[MAX_CMUX_BUFFER_SIZE];
//unsigned char muxMainBuffer[MAX_CMUX_BUFFER_SIZE];

/* CMUX mode of operation */
int MUX_MODE = 0;	/* BASIC mode */

struct cmux_channel_object {
	char *channel_id_name;
	char *Cobject_name[CMUX_MAX_CORE_OBJECTS];
};

/* Core Object names need to be verified, define a MACRO globally */
struct cmux_channel_object cmux_channel_Cobject[] = {
	{"channel_0",	{"control", 		NULL, 		NULL}},   
	{"channel_1",	{"call",			NULL, 		NULL}},
	{"channel_2",	{"sim",			NULL, 		NULL}},
	{"channel_3",	{"sat",			NULL, 		NULL}},
	{"channel_4",	{"umts_sms",		NULL, 		NULL}},
	{"channel_5",	{"ss",			NULL, 		NULL}},
	{"channel_6",	{"umts_network",	NULL, 		NULL}},
	{"channel_7",	{"modem",		"umts_ps",	NULL}},
};

/* All the local functions declared below */
static unsigned char calc_crc(unsigned char *header , int length);
static int rcv_crc_check(unsigned char *data , unsigned char len,unsigned char RcvFCS);

MUX *tcore_cmux_new(void);
static void tcore_cmux_free();

void tcore_cmux_link_core_object_hal(CMUX_Channels channel_id, TcorePlugin *plugin);
static gboolean tcore_cmux_recv_mux_data(CHANNEL* channel_ptr);
static void tcore_cmux_process_rcv_frame( unsigned char *data, int len);
static void tcore_cmux_process_channel_data(CHANNEL *ChannelInfoPtr);
static void tcore_cmux_control_channel_handle();
static void tcore_cmux_flush_channel_data();

void tcore_cmux_channel_init(CMUX_Channels channel_id);
int tcore_cmux_close_channel(int channelID);
	
static unsigned char *tcore_encode_cmux_frame(unsigned char *data,
													int length,
													int channel_id,
													int FrameType,
													unsigned char EABit,
													unsigned char CRBit,
													unsigned char PFBit,
													int *out_data_len);

static TReturn tcore_cmux_hal_power(TcoreHal *h, gboolean flag)
{
	TcorePlugin *p = 0;
	struct custom_data *user_data = 0;
//	gboolean ret = 0;
	dbg("Entry");

	p = tcore_hal_ref_plugin(h);
	if (!p) {
		err("Plugin is undefined");
		return TCORE_RETURN_FAILURE;
	}

	user_data = tcore_hal_ref_user_data(h);
	if (!user_data) {
		err("User data is undefined");
		return TCORE_RETURN_FAILURE;
	}

	tcore_hal_set_power_state(h, TRUE);

	dbg("Exit");
	return TCORE_RETURN_SUCCESS;
}

static TReturn tcore_cmux_hal_send(TcoreHal *h, unsigned int data_len, void *data)
{
	unsigned char *send_data= NULL;
	char *channel_name = NULL;
	int channel_id = MAX_CHANNEL_SUPPORTED;
	int len = 0;
	int i = 0;

	int ret;
	dbg("Entry");
	dbg("data: %s", (char *)data);

	/* Check if Logical HAL is Powered ON */
	if (tcore_hal_get_power_state(h) == FALSE) {
		err("HAL is not Powered UP");
		return TCORE_RETURN_FAILURE;
	}

	channel_name = tcore_hal_get_name(h);
	dbg("HAL name: %s", channel_name)
	if(channel_name) {
		while(i < MAX_CHANNEL_SUPPORTED) {
			dbg("HAL name of Channels: %s",  cmux_channel_Cobject[i].channel_id_name);
			if (0 == strcmp((char *) cmux_channel_Cobject[i].channel_id_name, (char *)channel_name)) {
				channel_id = i;
				dbg("Found Channel ID: %d", channel_id);

				/* Free memory */
				free(channel_name);
				break;
			}
			i++;
		}
	}
	else {
		err("No name defined for HAL");
		return TCORE_RETURN_FAILURE;
	}

	if(channel_id > MAX_CHANNEL_SUPPORTED) {
		err("Failed to find Channel ID");
		return TCORE_RETURN_FAILURE;
	}
	
	/* Muxing operation and Frame creation */
	/* Encoding frame */
	send_data = tcore_encode_cmux_frame(data, data_len, channel_id, UIH, 1, 1, 0, &len);
	if(NULL == send_data) {
		err("Failed to encode");
		return TCORE_RETURN_FAILURE;
	}
	
	/* Directly send to Physical HAL */
	ret = tcore_hal_send_data(MuxObjPtr->phy_hal, len, send_data);
	
	dbg("Exit");
	return TCORE_RETURN_SUCCESS;
}

/* CMUX supported HAL (Logical HAL) operations */
static struct tcore_hal_operations mux_hops =
{
	.power = tcore_cmux_hal_power,
	.send = tcore_cmux_hal_send,
};

static gboolean tcore_cmux_recv_mux_data(CHANNEL *channel_ptr)
{
	TcoreHal *hal = NULL;
	dbg("Entry");

	/* Dereferencing HAL from Channel Pointer */
	hal = channel_ptr->hal;
	
	dbg("Dispatching to logical HAL - hal: %x", hal);
	tcore_hal_dispatch_response_data(hal, 0, MuxObjPtr->Len, MuxObjPtr->InfoField);

	dbg("Exit");
	return TRUE;
}

void tcore_cmux_link_core_object_hal(CMUX_Channels channel_id, TcorePlugin *plugin)
{
	TcoreHal *hal = NULL;
	CoreObject *co = NULL;
	int index;
	dbg("Entry");

	if(CMUX_CHANNEL_0 != channel_id) {
		dbg("Normal channel [%d]", channel_id);
		
		/* Creating Logical HAL for Core Object - Mode - 'AT mode' */
		hal = tcore_hal_new(plugin, cmux_channel_Cobject[channel_id].channel_id_name, &mux_hops, TCORE_HAL_MODE_AT);
		dbg("hal: %p", hal);
		
		/* Update Logical HAL of CMUX Channel */
		MuxObjPtr->channelInfo[channel_id]->hal = hal;
		
		index = 0;
		while (NULL != cmux_channel_Cobject[channel_id].Cobject_name[index]) {
			/* Retrieving Core Object */
			dbg("Core Object: '%s'", cmux_channel_Cobject[channel_id].Cobject_name[index]);
			co = tcore_plugin_ref_core_object(plugin, cmux_channel_Cobject[channel_id].Cobject_name[index]);
			dbg("co: %p", co);

			if (0 == strcmp((const char *)cmux_channel_Cobject[channel_id].Cobject_name[index], "modem")) {
				MuxObjPtr->modem_co = co;
				dbg("'modem' Core object reference is stored");
			}

			/* Set Logical HAL to Core objects */
			tcore_object_set_hal(co, hal);

			/* Update Core Object list of CMUX Channel */
			MuxObjPtr->channelInfo[channel_id]->co = g_slist_append(MuxObjPtr->channelInfo[channel_id]->co, co);

			index++;
		};
	}
	else {
		/* Control Channel */
		dbg("Control channel");
		
		/* Creating Logical HAL for Core Object - Mode - 'AT mode' */
		hal = tcore_hal_new(plugin, cmux_channel_Cobject[channel_id].channel_id_name, &mux_hops, TCORE_HAL_MODE_AT);
		dbg("hal: %p", hal);
		
		/* Update Logical HAL of CMUX Channel */
		MuxObjPtr->channelInfo[channel_id]->hal = hal;
	}

	/* Set Logical HAL Power State to TRUE */
	tcore_hal_set_power_state(hal, TRUE);
	dbg("HAL Power is SET");
	
	dbg("Exit");
	return;
}

/************************************************************

Here we initialize the MUX

************************************************************/
MUX *tcore_cmux_new(void)
{
	MUX *p = NULL;
	int i = 0;
	dbg("Entry");
	
	p = (MUX*)calloc(sizeof(MUX), 1);
	if (!p) {
		err("Failed to allocate memory");
		return NULL;
	}

	p->InfoField = (unsigned char *)calloc(MAX_CMUX_BUFFER_SIZE, 1);	
	if (!p->InfoField) {
		err("Failed to allocate memory for info field");
		goto ERROR;
	}
	
	p->muxTempBuffer = (unsigned char *)calloc(MAX_CMUX_BUFFER_SIZE, 1);	
	if (!p->InfoField) {
		err("Failed to allocate memory for info field");
		goto ERROR;
	}
	
	p->muxMainBuffer = (unsigned char *)calloc(MAX_CMUX_BUFFER_SIZE, 1);
	if (!p->InfoField) {
		err("Failedto allocate memory for info field");
		goto ERROR;
	}
	
	p->state = MUX_NOT_INITIALIZED;

	for(i = 0 ; i < MAX_CHANNEL_SUPPORTED ; i++) {
		p->channelInfo[i] = (CHANNEL*)calloc(sizeof(CHANNEL) , 1);
		/* Check for Memory allocation failure */
		if(!p->channelInfo[i]) {
			err("Failed to allocate memory for info field");
			goto ERROR;
		}
	}

	dbg("Exit");
	return p;

ERROR:
	/* Free allocated memory */
	if(p) {
		if(p->InfoField) {
			free(p->InfoField);
		}
		
		if(p->muxTempBuffer) {
			free(p->muxTempBuffer);
		}
		
		if(p->muxMainBuffer) {
			free(p->muxMainBuffer);
		}

		for(i = 0; i < MAX_CHANNEL_SUPPORTED ; i++) {
			if(p->channelInfo[i]) {
				free(p->channelInfo[i]);
			}
		}

		free(p);
	}

	err("Exit");
	return NULL;
}



/************************************************************
This function is used while encoding .
There is seperate function for decoding CRC
Frame Checking Sequence Field
************************************************************/
static unsigned char calc_crc (unsigned char *header , int length)
{
	unsigned char FCS=0xFF;	/*Init*/
	
	/*len is the number of bytes in the message, p points to message*/
	while (length--) {
		FCS = crctable[FCS ^ *header++];
	}
	
	/*Ones complement*/
	return (0xFF-FCS);
}

/************************************************************
Function name : tcore_encode_cmux_frame


************************************************************/
static unsigned char *tcore_encode_cmux_frame(unsigned char *data,
												int length,
												int channel_id,
												int FrameType,
												unsigned char EABit,
												unsigned char CRBit,
												unsigned char PFBit,
												int *out_data_len)
{
	int tLength=0, frameLen=0;
	int crc_len =0;
	dbg("Entry");


	/* Flush channel data */
	tcore_cmux_flush_channel_data();

	if(length > MAX_CMUX_BUFFER_SIZE ){
		err("Length - %d  exceeds the limit", length);
		return NULL;
		}
	//flag Octet
	MuxObjPtr->InfoField[tLength++] = (char)0xF9;

	if(0x00 == MUX_MODE) {		/* BASIC */
		/* EA: Extension Bit
		* C/R: Command Response
		*/
		MuxObjPtr->InfoField[tLength] = MuxObjPtr->InfoField[tLength] | ((EABit & 0x01) | ((CRBit << 1) & 0x02)); 

		/* DLCI: Data Link Connection Identifier */
		/* Check if the channel is within range */
		if(channel_id < MAX_CHANNEL_SUPPORTED && channel_id >=0) {
			dbg("Channel ID: %d", channel_id);
			MuxObjPtr->InfoField[tLength] = MuxObjPtr->InfoField[tLength]|((unsigned char) channel_id << 2);
		}
		else {
			err("Channel is out of range[0-8]");
			return NULL;
		}
		tLength++;

		/* Control Field
		  * The content of the control field defines the type of frame.
		  * ***************************************************************
		  * Frame Type										0 1 2 3  4   5 6 7
		  * SABM (Set Asynchronous Balanced Mode)				1 1 1 1 P/F 1 0 0
		  * UA (Unnumbered Acknowledgement)					1 1 0 0 P/F 1 1 0
		  * DM (Disconnected Mode)								1 1 1 1 P/F 0 0 0
		  * DISC (Disconnect)									1 1 0 0 P/F 0 1 0
		  * UIH (Unnumbered Information with Header check)			1 1 1 1 P/F 1 1 1
		  ****************************************************************/
		if(PFBit) {
			MuxObjPtr->InfoField[tLength++] = FrameType |0x10;
		}
		else {
			MuxObjPtr->InfoField[tLength++] = FrameType;
		}

		/* 5.2.1.5 Length Indicator */
		if (length < 128) {
			//length =(char)length <<1|0x01;
			MuxObjPtr->InfoField[tLength++] =(char)length <<1|0x01;
			
			/* CRC calculatable length */
			crc_len = 3;
		}
		else {
			MuxObjPtr->InfoField[tLength++] = (char)(length << 1);  //Need to change
			MuxObjPtr->InfoField[tLength++] = (char)(length >> 7);  //Need to change 

			/* CRC calculatable length */
			crc_len = 4;
		}

		/* We need to divide the frames into maximum frame length supported by IMC.
		* If IMC supports length according to 27.010 , we can send max of 16,384 bytes.
		* Need to discuss with IMC.
		*/

		/* 5.2.1.4 Information Field*/
		if(length >0) {
			dbg("MEMCPY :length= %d", length);
			memcpy((MuxObjPtr->InfoField + tLength),data, length);
			dbg("MEMCPY :DONE");
			tLength += length;
		}
		else {
			dbg("info field length is zero");
		}

		/*5.2.1.6 Frame Checking Sequence Field (FCS)*/
		MuxObjPtr->InfoField[tLength++] = calc_crc(MuxObjPtr->InfoField+1, crc_len);

		/*Flag Octet*/
		MuxObjPtr->InfoField[tLength++] = 0xF9;

		frameLen = tLength;
	}
	else if (0x01 == MUX_MODE) {			/* TBD MUX_MODE_ADVANCE */
		dbg("For PHASE 2 !!");
	}

	*out_data_len = frameLen;
	dbg("*out_data_len: %d", *out_data_len);

	dbg("Exit frameLen: %d", frameLen);
	return MuxObjPtr->InfoField;
}


static int rcv_crc_check(unsigned char *data , unsigned char len,unsigned char RcvFCS)
{
	unsigned char FCS = 0xFF;

	/* 'len' is the number of bytes in the message, 'p' points to message */
	while (len--) {
		//input =(char) *data++;
		FCS = crctable[FCS ^ *data++];
	}
	
	/*Ones complement*/
	FCS = crctable[FCS ^ RcvFCS];
	
	/* 0xCF is the reversed order of 11110011 */
	if (0xCF == FCS) {		/* FCS is OK */
		return 1;
	}
	else {				/* FCS is NOT OK */
		return 0;
	}
}

/************************************************************
Function name : tcore_cmux_flush_channel_data


************************************************************/
static void tcore_cmux_flush_channel_data()
{
	dbg("Entry");

	MuxObjPtr->Len =0x0;
	memset(MuxObjPtr->InfoField, 0x0, MAX_CMUX_BUFFER_SIZE);
	
	dbg("Exit");
	return;
}

/************************************************************
Function name : mux_control_chnl_handle


************************************************************/
static void tcore_cmux_control_channel_handle()
{
	unsigned char Cmdtype;
	int MsgLen =0;
	unsigned char *PtrMsgStart =NULL;
	dbg("Entry");
	
	/* 5.4.6.1 Message format
	  * All messages sent between the multiplexers conform to the following type, length, value format:
	  * Type Length Value 1 Value2  \85
	  */
	if (MuxObjPtr->Len > 0) {   
		PtrMsgStart = MuxObjPtr->InfoField ;
		Cmdtype = MuxObjPtr->InfoField[0];
		/* The EA bit is an extension bit. The EA bit is set to 1 in the last octet of the sequence; 
		  * in other octets EA is set to 0.
		  * 
		  * Search for the last octet
		  */
		while( (*PtrMsgStart++ & 0x01)) {	//TBD
			MsgLen ++;
		}
		
		if((Cmdtype & 0x02) == 0x02) {	//This is a command Request
			//Modem Status Command
			if(Cmdtype == MSC ) 	{     
				dbg("Modem Status Command");
			}
			else if(Cmdtype == CLD) {	//Modem Status Command
				tcore_cmux_close();
			}
#if 0		/* We will be supporting the commands in Phase 2 */
			else if(Cmdtype == PN) //Modem Status Command
			{
			}
#endif		/* We will be supporting the commands in Phase 2 */
		}
	}
	else {
		err("Frame length is less than ZERO");
	}

	dbg("Exit");
	return;
}

/************************************************************
Function name : tcore_cmux_process_channel_data

Process the channel data
************************************************************/
static void tcore_cmux_process_channel_data(CHANNEL *ChannelInfoPtr)
{
	int frameType;
	int ChannelId;
	int len;
	int ret;
	unsigned char * send_data=NULL;
	static int count = 0;
	dbg("Entry");

	ChannelId = ChannelInfoPtr->ChannelId;
	dbg("Channel ID: %d", ChannelId);
	
	ChannelInfoPtr = MuxObjPtr->channelInfo[ChannelId];

	frameType = ChannelInfoPtr->FrameType & 0xEF;
	dbg("frameType: 0x%x", frameType);
	
	switch(frameType)
	{
		case UI:
		case UIH:
		{
			dbg("Received UIH Frame");
			if(0 == ChannelId) {
				/*this is control info*/
				dbg("Control information");
				tcore_cmux_control_channel_handle();
			}
			else {
				//put in the logical HAL queue
				//this goes to the Cobject

				//ChannelInfoPtr->Len = ChannelInfoPtr->Len;
				//memcpy(ChannelInfoPtr->InfoField, ChannelInfoPtr->InfoField, ChannelInfoPtr->Len);
				
				tcore_cmux_recv_mux_data(ChannelInfoPtr);
			}
			break;
		}
		case UA:
		{
			dbg("Received UA Frame");
			dbg("ChannelInfoPtr->state: %d", ChannelInfoPtr->state);
			if(MUX_CHANNEL_SABM_SEND_WAITING_FOR_UA == ChannelInfoPtr->state) {
				ChannelInfoPtr->state = MUX_CHANNEL_ESTABLISHED;

				count++;
				dbg("Count: %d", count);
				if(MAX_CHANNEL_SUPPORTED == count) {
					/* Indicate to CoreObject */
					CoreObject *co = NULL;

					/* 'modem' Core Object */
					co = MuxObjPtr->modem_co;
					if(NULL == co) {
						err("'modem' Core object is not present");
						return;
					}

					/* Emit callback */
					dbg("Emit Core object callback");
					tcore_object_emit_callback(co, "CMUX-UP", NULL);
					dbg("Emitted Core object callback");
					count = 0;
				}
			}
			else if(MUX_CHANNEL_DISC_SEND_WAITING_FOR_UA == ChannelInfoPtr->state) {
				ChannelInfoPtr->state = MUX_CHANNEL_CLOSED;

				if(0 == ChannelId) {
					MuxObjPtr->state = MUX_CLOSED;
					tcore_cmux_close();
				}
			}
			else  {
				err("Received UA in wrong state!!!");
			}
			break;
		}
		case DM:
		{
			/* 5.4.1 DLC Establishment : If the responding station is not ready or unwilling
			  * to establish the particular DLC it will reply with a DM frame with the
			  * F-bit set to 1.
			  */
			dbg("Received DM Frame");
			if((MUX_CHANNEL_ESTABLISHED == ChannelInfoPtr->state)
				|| (MUX_CHANNEL_SABM_SEND_WAITING_FOR_UA == ChannelInfoPtr->state)) {
					ChannelInfoPtr->state = MUX_CHANNEL_CLOSED;
			}
			tcore_cmux_flush_channel_data();

			break;
		}
		case DISC:
		{
			dbg("Received DISC Frame");
			if(0 == ChannelInfoPtr->pollFinalBit) {
				/* In the case where a SABM or DISC command with the P bit set to 0 is received then 
				  * the received frame shall be discarded.
				  */
				tcore_cmux_flush_channel_data();
			}
			else {
					if(MUX_CHANNEL_CLOSED == ChannelInfoPtr->state) {
					//If a DISC command is received while in disconnected mode a DM response should be sent
					/* Encoding frame */
					send_data = tcore_encode_cmux_frame(NULL, 0, ChannelId, DM, 1, 1, 1, &len);
			
					}
					else {   //send Unnumbered Acknowledgement
					send_data =	tcore_encode_cmux_frame(NULL,0, ChannelId,  UA, 1, 1, 1,&len);
					}

					if(0 == len) {
					err("Failed to encode");
					return;
						}
					/* Directly send to Physical HAL */
					ret = tcore_hal_send_data(MuxObjPtr->phy_hal,len, send_data);
					
					/*5.3.4 Disconnect (DISC) command: DISC command sent at DLCI 0 have the same 
					meaning as the Multiplexer Close Down command */
					tcore_cmux_flush_channel_data();        
					if(0 == ChannelId) {
						MuxObjPtr->state =MUX_CLOSED;
						tcore_cmux_close();
					}
			}
			break;
		}	
		case SABM:
		{
			dbg("Received SABM Frame");
			if(0 == ChannelInfoPtr->pollFinalBit) {
				/*In the case where a SABM or DISC command with the P bit set to 0 is received then 
				the received frame shall be discarded. */
				//flush_rcv_data();
				tcore_cmux_flush_channel_data();
			}
			else {
				send_data = tcore_encode_cmux_frame(NULL,0, ChannelId,  UA, 1, 1, 1,&len); 
				/* Directly send to Physical HAL */
					ret = tcore_hal_send_data(MuxObjPtr->phy_hal, len,send_data);

				if(ChannelInfoPtr->state != MUX_CHANNEL_ESTABLISHED) {
					ChannelInfoPtr->state = MUX_CHANNEL_ESTABLISHED;
				}
			} 
			break;
		}
	}

	return ;
}

/**********************************************

Function name :tcore_cmux_rcv_from_hal

This should be a callback from Physical HAL.
DATA Received from CP side.

***********************************************/
static void tcore_cmux_process_rcv_frame( unsigned char *data, int len)
{
	unsigned char *PtrFrameProcess =data;
	unsigned char *PtrBufStart = data;

	CHANNEL *ch =NULL;
	unsigned char ChannelId;
	int hlen;
	dbg("Entry");

	tcore_cmux_flush_channel_data();

	/* Get the Channel ID : 1st byte will be flag (F9)..Flag checking is already done.*/
	if((ChannelId = ((*++PtrFrameProcess  >> 2)& 0x3F)) < MAX_CHANNEL_SUPPORTED) {	 //max channel is 8

		ch = MuxObjPtr->channelInfo[ChannelId];

		ch->ChannelId = ChannelId;

		//get the EA bit
		ch->ExtBit = *PtrFrameProcess  & 0x01;

		//get the CR bit
		ch->CRBit = (*PtrFrameProcess ++ >> 1) & 0x01;

		//get the Frame Type
		ch->FrameType = *PtrFrameProcess  ++;

		//get the poll/Final bit
		ch->pollFinalBit = (ch->FrameType & 0x10)>> 4;

		//get the length . TBD
		if(*PtrFrameProcess  & 0x01) {			//if len <127
			MuxObjPtr->Len = *PtrFrameProcess ++ >> 1;
			hlen = 3;
		}
		else {
			MuxObjPtr->Len = *(PtrFrameProcess +1) << 7;
			MuxObjPtr->Len = MuxObjPtr->Len | ((* PtrFrameProcess ++ & 0xFE) >> 1);
			hlen = 4;
			PtrFrameProcess++;
		}
		
		/* Copy received information field */
		dbg("MEMCPY :length= %d", MuxObjPtr->Len);
		memcpy(MuxObjPtr->InfoField, PtrFrameProcess, MuxObjPtr->Len);
		dbg("MEMCPY :Done");
		PtrFrameProcess = PtrFrameProcess + MuxObjPtr->Len;

		//CRC check of the header
		if (rcv_crc_check(PtrBufStart+1, hlen, *PtrFrameProcess)) {

			dbg("Calling tcore_cmux_process_channel_data");
			tcore_cmux_process_channel_data(ch);
		}
		else {
			err("CRC check of the header FAILED.. Drop the packet !!");
		}
	}	
	else {
		err("Incorrect channel... Drop the packet !!");
	}

	dbg("Exit");
	return;
}

/**********************************************

Function name :tcore_cmux_rcv_from_hal

This should be a callback from Physical HAL.
DATA Received from CP side.

***********************************************/
int tcore_cmux_rcv_from_hal(unsigned char *data, int len)
{
	unsigned char *buf =data;
	int count =0, i =0;
	int alen =0; //actual length
	
	unsigned char *muxTempBuffer = MuxObjPtr->muxTempBuffer;
	unsigned char *muxMainBuffer = MuxObjPtr->muxMainBuffer;

	int Tlen = 0;			/* Total length 		  */
	int hlen = 0;			/* Header length 		  */
	int currentFlen = 0; 	/* Current Frame length    */	
	int pLen = 0;  		/* pending length in array */ 
	
	dbg("Entry");

#if 1
	//testing
	dbg("len: %d ", len);
	{
		int i=0;
		for(i = 0; i < len ; i++) {
			msg("%02x", data[i]);
		}
	}
#endif

	if (len > MAX_CMUX_BUFFER_SIZE){
		err("len - %d is more than MAX limit !!" ,len);
		return 0;
		}
		

	if( (len > 0 ) && (data != NULL)) {
		if(MuxObjPtr->isWaiting) {
			//* Checking if Frame fully contains F9 or not. If only F9,then reject, esle process.
			while(*buf++ == 0xF9) {
				count++;
			}

			if (count == len) {	// In case all are F9 
				err("Not a valid frame");
				return 0;
			}
			dbg("MEMCPY :MuxObjPtr->curMBufLen: %d & len = %d", MuxObjPtr->curMBufLen,len);
			memcpy((muxMainBuffer + MuxObjPtr->curMBufLen),data,len);
			dbg("MEMCPY :DONE");
			MuxObjPtr->curMBufLen = MuxObjPtr->curMBufLen + len;

			if(MuxObjPtr->pMsgLen <= len) {
				MuxObjPtr->isWaiting =0;
				tcore_cmux_rcv_from_hal(muxMainBuffer,MuxObjPtr->curMBufLen);
			}
			else {
				MuxObjPtr->pMsgLen = MuxObjPtr->pMsgLen - len;
				dbg("Still waiting for the complete frame!!!");
			}
		}
		else {
			if(*buf ==0xF9) {	// start flag present
				//remove extra flags
				while(*++buf == 0xF9) {
					count++;
				}

				//check if received data has atleast three octets between flags
				for(i = 0; ( (i<3) && (*(buf+i)!=0xF9)); i++);

				if((i<3) || (count == (len-1))) {	// In case all are F9 or three octets between falgs not found
					err("Not a valid frame");
					return 0;
				}

				alen = len - count;
				memset(muxTempBuffer, 0x0, MAX_CMUX_BUFFER_SIZE);
				dbg("MEMCPY :alen= %d", alen);
				memcpy(muxTempBuffer, data + count, alen);
				dbg("MEMCPY :DONE");
				buf = muxTempBuffer + 3;

				//find the info length field of the current frame
				FIND_LENGTH(buf, hlen, Tlen);

				//calculate the current frame length including the headers
				currentFlen = hlen +Tlen;
				memset(muxMainBuffer, 0x0, MAX_CMUX_BUFFER_SIZE);

				if(currentFlen==alen) {	//Only 1 frame is received and its a complete frame
					if(muxTempBuffer[currentFlen-0x01] == 0xF9) {
						dbg("MEMCPY :currentFlen= %d", currentFlen);
						memcpy(muxMainBuffer,muxTempBuffer,currentFlen);
						dbg("MEMCPY :DONE");
						tcore_cmux_process_rcv_frame(muxMainBuffer ,currentFlen);
						//initialze the main buffer
						memset(muxMainBuffer, 0x0, MAX_CMUX_BUFFER_SIZE);
					}
					else {
						err("End flag missing!!!");
						return 0;
					}
				}
				else if(currentFlen < alen) {		//more than one frame
					if(muxTempBuffer[currentFlen-0x01] == 0xF9) {
						dbg("MEMCPY :currentFlen= %d", currentFlen);
						memcpy(muxMainBuffer,muxTempBuffer,currentFlen);
						dbg("MEMCPY :DONE");
						tcore_cmux_process_rcv_frame(muxMainBuffer ,currentFlen);

						//initialze the main buffer
						memset(muxMainBuffer, 0x0, MAX_CMUX_BUFFER_SIZE);

						muxMainBuffer[0]=0xF9;
						pLen = alen - currentFlen;
						dbg("MEMCPY :pLen= %d", pLen);
						memcpy((muxMainBuffer + 0x01), (muxTempBuffer + currentFlen), pLen);
						dbg("MEMCPY :DONE");
						tcore_cmux_rcv_from_hal( muxMainBuffer,pLen + 0x01);
					}
					else {
						err("End flag missing!!!");
						return 0;
					}
				}
				else if(currentFlen > alen) {//the frame is incomplete.Yet to receive the full frame
					MuxObjPtr->isWaiting = 1;
					dbg("MEMCPY :alen= %d", alen);
					memcpy(muxMainBuffer, muxTempBuffer + count, alen);
					dbg("MEMCPY :DONE");
					MuxObjPtr->pMsgLen =  currentFlen - alen;
					MuxObjPtr->curMBufLen = alen ;
				}
			}
			else {
				err("Start flag missing!!!");
			}
		}
	} 
	else	{
		err("Incorrect Length!!! or NULL Data!!");
	}

	dbg("Exit");
	return 1;
}

#if 1
void tcore_cmux_channel_init(CMUX_Channels channel_id)
{
	CHANNEL *ch = NULL;
	dbg("Entry");
	
	ch = MuxObjPtr->channelInfo[channel_id];
	memset(ch, 0x0, sizeof(CHANNEL));
	
	ch->ChannelId = channel_id;
	ch->state = MUX_CHANNEL_SABM_SEND_WAITING_FOR_UA;

	ch->co = NULL;
	ch->hal = NULL;

	/* TODO - Check if required */
    	ch->FrameType = SABM;
	ch->ExtBit = 0x01;
	ch->CRBit = 0x01;
	ch->pollFinalBit = 0x01;

	dbg("Channel ID %d initialized", channel_id);
	
	dbg("Exit");
	return;
}
#else
/*********************************************************

OPEN CMUX CHANNEL
*********************************************************/
int open_cmux_channel(int channelID)
{
	CHANNEL *ch;
	unsigned char *send_data;
	int ret;
	int len;
	dbg("Entry");

	ch = MuxObjPtr->channelInfo[channelID];

	if(ch->state == MUX_CHANNEL_CLOSED) {
		ch->FrameType = SABM;
		ch->ExtBit = 0x01;
		ch->CRBit = 0x01;

		send_data = tcore_encode_cmux_frame(NULL, 0, channelID,  SABM, 0x01, 0x01, 0x01, &len); 
		
		/* Directly send to Physical HAL */
		ret = tcore_hal_send_data(MuxObjPtr->phy_hal,len, send_data);
	}
	else {
		//channel is already initialized
		err("Channel is already initialized");
	}

	dbg("Exit");
	return 1;
 }
#endif

/*********************************************************
			CLOSE CMUX CHANNEL
*********************************************************/
int tcore_cmux_close_channel(int channelID)
{
	CHANNEL *ch = NULL;
	unsigned char *send_data = NULL;
	int ret, len;
	dbg("Entry");

	ch = MuxObjPtr->channelInfo[channelID];
	
	if(ch->state != MUX_CHANNEL_CLOSED) {
		ch->FrameType = DISC;
		ch->ExtBit = 0x01;
		ch->CRBit = 0x01;
		ch->state = MUX_CHANNEL_DISC_SEND_WAITING_FOR_UA;

		/* Send DSC command */
		send_data = tcore_encode_cmux_frame(NULL,0, channelID,  DISC, 0x01, 0x01, 0x01, &len); 
		
		/* Directly send to Physical HAL */
		ret = tcore_hal_send_data(MuxObjPtr->phy_hal, len, send_data);

	}
	else {
		/* Channel is already closed */
		err("Channel is already closed");
	}

	dbg("Exit");
	return 1;
}

static void tcore_cmux_free()
{
	int channel;
	dbg("Entry");
	
	if(MuxObjPtr) {

		/* Free Information Field */
		if (MuxObjPtr->InfoField) 
			free(MuxObjPtr->InfoField);
				
		for (channel = 0 ; channel < MAX_CHANNEL_SUPPORTED ; channel++)
		{
			/* Free Channel Information */
			if(MuxObjPtr->channelInfo[channel]) {
				free(MuxObjPtr->channelInfo[channel]);
			}
		}

		/* Free MUX Object */
		free(MuxObjPtr);
	}
	else {
		err("MUX Object doesn't exist");
	}

	dbg("Exit");
	return;
}

void tcore_cmux_init(TcorePlugin *plugin, TcoreHal *h)
{
	TcoreHal *physical_hal = NULL;
	unsigned char *data = NULL;
	int data_len = 0;

	int index;
	dbg("Entry");

	physical_hal = (TcoreHal *)h;
	dbg("Physical HAL: %x", physical_hal);

	/* Creat new CMUX Object */
	MuxObjPtr = tcore_cmux_new();
	if(NULL == MuxObjPtr) {
		err("Failed to create MUX object");
		return;
	}

	/* Save Plugin */
	MuxObjPtr->p = plugin;
	
	/* Save Physical HAL */
	MuxObjPtr->phy_hal = physical_hal;

	/* Setting Receive callback function for data received from Physical HAL */
	MuxObjPtr->cb_func= tcore_cmux_recv_mux_data;
	
      /* After MUX setup, AT parse functionality of PHY HAL should be disabled,
        * here we change the mode of PHYSICAL HAL to Custom.
        */
	tcore_hal_set_mode(MuxObjPtr->phy_hal , TCORE_HAL_MODE_TRANSPARENT);
	dbg("Physical HAL mode changed to Transparent");
 
	/* Initialize all the Channels */
	/* Open all Channels */
	for(index = 0 ; index < MAX_CHANNEL_SUPPORTED ; index++) {

		dbg("Initialize the Channel %d", index); 
		tcore_cmux_channel_init((CMUX_Channels)index);
		
		dbg("Opening Channel %d", index);
		/* Encode CMUX Frame */
		data = tcore_encode_cmux_frame(NULL, 0, index, SABM, 0x01, 0x01, 0x01, &data_len);
		if(NULL == data) {
			err("Failed to encode");
			return;
		}
		dbg("data_len: %d data: %s", data_len, data);

#if 1
		/* For Testing */
		{
			int i;
			
			for(i = 0; i < data_len ; i++) {
				msg("%02x", data[i]);
			}
		}
#endif
		/* Send Frame to CP */
		tcore_hal_send_data(physical_hal, data_len, data);
		dbg("CMUX Control Request sent to CP");

		/* Set Core object and HAL */
		tcore_cmux_link_core_object_hal((CMUX_Channels)index, plugin);
	}

	dbg("Exit");
	return;
}

/*********************************************************

MUX Close down

*********************************************************/
void tcore_cmux_close()
{
	int channel = 0;
	int index = 0;
	CoreObject *co = NULL;
	GSList *co_list = NULL;
	dbg("Entry");

	for (channel = 0; channel < MAX_CHANNEL_SUPPORTED ; channel++) {
		dbg("Channel ID: %d", channel);
		index = 0;

		/* Close Channel - Send DSC command */
		tcore_cmux_close_channel(channel);

		/* Revert Physical HAL as HAL of each Core Object associated to this Channel */
		while (NULL != cmux_channel_Cobject[channel].Cobject_name[index]) {
			co = NULL;

			/* Core Objects list */
			co_list = MuxObjPtr->channelInfo[channel]->co;
			dbg("Core Objects list : %p", co_list);
			
			/* Core Object list may contain multiple Core Objects.
			  * Revert to Physical HAL for each of the Core Objects associated
			  * with this Channel
			  */
			while(NULL != co_list) {
				if(NULL != co_list->data) {
					if(!strcmp((const char *)cmux_channel_Cobject[channel].Cobject_name[index], (const char *)tcore_object_ref_name((CoreObject *)co_list->data))) {
						co = (CoreObject *)co_list->data;
						dbg("Found the Core Object");
						break;
					}
				}

				/* To next Core Object in the list */
				co_list = co_list->next;
			}

			/* Set the previous Physical HAL as HAL for Core Object */
			if(NULL != co) {
				tcore_object_set_hal(co, MuxObjPtr->phy_hal);
			}
			else {
				/* Proceed to next Channel */
				err("No more Core Objects present in this Channel");
				break;
			}

			/* To next Core Object */
			index++;
		}
		
		/* Free Logical HAL for Channel */
		tcore_hal_free(MuxObjPtr->channelInfo[channel]->hal);
	}
	
	/* Change the mode of PHYSICAL HAL to Custom */
	tcore_hal_set_mode(MuxObjPtr->phy_hal, TCORE_HAL_MODE_CUSTOM);

	/* Free all the allocated memory */
	tcore_cmux_free();
		
	dbg("Exit");
	return;
}
