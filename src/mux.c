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

#include <sys/ioctl.h>

#include <glib.h>

#include "tcore.h"

#include "hal.h"
#include "core_object.h"
#include "plugin.h"
#include "user_request.h"
#include "server.h"
#include "at.h"
#include "mux.h"

/* Channel name maximum length */
#define CMUX_MAX_CHANNEL_NAME			16

/* CMUX Commands */
#define CMUX_COMMAND_SABM				0x2F
#define CMUX_COMMAND_UA					0x63
#define CMUX_COMMAND_DM					0x0F
#define CMUX_COMMAND_DISC					0x43
#define CMUX_COMMAND_UIH					0xEF
#define CMUX_COMMAND_UI					0x03

/* CMUX Delimiter Byte */
#define CMUX_FRAME_DELIMITER				0xF9

/*
 * CMUX Channels [0-64] -
 *
 * CMUX_CHANNEL_0 is dedicated CMUX Control Channnel.
 */
typedef enum {
	CMUX_CHANNEL_NONE = -1,	/* No CMUX */
	CMUX_CHANNEL_0 = 0,		/* CMUX Control Channel */

	CMUX_CHANNEL_MAX = 65	/* Based on #GPP 27.010 */
} tcore_cmux_channel_id;

/*
 * CMUX Channel States
 *
 * Only Internal CMUX Channel states are managed,
 * Kernel CMUX Channel states are managed by Kernel directly.
 */
typedef enum {
	CMUX_CHANNEL_CLOSED,
	CMUX_CHANNEL_SABM_SEND_WAITING_FOR_UA,
	CMUX_CHANNEL_ESTABLISHED,
	CMUX_CHANNEL_UA_NOT_RECEIVED_RETRY,
	CMUX_CHANNEL_DM_RECEIVED_CLOSING,
	CMUX_CHANNEL_DM_SEND,
	CMUX_CHANNEL_DISC_RECEIVED_CLOSING,
	CMUX_CHANNEL_DISC_SEND_WAITING_FOR_UA,
	CMUX_CHANNEL_UA_SEND_CLOSING,
	CMUX_CHANNEL_UA_RECEIVED,
	CMUX_CHANNEL_UA_SENDING,
} tcore_cmux_channel_state;

/* CMUX State */
typedef enum {
	CMUX_NOT_INITIALIZED,
	CMUX_INITIALIZED,
	CMUX_CLOSED
} tcore_cmux_state;

/*
 * CMUX CONTROL COMMANDS
 * We are supporting only MSC and CLD commands for phase 1
 */
/* Modem Status Command */
#define  CMUX_COMMAND_MSC					0xE3
/* Multiplexer close down */
#define  CMUX_COMMAND_CLD					0xC3

#define FIND_LENGTH(buf, header_length, total_length)  do {	\
		if (*buf & 0x01) { \
			total_length = *buf++ >> 1;	\
			header_length = 6; \
		} else { \
			total_length = *(buf + 0x01) << 7; \
			total_length = total_length | (*buf & 0xFE) >> 1; \
			header_length = 7; \
		} \
} while (0)

/*================= CRC TABLE=========================*/
const unsigned char crc_table[256] = {	/* reversed, 8-bit, poly=0x07 */
	0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,
	0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
	0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,
	0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
	0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,
	0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
	0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,
	0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
	0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,
	0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
	0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,
	0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
	0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,
	0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
	0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,
	0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
	0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,
	0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
	0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,
	0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
	0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,
	0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
	0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,
	0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
	0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,
	0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
	0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,
	0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
	0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,
	0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
	0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,
	0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};
/*================= CRC TABLE=========================*/

/* CMUX Channel */
struct cmux_channel {
	TcoreHal *hal;

	tcore_cmux_channel_state channel_state;
	tcore_cmux_channel_id channel_id;

	int frame_type;
	unsigned char ext_bit;
	unsigned char cr_bit;
	unsigned char poll_final_bit;
};

/* CMUX structure - Internal */
struct cmux_internal {
	tcore_cmux_channel *channel_info[CMUX_CHANNEL_MAX];

	int is_waiting;

	int msg_len;
	int cur_main_buf_len;

	/* Channel Setup callbacks */
	cmux_setup_cb_func channel_setup_cb;
	gpointer channel_setup_user_data;

	/* Channel Setup complete callback */
	cmux_setup_complete_cb_func setup_complete_cb;
	gpointer setup_complete_user_data;

	int info_field_len;
	unsigned char *info_field;
};

/* Global CMUX Object */
struct cmux_obj {
	/* Physical HAL */
	TcoreHal *phy_hal;

	/* Modem Interface Plug-in */
	TcorePlugin *plugin;

	tcore_cmux_state cmux_state;

	tcore_cmux_mode cmux_mode;
	int max_cmux_channels;
	unsigned int max_cmux_buf_size;

	/* CMUX frame processing buffer */
	unsigned char *cmux_buffer;

	/* Only Internal MUX is managed by libtcore */
	struct cmux_internal internal_mux;
};

/* CMUX Object */
static GSList *g_cmux_obj = NULL;

/* All the local functions declared below */
static void _cmux_on_confirmation_message_send(TcorePending *plugin,
											gboolean result, void *user_data);

tcore_cmux_object *_cmux_new(int max_channels, unsigned int buffer_size);
static void _cmux_free(tcore_cmux_object *cmux_obj);

static void _cmux_channel_init(tcore_cmux_object *cmux_obj, int channel_id);
static void _cmux_close_channel(tcore_cmux_object *cmux_obj, int channel_id);

static TcoreHal *_cmux_create_logical_hal(tcore_cmux_object *cmux_obj,
								tcore_cmux_channel *channel);

static gboolean _cmux_recv_cmux_data(tcore_cmux_object *cmux_obj,
				tcore_cmux_channel *channel);
static void _cmux_process_rcv_frame(tcore_cmux_object *cmux_obj, int length);
static void _cmux_process_channel_data(tcore_cmux_object *cmux_obj,
					tcore_cmux_channel *channel);

static void _cmux_control_channel_handle(tcore_cmux_object *cmux_obj);
static void _cmux_flush_channel_data(tcore_cmux_object *cmux_obj);

static TReturn _cmux_send_data(TcoreHal *hal, int data_len, unsigned char *data);
static unsigned char *_cmux_encode_cmux_frame(tcore_cmux_object *cmux_obj,
						unsigned char *data, unsigned int length, int channel_id,
						int frame_type, unsigned char EA_bit, unsigned char CR_bit,
						unsigned char PF_bit, int *out_data_len);

static tcore_cmux_object *_cmux_get_cmux_object(TcorePlugin *plugin);

static unsigned char _cmux_calc_crc(unsigned char *header, int length);
static int _cmux_check_recv_crc(unsigned char *data,
									unsigned char length,
									unsigned char rcv_fcs);

static unsigned char _cmux_calc_crc(unsigned char *header, int length)
{
	unsigned char fcs = 0xFF;	/* Init */
	unsigned char crc = 0x00;

	/*
	 * 'length' is the number of bytes in the message,
	 * 'header' points to message
	 */
	while (length--)
		fcs = crc_table[fcs ^ *header++];

	/* Ones complement */
	crc = (0xFF - fcs);

	dbg("Exit - CRC: [0x%02x]", crc)
	return crc;
}

static int _cmux_check_recv_crc(unsigned char *data,
									unsigned char length,
									unsigned char rcv_fcs)
{
	unsigned char fcs = 0xFF;	/* Init */

	/*
	 * 'length' is the number of bytes in the message,
	 * 'data' points to message
	 */
	while (length--)
		fcs = crc_table[fcs ^ *data++];

	/* Ones complement */
	fcs = crc_table[fcs ^ rcv_fcs];

	/* 0xCF is the reversed order of 11110011 */
	if (fcs == 0xCF)	/* FCS is OK */
		return 1;
	else				/* FCS is NOT OK */
		return 0;
}

static tcore_cmux_object *_cmux_get_cmux_object(TcorePlugin *plugin)
{
	tcore_cmux_object *cmux_obj;
	GSList *tmp_obj;
	dbg("Entry");

	/* Check across CMUX Objects list for specific CMUX Object */
	for (tmp_obj = g_cmux_obj ; tmp_obj ; tmp_obj = tmp_obj->next) {
		cmux_obj = tmp_obj->data;
		if (cmux_obj == NULL)
			continue;

		/* Check for matching 'plugin' */
		if (plugin == cmux_obj->plugin) {
			dbg("Found CMUX object");
			return cmux_obj;
		}
	}

	return NULL;
}

static unsigned char *_cmux_encode_cmux_frame(tcore_cmux_object *cmux_obj,
						unsigned char *data, unsigned int length, int channel_id,
						int frame_type, unsigned char EA_bit, unsigned char CR_bit,
						unsigned char PF_bit, int *out_data_len)
{
	int frame_length = 0;
	int total_frame_length = 0;
	int crc_len = 0;
	dbg("Entry");

	/* Flush channel data */
	_cmux_flush_channel_data(cmux_obj);

	if (length > cmux_obj->max_cmux_buf_size) {
		err("Length - %d  exceeds the limit", length);
		return NULL;
	}

	/* Flag Octet */
	cmux_obj->internal_mux.info_field[frame_length++] = CMUX_FRAME_DELIMITER;

	/* Mode of Operation */
	if (cmux_obj->cmux_mode == CMUX_MODE_BASIC) {	/* BASIC */
		/*
		 * EA: Extension Bit
		 * C/R: Command Response
		 */
		cmux_obj->internal_mux.info_field[frame_length] =
				(cmux_obj->internal_mux.info_field[frame_length]
					| ((EA_bit & 0x01) | ((CR_bit << 1) & 0x02)));

		/* DLCI: Data Link Connection Identifier */
		/* Check if the channel is within range */
		if (channel_id < cmux_obj->max_cmux_channels && channel_id >= 0) {
			dbg("Channel ID: [%d]", channel_id);
			cmux_obj->internal_mux.info_field[frame_length] =
				(cmux_obj->internal_mux.info_field[frame_length]
							| ((unsigned char)channel_id << 2));
		} else {
			err("Channel ID [%d] is out of range [0-%d]",
					channel_id, cmux_obj->max_cmux_channels);
			return NULL;
		}
		frame_length++;

		/*
		 * Control Field
		 *
		 * The content of the control field defines the type of frame.
		 * ****************************************************************
		 * Frame Type										0 1 2 3  4   5 6 7
		 * SABM (Set Asynchronous Balanced Mode)				1 1 1 1 P/F 1 0 0
		 * UA (Unnumbered Acknowledgement)					1 1 0 0 P/F 1 1 0
		 * DM (Disconnected Mode)								1 1 1 1 P/F 0 0 0
		 * DISC (Disconnect)									1 1 0 0 P/F 0 1 0
		 * UIH (Unnumbered Information with Header check)			1 1 1 1 P/F 1 1 1
		 ******************************************************************/
		if (PF_bit)
			cmux_obj->internal_mux.info_field[frame_length++] =
													frame_type | 0x10;
		else
			cmux_obj->internal_mux.info_field[frame_length++] = frame_type;

		/* 5.2.1.5 Length Indicator */
		if (length < 128) {
			cmux_obj->internal_mux.info_field[frame_length++] =
													(char) length << 1 | 0x01;

			/* CRC calculatable length */
			crc_len = 3;
		} else {
			cmux_obj->internal_mux.info_field[frame_length++] =
													(char) (length << 1);
			cmux_obj->internal_mux.info_field[frame_length++] =
													(char) (length >> 7);

			/* CRC calculatable length */
			crc_len = 4;
		}

		/*
		 * We need to divide the frames into maximum frame length supported by IMC.
		 * If IMC supports length according to 27.010 , we can send max of 16,384 bytes.
		 * Need to discuss with IMC.
		 */

		/* 5.2.1.4 Information Field */
		if (length > 0) {
			memcpy((cmux_obj->internal_mux.info_field + frame_length),
														data, length);
			frame_length += length;
		} else
			dbg("info field length is zero");

		/* 5.2.1.6 Frame Checking Sequence Field (FCS) */
		cmux_obj->internal_mux.info_field[frame_length++] =
			_cmux_calc_crc(cmux_obj->internal_mux.info_field + 1, crc_len);

		/* Flag Octet */
		cmux_obj->internal_mux.info_field[frame_length++] = CMUX_FRAME_DELIMITER;

		total_frame_length = frame_length;
	} else if (cmux_obj->cmux_mode == CMUX_MODE_ADVANCED) {
		/* TBD CMUX_MODE_ADVANCE */
		dbg("Advanced CMUX mode : TBD");
	}

	*out_data_len = total_frame_length;
	dbg("Data (Frame) Length: [%d]", *out_data_len);

	return cmux_obj->internal_mux.info_field;
}

static void _cmux_control_channel_handle(tcore_cmux_object *cmux_obj)
{
	unsigned char cmd_type;
	int msg_len = 0;
	unsigned char *msg_start_ptr;
	dbg("Entry");

	/*
	 * 5.4.6.1 Message format
	 * All messages sent between the multiplexers conform to the following
	 * type, length, value format:
	 * Type Length Value 1 Value2  \85
	 */
	if (cmux_obj->internal_mux.info_field_len > 0) {
		msg_start_ptr = cmux_obj->internal_mux.info_field;
		cmd_type = cmux_obj->internal_mux.info_field[0];

		/*
		 * The EA bit is an extension bit. The EA bit is set to 1 in the last octet
		 * of the sequence; in other octets EA is set to 0.
		 *
		 * Search for the last octet
		 */
		while ((*msg_start_ptr++ & 0x01))
			msg_len++;

		if ((cmd_type & 0x02) == 0x02) {	/* This is a command Request */
			switch (cmd_type) {
				case CMUX_COMMAND_MSC:
					dbg("Modem Status Command");
					break;
				case CMUX_COMMAND_CLD:
					dbg("Multiplexer close down");

					cmux_obj->cmux_state = CMUX_CLOSED;

					/* TODO - Need to notify regarding CMUX closure */
					tcore_cmux_close(cmux_obj->phy_hal, NULL, NULL);
					break;
				default:
					/* We will be supporting these commands in Phase 2 */
					dbg("Default");
					break;
			}
		}
	} else
		err("Frame length is less than ZERO");

	dbg("Exit");
}

static void _cmux_process_channel_data(tcore_cmux_object *cmux_obj,
					tcore_cmux_channel *channel)
{
	int frame_type;
	int channel_id;
	int len;
	unsigned char *send_data;
	static int count = 0;
	int ret;
	dbg("Entry");

	channel_id = channel->channel_id;
	dbg("Channel ID: [%d]", channel_id);

	frame_type = channel->frame_type & 0xEF;
	dbg("Frame Type: [0x%02x]", frame_type);

	switch (frame_type) {
		case CMUX_COMMAND_UI:
		case CMUX_COMMAND_UIH:
			dbg("Received UI/UIH Frame");
			if (channel_id == CMUX_CHANNEL_0) {              /* This is control info */
				dbg("Control information");
				_cmux_control_channel_handle(cmux_obj);
			} else {
				dbg("Normal information");

				/* Put in the logical HAL queue, this goes to the Cobject */
				if (FALSE == _cmux_recv_cmux_data(cmux_obj, channel))
					err("Failed receive callback");
			}
			break;
		case CMUX_COMMAND_UA:
			dbg("Received UA Frame - Channel State: [%d]", channel->channel_state);
			if (CMUX_CHANNEL_SABM_SEND_WAITING_FOR_UA == channel->channel_state) {
				channel->channel_state = CMUX_CHANNEL_ESTABLISHED;

				if (channel->channel_id != CMUX_CHANNEL_0) {
					TcoreHal *hal;

					/* Create Logical HAL */
					hal = _cmux_create_logical_hal(cmux_obj, channel);
					if (hal != NULL) {
						dbg("Invoking CMUX Channel Setup callback for [%d] channel",
													channel->channel_id);
						/*
						 * 'channel_setup_cb' cannot be NULL as it is MANDATED during
						 * CMUX setup operation.
						 */
						cmux_obj->internal_mux.channel_setup_cb(channel->channel_id, hal,
										cmux_obj->internal_mux.channel_setup_user_data);
					} else
						err("Failed to Create Logical HAL");
				}

				count++;
				dbg("Count: [%d]", count);
				if (cmux_obj->max_cmux_channels == count) {
					dbg("Invoking CMUX Channel Setup Complete callback - Total Channels: [%d]",
													count);
					/*
					 * 'setup_complete_cb' cannot be NULL as it is MANDATED during
					 * CMUX setup operation.
					 */
					cmux_obj->internal_mux.setup_complete_cb(
						cmux_obj->internal_mux.setup_complete_user_data);

					/* Reset 'count' */
					count = 0;
				}
			} else if (CMUX_CHANNEL_DISC_SEND_WAITING_FOR_UA ==
										channel->channel_state) {
				channel->channel_state = CMUX_CHANNEL_CLOSED;

				if (channel_id == CMUX_CHANNEL_0) {
					cmux_obj->cmux_state = CMUX_CLOSED;

					/* TODO - Need to notify regarding CMUX closure */
					tcore_cmux_close(cmux_obj->phy_hal, NULL, NULL);
				}
			} else
				err("Received UA in wrong state!!!");

			break;
		case CMUX_COMMAND_DM:
			/*
			 * 5.4.1 DLC Establishment : If the responding station is not ready or unwilling
			 * to establish the particular DLC it will reply with a DM frame with the
			 * F-bit set to 1.
			 */
			dbg("Received DM Frame");
			if ((channel->channel_state == CMUX_CHANNEL_ESTABLISHED)
				|| (channel->channel_state ==
							CMUX_CHANNEL_SABM_SEND_WAITING_FOR_UA)) {
				/* Channel State set to Close */
				channel->channel_state = CMUX_CHANNEL_CLOSED;
			}

			/* Flush the Channel data */
			_cmux_flush_channel_data(cmux_obj);
			break;
		case CMUX_COMMAND_DISC:
			dbg("Received DISC Frame");
			if (channel->poll_final_bit == 0) {
				/*
				 * In the case where a CMUX_COMMAND_SABM or
				 * CMUX_COMMAND_DISC command with
				 * the P bit set to 0 is received then the received frame shall be
				 * discarded.
				 */

				/* Flush the Channel data */
				_cmux_flush_channel_data(cmux_obj);
			} else {
				if (channel->channel_state == CMUX_CHANNEL_CLOSED) {
					/*
					 * If a CMUX_COMMAND_DISC command is received while in
					 * disconnected mode a CMUX_COMMAND_DM response should
					 * be sent
					 */

					/* Encoding frame */
					send_data = _cmux_encode_cmux_frame(cmux_obj, NULL,
											0, channel_id, CMUX_COMMAND_DM,
											0x01, 0x01, 0x01, &len);
				} else {				/* Send Unnumbered Acknowledgement */
					/* Encoding frame */
					send_data = _cmux_encode_cmux_frame(cmux_obj, NULL,
											0, channel_id, CMUX_COMMAND_UA,
											0x01, 0x01, 0x01, &len);
				}

				if (len == 0) {
					err("Failed to encode");
					return;
				}

				/* Send CMUX data */
				ret = _cmux_send_data(cmux_obj->phy_hal, len, send_data);

				/* Flush the Channel data */
				_cmux_flush_channel_data(cmux_obj);

				/*
				 * 5.3.4 Disconnect (DISC) command: CMUX_COMMAND_DISC
				 * command sent at DLCI 0 have the same meaning as the
				 * Multiplexer Close Down command.
				 */
				if (channel_id == CMUX_CHANNEL_0) {
					cmux_obj->cmux_state = CMUX_CLOSED;

					/* Close CMUX */
					/* TODO - Need to notify regarding CMUX closure */
					tcore_cmux_close(cmux_obj->phy_hal, NULL, NULL);
				}
			}
			break;
		case CMUX_COMMAND_SABM:
			dbg("Received SABM Frame");
			if (channel->poll_final_bit == 0) {
				/*
				 * In the case where a CMUX_COMMAND_SABM or CMUX_COMMAND_DISC
				 * command with the P bit set to 0 is received then the received frame
				 * shall be discarded.
				 */

				/* Flush the Channel data */
				_cmux_flush_channel_data(cmux_obj);
			} else {
				/* Encoding frame */
				send_data = _cmux_encode_cmux_frame(cmux_obj, NULL,
										0, channel_id, CMUX_COMMAND_UA,
										0x01, 0x01, 0x01, &len);
				if (len != 0)
					/* Send CMUX data */
					ret = _cmux_send_data(cmux_obj->phy_hal, len, send_data);
				else
					err("Failed to encode");

				if (channel->channel_state != CMUX_CHANNEL_ESTABLISHED)
					/* Channel State set to Established */
					channel->channel_state = CMUX_CHANNEL_ESTABLISHED;
			}
		break;
	}

	dbg("Exit");
}

static void _cmux_flush_channel_data(tcore_cmux_object *cmux_obj)
{
	dbg("Entry");

	if (cmux_obj == NULL)
		return;

	cmux_obj->internal_mux.info_field_len = 0x0;
	memset(cmux_obj->internal_mux.info_field, 0x0, cmux_obj->max_cmux_buf_size);

	dbg("Exit");
}

static void _cmux_process_rcv_frame(tcore_cmux_object *cmux_obj, int length)
{
	unsigned char *frame_process_ptr = cmux_obj->cmux_buffer;
	unsigned char *buf_start_ptr = cmux_obj->cmux_buffer;

	tcore_cmux_channel *ch;
	unsigned char channel_id;
	int header_length;
	dbg("Entry");

	/* Flush channel data */
	_cmux_flush_channel_data(cmux_obj);

	/* Get the Channel ID: 1st byte will be flag (F9). Flag checking is already done */
	channel_id = (*++frame_process_ptr >> 2) & 0x3F;

	if (channel_id < cmux_obj->max_cmux_channels) {
		ch = cmux_obj->internal_mux.channel_info[channel_id];

		ch->channel_id = channel_id;

		/* get the EA bit */
		ch->ext_bit = *frame_process_ptr & 0x01;

		/* get the CR bit */
		ch->cr_bit = (*frame_process_ptr++ >> 1) & 0x01;

		/* get the Frame Type */
		ch->frame_type = *frame_process_ptr++;

		/* get the poll/Final bit */
		ch->poll_final_bit = (ch->frame_type & 0x10) >> 4;

		/* get the length . TBD */
		if (*frame_process_ptr & 0x01) {                        /* if, len < 127 */
			cmux_obj->internal_mux.info_field_len = *frame_process_ptr++ >> 1;
			header_length = 3;
		} else {
			cmux_obj->internal_mux.info_field_len = *(frame_process_ptr + 1) << 7;
			cmux_obj->internal_mux.info_field_len =
						(cmux_obj->internal_mux.info_field_len
						| ((*frame_process_ptr++ & 0xFE) >> 1));
			header_length = 4;
			frame_process_ptr++;
		}
		dbg("info_field_len: [%d]", cmux_obj->internal_mux.info_field_len);

		/* Copy received information field */
		memcpy(cmux_obj->internal_mux.info_field, frame_process_ptr,
					cmux_obj->internal_mux.info_field_len);

		frame_process_ptr = frame_process_ptr + cmux_obj->internal_mux.info_field_len;

		/* CRC check of the header */
		if (_cmux_check_recv_crc((buf_start_ptr + 1), header_length, *frame_process_ptr)) {
			dbg("Calling _cmux_process_channel_data");
			_cmux_process_channel_data(cmux_obj, ch);
		} else
			err("CRC check of the header FAILED.. Drop the packet !!");
	} else
		err("Incorrect channel... Drop the packet !!");

	dbg("Exit");
}

static TReturn _cmux_send_data(TcoreHal *hal, int data_len, unsigned char *data)
{
	TReturn ret;
	dbg("Entry");

	/* Directly send to Physical HAL */
	ret = tcore_hal_send_data(hal, data_len, (void *) data);
	if (ret != TCORE_RETURN_SUCCESS) {
		err("Failed to send CMUX data");
	} else
		dbg("Successfully sent CMUX data");

	dbg("Exit");
	return ret;
}

static gboolean _cmux_recv_cmux_data(tcore_cmux_object *cmux_obj,
				tcore_cmux_channel *channel)
{
	TcoreHal *hal;
	dbg("Entry");

	/* Dereferencing HAL from Channel Pointer */
	hal = channel->hal;
	if (hal == NULL) {
		err("No HAL");
		return FALSE;
	}

	dbg("Dispatching to logical HAL - hal: [0x%x]", (unsigned int)hal);
	if (tcore_hal_dispatch_response_data(hal, 0,
			cmux_obj->internal_mux.info_field_len,
			cmux_obj->internal_mux.info_field)
			!= TCORE_RETURN_SUCCESS) {
		err("Exit");
		return FALSE;
	}

	dbg("Exit");
	return TRUE;
}

static TReturn _cmux_hal_power(TcoreHal *hal, gboolean flag)
{
	dbg("Entry");

	if (flag == TRUE) {		/* Powering ON */
		dbg("Powering ON");
		return tcore_hal_set_power_state(hal, TRUE);
	} else {					/* Powering OFF */
		dbg("Powering OFF");
		return tcore_hal_set_power_state(hal, FALSE);
	}
}

static TReturn _cmux_hal_send(TcoreHal *hal, unsigned int data_len, void *data)
{
	tcore_cmux_object *cmux_obj;
	char *hal_name;
	unsigned char *send_data;
	char *channel_name;
	int channel_id;
	int len;

	TReturn ret = TCORE_RETURN_FAILURE;
	dbg("Entry");

	/* Check if Logical HAL is Powered ON */
	if (tcore_hal_get_power_state(hal) == FALSE) {
		err("HAL is not Powered UP");
		return TCORE_RETURN_FAILURE;
	}

	/*
	 * Get CMUX Object from Modem Interface Plug-in
	 */
	cmux_obj = _cmux_get_cmux_object(tcore_hal_ref_plugin(hal));
	if (cmux_obj == NULL) {
		err("Failed to find CMUX object");
		return TCORE_RETURN_FAILURE;
	}

	channel_id = cmux_obj->max_cmux_channels + 1;

	channel_name = tcore_hal_get_name(hal);
	dbg("HAL name: [%s]", channel_name);

	if (channel_name != NULL) {
		int i;

		/*
		 * Channel 0 is dedicated to CMUX Control Channel,
		 * hence starting from 1.
		 */
		for (i = 1 ; cmux_obj->max_cmux_channels > i ; i++) {
			hal_name =
				tcore_hal_get_name(cmux_obj->internal_mux.channel_info[i]->hal);
			if (hal_name == NULL) {
				dbg("HAL name: [%s]", hal_name);
				continue;
			}

			/*
			 * Comparing all Logical HAL names with required HAL name.
			 */
			if (strcmp(hal_name, channel_name) == 0) {
				channel_id = cmux_obj->internal_mux.channel_info[i]->channel_id;
				dbg("Found Channel ID: [%d]", channel_id);

				/* Free HAL name */
				g_free(hal_name);

				break;
			}

			/* Free HAL name */
			g_free(hal_name);
		}

		/* Free memory */
		g_free(channel_name);
	} else {
		err("No name defined for HAL");
		return ret;
	}

	if (channel_id > cmux_obj->max_cmux_channels) {
		err("Failed to find Channel ID");
		return ret;
	}

	/* Muxing operation and Frame creation */
	/* Encoding frame */
	send_data = _cmux_encode_cmux_frame(cmux_obj, data, data_len, channel_id,
					CMUX_COMMAND_UIH, 0x1, 0x1, 0x0, &len);
	if (len == 0) {
		err("Failed to encode");
		return TCORE_RETURN_FAILURE;
	}

	/* Send CMUX data */
	ret = _cmux_send_data(cmux_obj->phy_hal, len, send_data);

	dbg("Exit");
	return ret;
}

static TReturn _cmux_hal_setup_netif(CoreObject *co,
				TcoreHalSetupNetifCallback func, void *user_data,
				unsigned int cid, gboolean enable)
{
	tcore_cmux_object *cmux_obj;
	TcoreHal *hal;
	dbg("Entry");

	/* Get secondary HAL from Core Object */
	hal = tcore_object_get_hal(co);

	/*
	 * Get CMUX Object from Modem Interface Plug-in
	 */
	cmux_obj = _cmux_get_cmux_object(tcore_hal_ref_plugin(hal));
	if (cmux_obj == NULL)
		return TCORE_RETURN_FAILURE;

	return tcore_hal_setup_netif(cmux_obj->phy_hal,
								co, func, user_data, cid, enable);
}

/* CMUX supported HAL (Logical HAL) operations */
static struct tcore_hal_operations cmux_hops = {
	.power = _cmux_hal_power,
	.send = _cmux_hal_send,
	.setup_netif = _cmux_hal_setup_netif,
};

static TcoreHal *_cmux_create_logical_hal(tcore_cmux_object *cmux_obj,
								tcore_cmux_channel *channel)
{
	TcoreHal *hal;
	char channel_id_name[CMUX_MAX_CHANNEL_NAME];
	dbg("Entry");

	if ((cmux_obj == NULL) || (channel == NULL)) {
		err("Invalid input parameters");
		return NULL;
	}

	/* Creating Logical HAL for Core Object - Mode - 'AT mode' */
	snprintf(channel_id_name, sizeof(channel_id_name),
						"channel_%d", channel->channel_id);

	/* Creating Logical HAL */
	hal = tcore_hal_new(cmux_obj->plugin,
					channel_id_name, &cmux_hops, TCORE_HAL_MODE_AT);
	dbg("hal: %p", hal);
	if (hal == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	/* Updating Logical HAL of CMUX Channel */
	channel->hal = hal;

	dbg("Exit");
	return hal;
}

tcore_cmux_object *_cmux_new(int max_channels, unsigned int buffer_size)
{
	tcore_cmux_object *cmux_obj;
	int i;
	dbg("Entry");

	/* Allocating memory for mux */
	cmux_obj = (tcore_cmux_object *)g_try_new0(tcore_cmux_object, 1);
	if (cmux_obj == NULL) {
		err("Failed to allocate memory");
		return NULL;
	}

	/* Allocating memory for info_field */
	cmux_obj->internal_mux.info_field =
					(unsigned char *)g_try_malloc0(buffer_size);
	if (cmux_obj->internal_mux.info_field == NULL) {
		err("Failed to allocate memory for info field");
		goto ERROR;
	}

	/* CMUX State initialize to CMUX_NOT_INITIALIZED */
	cmux_obj->cmux_state = CMUX_NOT_INITIALIZED;

	/*
	 * Allocating memory for channel_info
	 * max_channels defines the maximum channels user requested,
	 * hence + 1 is required for CMUX Control Channel
	 */
	for (i = 0; i < (max_channels + 1) ; i++) {
		/* Allocating memory for channel_info */
		cmux_obj->internal_mux.channel_info[i] =
				(tcore_cmux_channel *)g_try_new0(tcore_cmux_channel, 1);
		if (cmux_obj->internal_mux.channel_info[i] == NULL) {
			err("Failed to allocate memory for channel_info of channel: [%d]", i);
			goto ERROR;
		}
	}

	dbg("Exit");
	return cmux_obj;

ERROR:
	/* Free allocated memory */
	if (cmux_obj != NULL) {
		/* Free info_field */
		g_free(cmux_obj->internal_mux.info_field);

		for (i = 0; i < (max_channels + 1) ; i++)
			/* Free channel_info */
			g_free(cmux_obj->internal_mux.channel_info[i]);

		/* Free CMUX object memory */
		g_free(cmux_obj);
	}

	err("Exit");
	return NULL;
}

static void _cmux_channel_init(tcore_cmux_object *cmux_obj, int channel_id)
{
	tcore_cmux_channel *ch;
	dbg("Entry");

	ch = cmux_obj->internal_mux.channel_info[channel_id];
	if (ch == NULL)
		return;

	ch->channel_id = channel_id;
	ch->channel_state = CMUX_CHANNEL_SABM_SEND_WAITING_FOR_UA;

	ch->hal = NULL;

	/* TODO - Check if required */
	ch->frame_type = CMUX_COMMAND_SABM;
	ch->ext_bit = 0x01;
	ch->cr_bit = 0x01;
	ch->poll_final_bit = 0x01;

	dbg("Channel ID [%d] - Initialized", channel_id);
}

static void _cmux_close_channel(tcore_cmux_object *cmux_obj, int channel_id)
{
	tcore_cmux_channel *ch;
	unsigned char *send_data;
	int ret, len = 0;
	dbg("Entry");

	ch = cmux_obj->internal_mux.channel_info[channel_id];
	if (ch == NULL)
		return;

	if (ch->channel_state != CMUX_CHANNEL_CLOSED) {
		ch->frame_type = CMUX_COMMAND_DISC;
		ch->ext_bit = 0x01;
		ch->cr_bit = 0x01;
		ch->channel_state = CMUX_CHANNEL_DISC_SEND_WAITING_FOR_UA;

		/* Send DSC command */
		/* Encoding frame */
		send_data = _cmux_encode_cmux_frame(cmux_obj, NULL, 0, channel_id,
						CMUX_COMMAND_DISC, 0x01, 0x01, 0x01, &len);
		if (len != 0)
			/* Send CMUX data */
			ret = _cmux_send_data(cmux_obj->phy_hal, len, send_data);
		else
			err("Failed to encode");
	} else
		/* Channel is already closed */
		err("Channel is already closed");

	dbg("Exit");
}

static void _cmux_free(tcore_cmux_object *cmux_obj)
{
	int channel;
	dbg("Entry");

	/*
	 * This function is used internally only, hence sanity check for 'cmux_obj'
	 * is NOT required.
	 */
	/* Free Information Field */
	g_free(cmux_obj->internal_mux.info_field);
	cmux_obj->internal_mux.info_field = NULL;

	for (channel = 0; channel < cmux_obj->max_cmux_channels; channel++) {
		/* Free Channel Information */
		g_free(cmux_obj->internal_mux.channel_info[channel]);
		cmux_obj->internal_mux.channel_info[channel] = NULL;
	}

	/* Free CMUX Object */
	g_free(cmux_obj);
	cmux_obj = NULL;

	dbg("Exit");
}

static void _cmux_on_confirmation_message_send(TcorePending *plugin,
											gboolean result, void *user_data)
{
	dbg("Message out from queue");

	if (result == FALSE) {		/* Fail */
		err("SEND FAIL");
	} else
		dbg("SEND OK");
}

void tcore_cmux_rcv_from_hal(TcoreHal *hal, unsigned char *data, size_t length)
{
#define TCORE_CMUX_DECODE_FLAG_HUNT			0
#define TCORE_CMUX_DECODE_ADDR_HUNT			1
#define TCORE_CMUX_DECODE_CONTROL_HUNT		2
#define TCORE_CMUX_DECODE_LENGTH1_HUNT		3
#define TCORE_CMUX_DECODE_LENGTH2_HUNT		4
#define TCORE_CMUX_DECODE_DATA_HUNT			5
#define TCORE_CMUX_DECODE_FCS_HUNT			6

	tcore_cmux_object *cmux_obj;

	static int decode_state = TCORE_CMUX_DECODE_FLAG_HUNT;
	static unsigned char dec_fcs;
	static unsigned char *dec_data;
	static unsigned short dec_length;
	static size_t full_frame_len;

	size_t pos = -1;
	int cp_len;
	dbg("Entry");

	/*
	 * Get CMUX Object from Modem Interface Plug-in
	 */
	cmux_obj = _cmux_get_cmux_object(tcore_hal_ref_plugin(hal));
	if (cmux_obj == NULL)
		return;

DECODE_STATE_CHANGE:
	if (++pos >= length)
		return;

	switch(decode_state) {
		case TCORE_CMUX_DECODE_FLAG_HUNT:
			full_frame_len = 0;
			dec_length = 0;
			dec_fcs = 0xFF;
			dec_data = cmux_obj->cmux_buffer;
			goto FLAG_HUNT;
		break;
		case TCORE_CMUX_DECODE_ADDR_HUNT:
			goto ADDR_HUNT;
		break;
		case TCORE_CMUX_DECODE_CONTROL_HUNT:
			goto CONTROL_HUNT;
		break;
		case TCORE_CMUX_DECODE_LENGTH1_HUNT:
			goto LENGTH1_HUNT;
		break;
		case TCORE_CMUX_DECODE_LENGTH2_HUNT:
			goto LENGTH2_HUNT;
		break;
		case TCORE_CMUX_DECODE_DATA_HUNT:
			goto DATA_HUNT;
		break;
		case TCORE_CMUX_DECODE_FCS_HUNT:
			goto FCS_HUNT;
		break;
	}

FLAG_HUNT:
	while (data[pos] != CMUX_FRAME_DELIMITER)
		if (++pos >= length)
			return;

	decode_state = TCORE_CMUX_DECODE_ADDR_HUNT;
	goto DECODE_STATE_CHANGE;

ADDR_HUNT:
	while (data[pos] == CMUX_FRAME_DELIMITER)
		if (++pos >= length)
			return;

	dec_fcs = crc_table[dec_fcs^data[pos]];
	decode_state = TCORE_CMUX_DECODE_CONTROL_HUNT;
	*dec_data++ = CMUX_FRAME_DELIMITER;
	*dec_data++ = data[pos];
	full_frame_len += 2;
	goto DECODE_STATE_CHANGE;

CONTROL_HUNT:
	dec_fcs = crc_table[dec_fcs^data[pos]];
	decode_state = TCORE_CMUX_DECODE_LENGTH1_HUNT;
	*dec_data++ = data[pos];
	full_frame_len++;
	goto DECODE_STATE_CHANGE;

LENGTH1_HUNT:
	dec_length = data[pos] >> 1;
	dec_fcs = crc_table[dec_fcs^data[pos]];
	if (data[pos] & 0x1)		/* EA */
		if (dec_length > 0)
			decode_state = TCORE_CMUX_DECODE_DATA_HUNT;
		else
			decode_state = TCORE_CMUX_DECODE_FCS_HUNT;
	else
		decode_state = TCORE_CMUX_DECODE_LENGTH2_HUNT;

	*dec_data++ = data[pos];
	full_frame_len++;
	goto DECODE_STATE_CHANGE;

LENGTH2_HUNT:
	dec_length |= ((unsigned short)data[pos] << 7);
	dec_fcs = crc_table[dec_fcs^data[pos]];
	decode_state = TCORE_CMUX_DECODE_DATA_HUNT;
	*dec_data++ = data[pos];
	full_frame_len++;
	goto DECODE_STATE_CHANGE;

DATA_HUNT:
	if (dec_length < (length - pos)) {	/* frame data fully available in the buffer */
		cp_len = dec_length;
		decode_state = TCORE_CMUX_DECODE_FCS_HUNT;
	} else {						/* frame data partially available in the buffer */
		cp_len = (length - pos);
		decode_state = TCORE_CMUX_DECODE_DATA_HUNT;
	}

	memcpy(dec_data, data + pos, cp_len);
	dec_data += cp_len;
	pos += (cp_len - 1);
	dec_length -= cp_len;
	full_frame_len += cp_len;

	goto DECODE_STATE_CHANGE;

FCS_HUNT:
	*dec_data++ = data[pos];
	*dec_data++ = CMUX_FRAME_DELIMITER;
	full_frame_len += 2;
	_cmux_process_rcv_frame(cmux_obj, full_frame_len);

	/* enter flag hunt mode */
	decode_state = TCORE_CMUX_DECODE_FLAG_HUNT;
	goto DECODE_STATE_CHANGE;
}

/* CMUX initialization */
TReturn tcore_cmux_init(TcoreHal *phy_hal, unsigned int frame_size,
					TcorePendingResponseCallback resp_cb, void *resp_cb_data)
{
	TcorePending *pending = NULL;
	TcoreATRequest *req = NULL;
	TReturn ret = TCORE_RETURN_FAILURE;
	char *cmd_str;
	dbg("Entry");

	if ((phy_hal == NULL) || (resp_cb == NULL))
		return TCORE_RETURN_EINVAL;

	if (frame_size > 32768)
		return TCORE_RETURN_EINVAL;

	/*
	 * 3GPP 27.010 Section 5.7.2 Maximum Frame Size (N1)
	 * If frame size is greater than Zero then,
	 * the request is sent for frame size
	 * else,
	 * request is sent for 'default' frame size
	 * (default Value: 31 (64 if Advanced option is used)).
	 */
	if (frame_size > 0)
		cmd_str = g_strdup_printf("AT+CMUX=0,0,%d,1509,10,3,30,,", frame_size);
	else
		cmd_str = g_strdup_printf("AT+CMUX=0,0,,1509,10,3,30,,");

	if (cmd_str == NULL)
		return TCORE_RETURN_ENOMEM;

	/* Create Pending Request */
	pending = tcore_pending_new(NULL, 0);
	if (pending == NULL) {
		dbg("Pending is NULL");

		g_free(cmd_str);
		return ret;
	}

	/* Create AT-Command Request */
	req = tcore_at_request_new(cmd_str, "+CMUX:", TCORE_AT_NO_RESULT);

	/* Free command string */
	g_free(cmd_str);

	if (req == NULL) {
		dbg("Request is NULL");
		tcore_pending_free(pending);
		return ret;
	}
	dbg("AT Command: [%s], Prefix(if any): [%s], AT-Command length: [%d]", req->cmd, req->prefix, strlen(req->cmd));

	tcore_pending_set_request_data(pending, 0, req);
	tcore_pending_set_response_callback(pending, resp_cb, resp_cb_data);
	tcore_pending_set_send_callback(pending, _cmux_on_confirmation_message_send, NULL);

	ret = tcore_hal_send_request(phy_hal, pending);

	dbg("Exit");
	return ret;
}

/* Setup Internal CMUX */
TReturn tcore_cmux_setup_internal_mux(tcore_cmux_mode mode,
	int max_channels, unsigned int cmux_buf_size, TcoreHal *phy_hal,
	cmux_setup_cb_func channel_setup_cb, gpointer channel_setup_user_data,
	cmux_setup_complete_cb_func setup_complete_cb, gpointer setup_complete_user_data)
{
	tcore_cmux_object *cmux_obj;
	unsigned char *data;
	int data_len;
	int index;

	TReturn ret = TCORE_RETURN_FAILURE;
	dbg("Entry");
	dbg("Internal CMUX setup request");

	/* Sanity Check */
	if ((cmux_buf_size <= 0)
			|| (phy_hal == NULL)
			|| (channel_setup_cb == NULL)
			|| (setup_complete_cb == NULL)) {
		err("CMUX Buffer size: [%d] Physical HAL: [0x%x] setup_complete_cb: [0x%x]",
			cmux_buf_size, (unsigned int)phy_hal, setup_complete_cb);
		return TCORE_RETURN_EINVAL;
	}

	dbg("Physical HAL: [0x%x] cmux_buf_size: [%d]",
								(unsigned int)phy_hal, cmux_buf_size);

	/*
	 * Max Channels
	 *	(+ 1) is for CMUX Control Channel
	 */
	if ((max_channels +1) >= CMUX_CHANNEL_MAX) {
		err("Number of Channels requested is greater than supported");
		return TCORE_RETURN_EINVAL;
	}

	/* Create new CMUX Object */
	cmux_obj = _cmux_new(max_channels, cmux_buf_size);
	if (cmux_obj == NULL) {
		err("Failed to create CMUX object");
		return TCORE_RETURN_ENOMEM;
	}

	/* Maximum Buffer size for CMUX frame processing */
	cmux_obj->max_cmux_buf_size = cmux_buf_size;

	/* Maximum Channels requested */
	cmux_obj->max_cmux_channels = max_channels + 1;

	/* Allocating buffer for CMUX frames proposing */
	cmux_obj->cmux_buffer = (unsigned char *)g_try_malloc0(cmux_buf_size);
	if (cmux_obj->cmux_buffer == NULL) {
		err("Failed to allocate memory");

		ret = TCORE_RETURN_ENOMEM;
		goto ERROR;
	}

	/* Update 'mode' */
	cmux_obj->cmux_mode = mode;
	dbg("Mode: \'%s\'", (mode == CMUX_MODE_BASIC ? "Basic" : "Advanced"));

	/* Save Physical HAL */
	cmux_obj->phy_hal = phy_hal;

	/* Save Modem Interface Plug-in */
	cmux_obj->plugin = tcore_hal_ref_plugin(phy_hal);

	/* CMUX setup callback function */
	cmux_obj->internal_mux.channel_setup_cb = channel_setup_cb;
	cmux_obj->internal_mux.channel_setup_user_data = channel_setup_user_data;

	/* CMUX setup complete callback function */
	cmux_obj->internal_mux.setup_complete_cb = setup_complete_cb;
	cmux_obj->internal_mux.setup_complete_user_data = setup_complete_user_data;

	/*
	 * After CMUX setup, AT parse functionality of PHY HAL should be disabled,
	 * here we change the mode of PHYSICAL HAL to Transparent from AT.
	 */
	tcore_hal_set_mode(phy_hal, TCORE_HAL_MODE_TRANSPARENT);
	dbg("Physical HAL mode changed to Transparent");

	/* Adding CMUX object to Global List */
	g_cmux_obj = g_slist_insert(g_cmux_obj, cmux_obj, 0);

	/* Initialize all the Channels for the CMUX object */
	/* Open all Channels */
	for (index = 0; index < cmux_obj->max_cmux_channels; index++) {
		dbg("Initializing CMUX Channel [%d]", index);
		_cmux_channel_init(cmux_obj, index);

		dbg("Opening Channel ID [%d]", index);
		/* Encode CMUX Frame */
		data = _cmux_encode_cmux_frame(cmux_obj, NULL, 0, index,
					CMUX_COMMAND_SABM, 0x01, 0x01, 0x01, &data_len);
		if (data_len == 0) {
			err("Failed to encode");
			goto ERROR;
		}
		dbg("data_len: [%d] data: [%s]", data_len, data);

		/* Send CMUX data */
		ret = _cmux_send_data(cmux_obj->phy_hal, data_len, data);
		if (ret != TCORE_RETURN_SUCCESS) {
			err("Failed to send CMUX Control Request for Channel ID: [%d]", index);
			goto ERROR;
		} else
			dbg("CMUX Control Request sent to CP");
	}

	dbg("Exit");
	return ret;

ERROR:
	/* Free the allocated CMUX memory */
	_cmux_free(cmux_obj);

	err("Exit");
	return ret;
}

/* Close CMUX */
void tcore_cmux_close(TcoreHal *phy_hal,
	cmux_channel_close_cb_func channel_close_cb, gpointer channel_close_user_data)
{
	tcore_cmux_object *cmux_obj;
	int channel_id;
	dbg("Entry");

	/*
	 * Get CMUX Object from Modem Interface Plug-in
	 */
	cmux_obj = _cmux_get_cmux_object(tcore_hal_ref_plugin(phy_hal));
	if (cmux_obj == NULL)
		return;

	/* Internal CMUX */
	dbg("Closing Internal CMUX");

	/* Close all Channels */
	for (channel_id = 0;
			channel_id < cmux_obj->max_cmux_channels;
			channel_id++) {
		dbg("Closing Channel - ID: [%d]", channel_id);

		/* Close Channel - Send DSC command */
		_cmux_close_channel(cmux_obj, channel_id);

		/* Invoke callback */
		if (channel_close_cb != NULL)
			channel_close_cb(cmux_obj->internal_mux.channel_info[channel_id]->hal,
						channel_close_user_data);

		/* Free Logical HAL for Channel */
		tcore_hal_free(cmux_obj->internal_mux.channel_info[channel_id]->hal);
		cmux_obj->internal_mux.channel_info[channel_id]->hal = NULL;
	}

	/* Freeing CMUX frame processing buffer */
	g_free(cmux_obj->cmux_buffer);
	cmux_obj->cmux_buffer = NULL;

	/* Change the mode of PHYSICAL HAL from Transparent to AT */
	tcore_hal_set_mode(cmux_obj->phy_hal, TCORE_HAL_MODE_AT);

	/* Remove from list */
	g_cmux_obj = g_slist_remove(g_cmux_obj, cmux_obj);

	/* Free all the allocated memory */
	_cmux_free(cmux_obj);

	dbg("Exit");
}
