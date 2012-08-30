/*
 * libtcore
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 * Contact: Arijit Sen <arijit.sen@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

/* Maximum Core objects per Logical HAL (indirectly per Channel) */
#define MAX_CMUX_CORE_OBJECTS		3

/* Max CMUX Buffer size */
#define MAX_CMUX_BUFFER_SIZE		2000

/* Max muber of CMUX Channels */
#define MAX_CMUX_CHANNELS_SUPPORTED	8

/* CMUX Commands */
#define CMUX_COMMAND_SABM		0x2F
#define CMUX_COMMAND_UA			0x63
#define CMUX_COMMAND_DM			0x0F
#define CMUX_COMMAND_DISC			0x43
#define CMUX_COMMAND_UIH			0xEF
#define CMUX_COMMAND_UI			0x03

/* MUX CONTROL COMMANDS 
  * We are supporting only MSC command for phase 1
  */
#define  CMUX_COMMAND_MSC			0xE3	//Modem Status Command
#define  CMUX_COMMAND_CLD			0xC3	//Multiplexer close down
#if 0	/* Support in Phase 2 */
#define CMUX_COMMAND_PN	        	//Parameter Negotiation
#define CMUX_COMMAND_PSC      		//Power Saving Control
#define CMUX_COMMAND_TEST     		//Test Command
#define CMUX_COMMAND_NSC   		//Non Supported Command Response
#define CMUX_COMMAND_RPN     		//Remote Port Negotiation Command
#define CMUX_COMMAND_RLS     		//Remote Line Status Command
#define CMUX_COMMAND_SNC   		//Service Negotiation Command
#endif	/* Support in Phase 2 */

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

/* MUX Channel States */
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

/* MUX State */
typedef enum MuxState {
	MUX_NOT_INITIALIZED,
	MUX_INITIALIZED,
	MUX_CLOSED
} MuxState;

#define FIND_LENGTH(buf, header_length, total_length)  do{ \
											if (*buf & 0x01) { \
												total_length = *buf++ >>1; \
												header_length=6; \
												} \
												else { \
													total_length = *(buf + 0x01) << 7; \
													total_length = total_length | (*buf & 0xFE) >> 1; \
													header_length = 7; \
													} \
										} while(0)

/*================= CRC TABLE=========================*/
const unsigned char crc_table[256] = { //reversed, 8-bit, poly=0x07
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

/* CMUX Channel */
typedef struct cmux_channel {   
	GSList *co; 
	TcoreHal *hal;
	
	MuxChannelState state; 
	CMUX_Channels channel_id;
	int  frame_type;
	unsigned char  ext_bit;
	unsigned char  cr_bit;
	unsigned char  poll_final_bit;
} CHANNEL;

/* CMUX callback prototype */
typedef gboolean (*mux_cb_func)(CHANNEL *channel_ptr);

/* CMUX structure */
typedef struct cmux {
	MuxState state;

	CHANNEL *channel_info[MAX_CMUX_CHANNELS_SUPPORTED];
	int is_waiting;
	int msg_len;
	int cur_main_buf_len;

	/* Plugin */
	TcorePlugin *plugin;
	
	/* Physical HAL */
	TcoreHal *phy_hal;

	/* Modem Core Object */
	CoreObject *modem_co;

	mux_cb_func  cb_func;

	/* Resolve Crash issue*/
	int  info_field_len;
	/*Information field*/
	unsigned char *info_field;

	unsigned char *mux_temp_buffer;
	unsigned char *mux_main_buffer;
} MUX;

/* Global pointer MUX Object pointer */
MUX *g_mux_obj_ptr = NULL;

/* CMUX mode of operation */
int g_mux_mode = 0;	/* BASIC mode */

struct cmux_channel_object {
	char *channel_id_name;
	char *core_object_name[MAX_CMUX_CORE_OBJECTS];
};

/* Core Object names need to be verified, define a MACRO globally */
struct cmux_channel_object cmux_channel_core_object[] = {
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
static int rcv_crc_check(unsigned char *data, unsigned char len,unsigned char rcv_FCS);

MUX *tcore_cmux_new(void);
static void tcore_cmux_free(void);

void tcore_cmux_link_core_object_hal(CMUX_Channels channel_id, TcorePlugin *plugin);
static gboolean tcore_cmux_recv_mux_data(CHANNEL* channel_ptr);
static void tcore_cmux_process_rcv_frame( unsigned char *data, int len);
static void tcore_cmux_process_channel_data(CHANNEL *channel_info_ptr);
static void tcore_cmux_control_channel_handle(void);
static void tcore_cmux_flush_channel_data(void);

static void tcore_cmux_channel_init(CMUX_Channels channel_id);
static void tcore_cmux_close_channel(int channel_id);
	
static unsigned char *tcore_encode_cmux_frame(unsigned char *data,
													int length,
													int channel_id,
													int frame_type,
													unsigned char EA_bit,
													unsigned char CR_bit,
													unsigned char PF_bit,
													int *out_data_len);

static TReturn tcore_cmux_send_data(int data_len, unsigned char *data);

static TReturn tcore_cmux_hal_power(TcoreHal *h, gboolean flag)
{
	TcorePlugin *p = NULL;
	struct custom_data *user_data = NULL;
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
	int channel_id = MAX_CMUX_CHANNELS_SUPPORTED;
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
		while(i < MAX_CMUX_CHANNELS_SUPPORTED) {
			dbg("HAL name of Channels: %s",  cmux_channel_core_object[i].channel_id_name);
			if (0 == strcmp((char *)cmux_channel_core_object[i].channel_id_name, (char *)channel_name)) {
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

	if(channel_id > MAX_CMUX_CHANNELS_SUPPORTED) {
		err("Failed to find Channel ID");
		return TCORE_RETURN_FAILURE;
	}
	
	/* Muxing operation and Frame creation */
	/* Encoding frame */
	send_data = tcore_encode_cmux_frame(data, data_len, channel_id, CMUX_COMMAND_UIH, 1, 1, 0, &len);
	if(0 == len) {
		err("Failed to encode");
		return TCORE_RETURN_FAILURE;
	}
	
	/* Send CMUX data */
	ret = tcore_cmux_send_data(len, send_data);
	
	dbg("Exit");
	return TCORE_RETURN_SUCCESS;
}

/* CMUX supported HAL (Logical HAL) operations */
static struct tcore_hal_operations mux_hops =
{
	.power = tcore_cmux_hal_power,
	.send = tcore_cmux_hal_send,
};

static TReturn tcore_cmux_send_data(int data_len, unsigned char *data)
{
	TReturn ret = TCORE_RETURN_SUCCESS;
	dbg("Entry");

	/* Directly send to Physical HAL */
	ret = tcore_hal_send_data(g_mux_obj_ptr->phy_hal, data_len, (void *)data);
	if(TCORE_RETURN_SUCCESS != ret) {
		err("Failed to send CMUX data");
	}
	else{
		dbg("Successfully sent CMUX data");
	}

	dbg("Exit");
	return ret;
}
static gboolean tcore_cmux_recv_mux_data(CHANNEL *channel_ptr)
{
	TcoreHal *hal = NULL;
	dbg("Entry");

	/* Dereferencing HAL from Channel Pointer */
	hal = channel_ptr->hal;
	
	dbg("Dispatching to logical HAL - hal: %x", hal);
	tcore_hal_dispatch_response_data(hal, 0, g_mux_obj_ptr->info_field_len, g_mux_obj_ptr->info_field);

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
		hal = tcore_hal_new(plugin, cmux_channel_core_object[channel_id].channel_id_name, &mux_hops, TCORE_HAL_MODE_AT);
		dbg("hal: %p", hal);
		
		/* Update Logical HAL of CMUX Channel */
		g_mux_obj_ptr->channel_info[channel_id]->hal = hal;
		
		index = 0;
		while (NULL != cmux_channel_core_object[channel_id].core_object_name[index]) {
			/* Retrieving Core Object */
			dbg("Core Object: '%s'", cmux_channel_core_object[channel_id].core_object_name[index]);
			co = tcore_plugin_ref_core_object(plugin, cmux_channel_core_object[channel_id].core_object_name[index]);
			dbg("co: %p", co);

			if (0 == strcmp((const char *)cmux_channel_core_object[channel_id].core_object_name[index], "modem")) {
				g_mux_obj_ptr->modem_co = co;
				dbg("'modem' Core object reference is stored");
			}

			/* Set Logical HAL to Core objects */
			tcore_object_set_hal(co, hal);

			/* Update Core Object list of CMUX Channel */
			g_mux_obj_ptr->channel_info[channel_id]->co = g_slist_append(g_mux_obj_ptr->channel_info[channel_id]->co, co);

			/* Next Core Object of the channel */
			index++;
		};
	}
	else {
		/* Control Channel */
		dbg("Control channel");
		
		/* Creating Logical HAL for Core Object - Mode - 'AT mode' */
		hal = tcore_hal_new(plugin, cmux_channel_core_object[channel_id].channel_id_name, &mux_hops, TCORE_HAL_MODE_AT);
		dbg("hal: %p", hal);
		
		/* Update Logical HAL of CMUX Channel */
		g_mux_obj_ptr->channel_info[channel_id]->hal = hal;
	}

	/* Set Logical HAL Power State to TRUE */
	tcore_hal_set_power_state(hal, TRUE);
	dbg("HAL Power is SET");
	
	dbg("Exit");
	return;
}

MUX *tcore_cmux_new(void)
{
	MUX *mux = NULL;
	int i = 0;
	dbg("Entry");
	
	/* Allocating memory for mux */
	mux = (MUX*)calloc(sizeof(MUX), 1);
	if (!mux) {
		err("Failed to allocate memory");
		return NULL;
	}

	/* Allocating memory for info_field */
	mux->info_field = (unsigned char *)calloc(MAX_CMUX_BUFFER_SIZE, 1);	
	if (!mux->info_field) {
		err("Failed to allocate memory for info field");
		goto ERROR;
	}
	
	/* Allocating memory for mux_temp_buffer */
	mux->mux_temp_buffer = (unsigned char *)calloc(MAX_CMUX_BUFFER_SIZE, 1);	
	if (!mux->mux_temp_buffer) {
		err("Failed to allocate memory for info field");
		goto ERROR;
	}
	
	/* Allocating memory for mux_main_buffer */
	mux->mux_main_buffer = (unsigned char *)calloc(MAX_CMUX_BUFFER_SIZE, 1);
	if (!mux->mux_main_buffer) {
		err("Failedto allocate memory for info field");
		goto ERROR;
	}
	
	/* MUX State initialize to MUX_NOT_INITIALIZED */
	mux->state = MUX_NOT_INITIALIZED;

	/* Allocating memory for channel_info */
	for(i = 0 ; i < MAX_CMUX_CHANNELS_SUPPORTED ; i++) {
		mux->channel_info[i] = (CHANNEL*)calloc(sizeof(CHANNEL), 1);
		/* Check for Memory allocation failure */
		if(!mux->channel_info[i]) {
			err("Failed to allocate memory for channel_info of channel: %d", i);
			goto ERROR;
		}
	}

	dbg("Exit");
	return mux;

ERROR:
	/* Free allocated memory */
	if(mux) {
		if(mux->info_field) {
			free(mux->info_field);
		}
		
		if(mux->mux_temp_buffer) {
			free(mux->mux_temp_buffer);
		}
		
		if(mux->mux_main_buffer) {
			free(mux->mux_main_buffer);
		}

		for(i = 0; i < MAX_CMUX_CHANNELS_SUPPORTED ; i++) {
			if(mux->channel_info[i]) {
				free(mux->channel_info[i]);
			}
		}

		free(mux);
	}

	err("Exit");
	return NULL;
}

static unsigned char calc_crc (unsigned char *header , int length)
{
	unsigned char FCS=0xFF;	/*Init*/
	unsigned char crc = 0x00;
	dbg("Entry");
	
	/* 'length' is the number of bytes in the message, 'header' points to message */
	while (length--) {
		FCS = crc_table[FCS ^ *header++];
	}
	
	/*Ones complement*/
	crc = (0xFF-FCS);

	dbg("Exit - crc: 0x%02x", crc)
	return crc;
}

static unsigned char *tcore_encode_cmux_frame(unsigned char *data,
												int length,
												int channel_id,
												int frame_type,
												unsigned char EA_bit,
												unsigned char CR_bit,
												unsigned char PF_bit,
												int *out_data_len)
{
	int frame_length = 0;
	int total_frame_length = 0;
	int crc_len =0;
	dbg("Entry");

	/* Flush channel data */
	tcore_cmux_flush_channel_data();

	if(length > MAX_CMUX_BUFFER_SIZE ){
		err("Length - %d  exceeds the limit", length);
		return NULL;
		}

	/* Flag Octet */
	g_mux_obj_ptr->info_field[frame_length++] = (char)0xF9;

	/* Mode of Operation */
	if(0x00 == g_mux_mode) {				/* BASIC */
		/* EA: Extension Bit
		* C/R: Command Response
		*/
		g_mux_obj_ptr->info_field[frame_length] = g_mux_obj_ptr->info_field[frame_length] | ((EA_bit & 0x01) | ((CR_bit << 1) & 0x02)); 

		/* DLCI: Data Link Connection Identifier */
		/* Check if the channel is within range */
		if(channel_id < MAX_CMUX_CHANNELS_SUPPORTED && channel_id >=0) {
			dbg("Channel ID: %d", channel_id);
			g_mux_obj_ptr->info_field[frame_length] = g_mux_obj_ptr->info_field[frame_length]|((unsigned char) channel_id << 2);
		}
		else {
			err("Channel is out of range[0-8]");
			return NULL;
		}
		frame_length++;

		/* Control Field
		  * The content of the control field defines the type of frame.
		  * ****************************************************************
		  * Frame Type										0 1 2 3  4   5 6 7
		  * SABM (Set Asynchronous Balanced Mode)				1 1 1 1 P/F 1 0 0
		  * UA (Unnumbered Acknowledgement)					1 1 0 0 P/F 1 1 0
		  * DM (Disconnected Mode)								1 1 1 1 P/F 0 0 0
		  * DISC (Disconnect)									1 1 0 0 P/F 0 1 0
		  * UIH (Unnumbered Information with Header check)			1 1 1 1 P/F 1 1 1
		  *****************************************************************/
		if(PF_bit) {
			g_mux_obj_ptr->info_field[frame_length++] = frame_type |0x10;
		}
		else {
			g_mux_obj_ptr->info_field[frame_length++] = frame_type;
		}

		/* 5.2.1.5 Length Indicator */
		if (length < 128) {
			g_mux_obj_ptr->info_field[frame_length++] = (char)length <<1|0x01;
			
			/* CRC calculatable length */
			crc_len = 3;
		}
		else {
			g_mux_obj_ptr->info_field[frame_length++] = (char)(length << 1);  //Need to change
			g_mux_obj_ptr->info_field[frame_length++] = (char)(length >> 7);  //Need to change 

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
			memcpy((g_mux_obj_ptr->info_field + frame_length),data, length);
			dbg("MEMCPY :DONE");
			frame_length += length;
		}
		else {
			dbg("info field length is zero");
		}

		/*5.2.1.6 Frame Checking Sequence Field (FCS)*/
		g_mux_obj_ptr->info_field[frame_length++] = calc_crc(g_mux_obj_ptr->info_field+1, crc_len);

		/*Flag Octet*/
		g_mux_obj_ptr->info_field[frame_length++] = 0xF9;

		total_frame_length = frame_length;
	}
	else if (0x01 == g_mux_mode) {			/* TBD MUX_MODE_ADVANCE */
		dbg("For PHASE 2 !!");
	}

	*out_data_len = total_frame_length;
	dbg("*out_data_len: %d", *out_data_len);

	dbg("Exit total_frame_length: %d", total_frame_length);
	return g_mux_obj_ptr->info_field;
}

static int rcv_crc_check(unsigned char *data, unsigned char len, unsigned char rcv_FCS)
{
	unsigned char FCS = 0xFF;
	dbg("Entry");

	/* 'len' is the number of bytes in the message, 'data' points to message */
	while (len--) {
		FCS = crc_table[FCS ^ *data++];
	}
	
	/*Ones complement*/
	FCS = crc_table[FCS ^ rcv_FCS];
	
	/* 0xCF is the reversed order of 11110011 */
	if (0xCF == FCS) {		/* FCS is OK */
		dbg("Exit");
		return 1;
	}
	else {				/* FCS is NOT OK */
		err("Exit");
		return 0;
	}
}

static void tcore_cmux_flush_channel_data(void)
{
	dbg("Entry");

	g_mux_obj_ptr->info_field_len =0x0;
	memset(g_mux_obj_ptr->info_field, 0x0, MAX_CMUX_BUFFER_SIZE);
	
	dbg("Exit");
	return;
}

static void tcore_cmux_control_channel_handle(void)
{
	unsigned char cmd_type;
	int msg_len = 0;
	unsigned char *msg_start_ptr =NULL;
	dbg("Entry");
	
	/* 5.4.6.1 Message format
	  * All messages sent between the multiplexers conform to the following type, length, value format:
	  * Type Length Value 1 Value2  \85
	  */
	if (g_mux_obj_ptr->info_field_len > 0) {   
		msg_start_ptr = g_mux_obj_ptr->info_field;
		cmd_type = g_mux_obj_ptr->info_field[0];
		
		/* The EA bit is an extension bit. The EA bit is set to 1 in the last octet of the sequence; 
		  * in other octets EA is set to 0.
		  * 
		  * Search for the last octet
		  */
		while( (*msg_start_ptr++ & 0x01)) {	//TBD
			msg_len ++;
		}
		
		if((cmd_type & 0x02) == 0x02) {	//This is a command Request
			switch(cmd_type)
			{
				case CMUX_COMMAND_MSC:
				{
				dbg("Modem Status Command");
					break;
			}
				case CMUX_COMMAND_CLD:
				{
					dbg("Multiplexer close down");
				tcore_cmux_close();
					break;
			}
				default:
			{
					/* We will be supporting these commands in Phase 2 */
					dbg("Default");
					break;
			}
			}
		}
	}
	else {
		err("Frame length is less than ZERO");
	}

	dbg("Exit");
	return;
}

static void tcore_cmux_process_channel_data(CHANNEL *channel_info_ptr)
{
	int frame_type;
	int channel_id;
	int len;
	unsigned char * send_data=NULL;
	static int count = 0;

	int ret;
	dbg("Entry");

	channel_id = channel_info_ptr->channel_id;
	dbg("Channel ID: %d", channel_id);
	
	frame_type = channel_info_ptr->frame_type & 0xEF;
	dbg("frame_type: 0x%x", frame_type);

	switch(frame_type)
	{
		case CMUX_COMMAND_UI:
		case CMUX_COMMAND_UIH:
		{
			dbg("Received UI/UIH Frame");
			if(0 == channel_id) {			/* This is control info */
				dbg("Control information");
				tcore_cmux_control_channel_handle();
			}
			else {
				dbg("Normal information");
				//put in the logical HAL queue
				//this goes to the Cobject

				tcore_cmux_recv_mux_data(channel_info_ptr);
			}
			break;
		}
		case CMUX_COMMAND_UA:
		{
			dbg("Received UA Frame");
			dbg("channel_info_ptr->state: %d", channel_info_ptr->state);
			if(MUX_CHANNEL_SABM_SEND_WAITING_FOR_UA == channel_info_ptr->state) {
				channel_info_ptr->state = MUX_CHANNEL_ESTABLISHED;

				count++;
				dbg("Count: %d", count);
				if(MAX_CMUX_CHANNELS_SUPPORTED == count) {
					/* Indicate to CoreObject */
					CoreObject *co = NULL;

					/* 'modem' Core Object */
					co = g_mux_obj_ptr->modem_co;
					if(NULL == co) {
						err("'modem' Core object is not present");
						return;
					}

					/* Emit callback */
					dbg("Emit Core object callback");
					tcore_object_emit_callback(co, "CMUX-UP", NULL);
					dbg("Emitted Core object callback");

					/* Reset 'count' */
					count = 0;
				}
			}
			else if(MUX_CHANNEL_DISC_SEND_WAITING_FOR_UA == channel_info_ptr->state) {
				channel_info_ptr->state = MUX_CHANNEL_CLOSED;

				if(0 == channel_id) {
					g_mux_obj_ptr->state = MUX_CLOSED;
					tcore_cmux_close();
				}
			}
			else  {
				err("Received UA in wrong state!!!");
			}
			break;
		}
		case CMUX_COMMAND_DM:
		{
			/* 5.4.1 DLC Establishment : If the responding station is not ready or unwilling
			  * to establish the particular DLC it will reply with a DM frame with the
			  * F-bit set to 1.
			  */
			dbg("Received DM Frame");
			if((MUX_CHANNEL_ESTABLISHED == channel_info_ptr->state)
				|| (MUX_CHANNEL_SABM_SEND_WAITING_FOR_UA == channel_info_ptr->state)) {
				/* Channel State set to Close */
				channel_info_ptr->state = MUX_CHANNEL_CLOSED;
			}

			/* Flush the Channel data */
			tcore_cmux_flush_channel_data();

			break;
		}
		case CMUX_COMMAND_DISC:
		{
			dbg("Received DISC Frame");
			if(0 == channel_info_ptr->poll_final_bit) {
				/* In the case where a CMUX_COMMAND_SABM or CMUX_COMMAND_DISC command with
				  * the P bit set to 0 is received then the received frame shall be discarded.
				  */

				/* Flush the Channel data */
				tcore_cmux_flush_channel_data();
			}
			else {
				if(MUX_CHANNEL_CLOSED == channel_info_ptr->state) {
					/* If a CMUX_COMMAND_DISC command is received while in disconnected mode
					  * a CMUX_COMMAND_DM response should be sent
					  */

					/* Encoding frame */
					send_data = tcore_encode_cmux_frame(NULL, 0, channel_id, CMUX_COMMAND_DM, 1, 1, 1, &len);
					}
					else {   //send Unnumbered Acknowledgement
					send_data = tcore_encode_cmux_frame(NULL, 0, channel_id, CMUX_COMMAND_UA, 1, 1, 1, &len);
					}

					if(0 == len) {
					err("Failed to encode");
					return;
						}
					
				/* Send CMUX data */
				ret = tcore_cmux_send_data(len, send_data);
				
				/* Flush the Channel data */
					tcore_cmux_flush_channel_data();        

				/* 5.3.4 Disconnect (DISC) command: CMUX_COMMAND_DISC command sent at DLCI 0
				  * have the same meaning as the Multiplexer Close Down command
				  */
				if(0 == channel_id) {
					g_mux_obj_ptr->state = MUX_CLOSED;

					/* Close CMUX */
						tcore_cmux_close();
					}
			}
			break;
		}	
		case CMUX_COMMAND_SABM:
		{
			dbg("Received SABM Frame");
			if(0 == channel_info_ptr->poll_final_bit) {
				/* In the case where a CMUX_COMMAND_SABM or CMUX_COMMAND_DISC command with
				  * the P bit set to 0 is received then the received frame shall be discarded.
				  */

				/* Flush the Channel data */
				tcore_cmux_flush_channel_data();
			}
			else {
				/* Encoding frame */
				send_data = tcore_encode_cmux_frame(NULL, 0, channel_id,  CMUX_COMMAND_UA, 1, 1, 1, &len); 
				if(0 != len) {
					/* Send CMUX data */
					ret = tcore_cmux_send_data(len,send_data);
				}
				else {
					err("Failed to encode");
				}

				if(channel_info_ptr->state != MUX_CHANNEL_ESTABLISHED) {
					/* Channel State set to Established */
					channel_info_ptr->state = MUX_CHANNEL_ESTABLISHED;
				}
			} 
			break;
		}
	}

	dbg("Exit");
	return ;
}

static void tcore_cmux_process_rcv_frame( unsigned char *data, int len)
{
	unsigned char *frame_process_ptr = data;
	unsigned char *buf_start_ptr = data;

	CHANNEL *ch =NULL;
	unsigned char channel_id;
	int header_length;
	dbg("Entry");

	tcore_cmux_flush_channel_data();

#if 1
	//testing
	dbg("len: %d ", len);
	{
		int i=0;
		for(i = 0; i < len ; i++) {
			dbg("%02x", data[i]);
		}
	}
#endif

	/* Get the Channel ID : 1st byte will be flag (F9)..Flag checking is already done.*/
	channel_id = (*++frame_process_ptr  >> 2) & 0x3F;

	if(channel_id < MAX_CMUX_CHANNELS_SUPPORTED) {			//max channel is 8
		ch = g_mux_obj_ptr->channel_info[channel_id];

		ch->channel_id = channel_id;

		//get the EA bit
		ch->ext_bit = *frame_process_ptr & 0x01;

		//get the CR bit
		ch->cr_bit = (*frame_process_ptr++ >> 1) & 0x01;

		//get the Frame Type
		ch->frame_type = *frame_process_ptr++;

		//get the poll/Final bit
		ch->poll_final_bit = (ch->frame_type & 0x10) >> 4;

		//get the length . TBD
		if(*frame_process_ptr  & 0x01) {						//if, len < 127
			g_mux_obj_ptr->info_field_len = *frame_process_ptr++ >> 1;
			header_length = 3;
		}
		else {
			g_mux_obj_ptr->info_field_len = *(frame_process_ptr + 1) << 7;
			g_mux_obj_ptr->info_field_len = g_mux_obj_ptr->info_field_len | ((*frame_process_ptr++ & 0xFE) >> 1);
			header_length = 4;
			frame_process_ptr++;
		}
		dbg("info_field_len: %d", g_mux_obj_ptr->info_field_len);
		
		/* Copy received information field */
		memcpy(g_mux_obj_ptr->info_field, frame_process_ptr, g_mux_obj_ptr->info_field_len);

		frame_process_ptr = frame_process_ptr + g_mux_obj_ptr->info_field_len;

		//CRC check of the header
		if (rcv_crc_check(buf_start_ptr+1, header_length, *frame_process_ptr)) {
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

int tcore_cmux_rcv_from_hal(unsigned char *data, int len)
{
	unsigned char *buf =data;
	int count =0;
	int i = 0;
	int actual_len = 0; //actual length
	
	unsigned char *mux_temp_buffer = g_mux_obj_ptr->mux_temp_buffer;
	unsigned char *mux_main_buffer = g_mux_obj_ptr->mux_main_buffer;

	int total_length = 0;				/* Total length */
	int header_length = 0;			/* Header length */
	int current_frame_length = 0;		/* Current Frame length */	
	int pending_length = 0;			/* pending length in array */ 
	dbg("Entry");

	if (len > MAX_CMUX_BUFFER_SIZE){
		err("len - %d is more than MAX limit !!" ,len);
		return 0;
		}
		
	if( (len > 0 ) && (data != NULL)) {
		if(g_mux_obj_ptr->is_waiting) {
			/* Checking if Frame fully contains F9 or not. If only F9, then reject, esle process */
			while(*buf++ == 0xF9) {
				count++;
			}

			if (count == len) {	// In case all are F9 
				err("Not a valid frame");
				return 0;
			}

			dbg("cur_main_buf_len: %d len = %d", g_mux_obj_ptr->cur_main_buf_len, len);
			memcpy((mux_main_buffer + g_mux_obj_ptr->cur_main_buf_len), data, len);

			g_mux_obj_ptr->cur_main_buf_len = g_mux_obj_ptr->cur_main_buf_len + len;
			dbg("cur_main_buf_len: %d", g_mux_obj_ptr->cur_main_buf_len);

			if(g_mux_obj_ptr->msg_len <= len) {
				g_mux_obj_ptr->is_waiting = 0;
				tcore_cmux_rcv_from_hal(mux_main_buffer, g_mux_obj_ptr->cur_main_buf_len);
			}
			else {
				g_mux_obj_ptr->msg_len = g_mux_obj_ptr->msg_len - len;
				dbg("Still waiting for the complete frame!!!");
			}
		}
		else {
			if(*buf ==0xF9) {	// start flag present
				/* Remove extra flags */
				while(*++buf == 0xF9) {
					count++;
				}

				/* Check if received data has atleast three octets between flags */
				for(i = 0; ( (i<3) && (*(buf+i)!=0xF9)); i++);

				if((i<3) || (count == (len-1))) {				// In case all are F9 or three octets between flags not found
					err("Not a valid frame");
					return 0;
				}

				actual_len = len - count;
				dbg("actual_length: %d", actual_len);

				/* Memset mux_temp_buffer */
				memset(mux_temp_buffer, 0x0, MAX_CMUX_BUFFER_SIZE);

				/* Copy data to mux_temp_buffer */				
				memcpy(mux_temp_buffer, data + count, actual_len);

				buf = mux_temp_buffer + 3;

				/* Find the info length field of the current frame */
				FIND_LENGTH(buf, header_length, total_length);

				/* Calculate the current frame length including the headers */
				current_frame_length = header_length + total_length;

				/* Memset mux_main_buffer */
				memset(mux_main_buffer, 0x0, MAX_CMUX_BUFFER_SIZE);

				if(current_frame_length == actual_len) {				//Only 1 frame is received and its a complete frame
					dbg("Single frame received");
					if(mux_temp_buffer[current_frame_length-0x01] == 0xF9) {
						dbg("current_frame_length: %d", current_frame_length);
						
						/* Copy received frame to mux_main_buffer */
						memcpy(mux_main_buffer,mux_temp_buffer, current_frame_length);

						/* Process the recieved frame */
						tcore_cmux_process_rcv_frame(mux_main_buffer, current_frame_length);
						
						/* Initialize the main buffer */
						memset(mux_main_buffer, 0x0, MAX_CMUX_BUFFER_SIZE);
					}
					else {
						err("End flag missing!!!");
						return 0;
					}
				}
				else if(current_frame_length < actual_len) {			//more than one frame
					dbg("Multiple frames received");
					if(mux_temp_buffer[current_frame_length-0x01] == 0xF9) {
						dbg("current_frame_length: %d", current_frame_length);

						/* Copy received frame to mux_main_buffer */
						memcpy(mux_main_buffer, mux_temp_buffer, current_frame_length);

						/* Process the recieved frame */
						tcore_cmux_process_rcv_frame(mux_main_buffer ,current_frame_length);

						/* Initialize the main buffer */
						memset(mux_main_buffer, 0x0, MAX_CMUX_BUFFER_SIZE);
						mux_main_buffer[0] = 0xF9;
						
						pending_length = actual_len - current_frame_length;
						dbg("pending_length: %d", pending_length);

						/* Copy the rest of the frames to mux_main_buffer */
						memcpy((mux_main_buffer + 0x01), (mux_temp_buffer + current_frame_length), pending_length);

						/* Receive pending frames */
						tcore_cmux_rcv_from_hal(mux_main_buffer, pending_length + 0x01);
					}
					else {
						err("End flag missing!!!");
						return 0;
					}
				}
				else if(current_frame_length > actual_len) {			// frame is incomplete. Yet to receive the full frame
					dbg("Incomplete frame received");
					g_mux_obj_ptr->is_waiting = 1;					
					dbg("actual_len: %d", actual_len);

					/* Copy received frame to mux_main_buffer */
					memcpy(mux_main_buffer, mux_temp_buffer + count, actual_len);
					
					g_mux_obj_ptr->msg_len =  current_frame_length - actual_len;
					g_mux_obj_ptr->cur_main_buf_len = actual_len;
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

static void tcore_cmux_channel_init(CMUX_Channels channel_id)
{
	CHANNEL *ch = NULL;
	dbg("Entry");
	
	ch = g_mux_obj_ptr->channel_info[channel_id];
	memset(ch, 0x0, sizeof(CHANNEL));
	
	ch->channel_id = channel_id;
	ch->state = MUX_CHANNEL_SABM_SEND_WAITING_FOR_UA;

	ch->co = NULL;
	ch->hal = NULL;

	/* TODO - Check if required */
    	ch->frame_type = CMUX_COMMAND_SABM;
	ch->ext_bit = 0x01;
	ch->cr_bit = 0x01;
	ch->poll_final_bit = 0x01;

	dbg("Channel ID %d initialized", channel_id);
	
	dbg("Exit");
	return;
}

static void tcore_cmux_close_channel(int channel_id)
{
	CHANNEL *ch = NULL;
	unsigned char *send_data = NULL;
	int ret, len =0;
	dbg("Entry");

	ch = g_mux_obj_ptr->channel_info[channel_id];
	
	if(ch->state != MUX_CHANNEL_CLOSED) {
		ch->frame_type = CMUX_COMMAND_DISC;
		ch->ext_bit = 0x01;
		ch->cr_bit = 0x01;
		ch->state = MUX_CHANNEL_DISC_SEND_WAITING_FOR_UA;

		/* Send DSC command */
		/* Encoding frame */
		send_data = tcore_encode_cmux_frame(NULL,0, channel_id,  CMUX_COMMAND_DISC, 0x01, 0x01, 0x01, &len); 
		if(0 != len) {
			/* Send CMUX data */
			ret = tcore_cmux_send_data(len, send_data);
		}
		else {
			err("Failed to encode");
		}
	}
	else {
		/* Channel is already closed */
		err("Channel is already closed");
	}

	dbg("Exit");
	return;
}

static void tcore_cmux_free(void)
{
	int channel;
	dbg("Entry");
	
	if(g_mux_obj_ptr) {
		/* Free Information Field */
		if (g_mux_obj_ptr->info_field) 
			free(g_mux_obj_ptr->info_field);
				
		for (channel = 0 ; channel < MAX_CMUX_CHANNELS_SUPPORTED ; channel++) {
			/* Free Channel Information */
			if(g_mux_obj_ptr->channel_info[channel]) {
				free(g_mux_obj_ptr->channel_info[channel]);
			}
		}

		/* Free MUX Object */
		free(g_mux_obj_ptr);
	}
	else {
		err("MUX Object doesn't exist");
	}

	dbg("Exit");
	return;
}

TReturn tcore_cmux_init(TcorePlugin *plugin, TcoreHal *hal)
{
	unsigned char *data = NULL;
	int data_len = 0;

	int index;

	TReturn ret = TCORE_RETURN_SUCCESS;
	dbg("Entry");

	dbg("Physical HAL: %x", hal);

	/* Creat new CMUX Object */
	g_mux_obj_ptr = tcore_cmux_new();
	if(NULL == g_mux_obj_ptr) {
		err("Failed to create MUX object");

		ret = TCORE_RETURN_FAILURE;
		goto ERROR;
	}

	/* Save Plugin */
	g_mux_obj_ptr->plugin = plugin;
	
	/* Save Physical HAL */
	g_mux_obj_ptr->phy_hal = hal;

	/* Setting Receive callback function for data received from Physical HAL */
	g_mux_obj_ptr->cb_func= tcore_cmux_recv_mux_data;
	
      /* After MUX setup, AT parse functionality of PHY HAL should be disabled,
        * here we change the mode of PHYSICAL HAL to Transparent.
        */
	tcore_hal_set_mode(g_mux_obj_ptr->phy_hal, TCORE_HAL_MODE_TRANSPARENT);
	dbg("Physical HAL mode changed to Transparent");
 
	/* Initialize all the Channels */
	/* Open all Channels */
	for(index = 0 ; index < MAX_CMUX_CHANNELS_SUPPORTED ; index++) {

		dbg("Initialize the Channel %d", index); 
		tcore_cmux_channel_init((CMUX_Channels)index);
		
		dbg("Opening Channel %d", index);
		/* Encode CMUX Frame */
		data = tcore_encode_cmux_frame(NULL, 0, index, CMUX_COMMAND_SABM, 0x01, 0x01, 0x01, &data_len);
		if(0 == data_len) {
			err("Failed to encode");

			ret = TCORE_RETURN_FAILURE;
			goto ERROR;
		}
		dbg("data_len: %d data: %s", data_len, data);

		/* Send CMUX data */
		tcore_cmux_send_data(data_len, data);
		dbg("CMUX Control Request sent to CP");

		/* Set Core object and HAL */
		tcore_cmux_link_core_object_hal((CMUX_Channels)index, plugin);
	}

	dbg("Exit");
	return ret;

ERROR:
	/* Free the allocated CMUX memory */
	tcore_cmux_free();

	err("Exit");
	return ret;
}

void tcore_cmux_close(void)
{
	int channel = 0;
	int index = 0;
	CoreObject *co = NULL;
	GSList *co_list = NULL;
	dbg("Entry");

	for (channel = 0 ; channel < MAX_CMUX_CHANNELS_SUPPORTED ; channel++) {
		dbg("Channel ID: %d", channel);
		index = 0;

		/* Close Channel - Send DSC command */
		tcore_cmux_close_channel(channel);

		/* Revert Physical HAL as HAL of each Core Object associated to this Channel */
		while (NULL != cmux_channel_core_object[channel].core_object_name[index]) {
			co = NULL;

			/* Core Objects list */
			co_list = g_mux_obj_ptr->channel_info[channel]->co;
			dbg("Core Objects list : %p", co_list);
			
			/* Core Object list may contain multiple Core Objects.
			  * Revert to Physical HAL for each of the Core Objects associated
			  * with this Channel
			  */
			while(NULL != co_list) {
				if(NULL != co_list->data) {
					if(!strcmp((const char *)cmux_channel_core_object[channel].core_object_name[index], (const char *)tcore_object_ref_name((CoreObject *)co_list->data))) {
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
				tcore_object_set_hal(co, g_mux_obj_ptr->phy_hal);
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
		tcore_hal_free(g_mux_obj_ptr->channel_info[channel]->hal);
	}
	
	/* Change the mode of PHYSICAL HAL to Custom */
	tcore_hal_set_mode(g_mux_obj_ptr->phy_hal, TCORE_HAL_MODE_CUSTOM);

	/* Free all the allocated memory */
	tcore_cmux_free();
		
	dbg("Exit");
	return;
}
