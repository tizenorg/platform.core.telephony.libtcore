/*
 * libtcore
 *
 * Copyright (c) 2013 Samsung Electronics Co. Ltd. All rights reserved.
 * Copyright (c) 2013 Intel Corporation. All rights reserved.
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

#ifndef __MUX_H__
#define __MUX_H__

#include "queue.h"

/* CMUX modes */
typedef enum {
	CMUX_MODE_BASIC = 0x00,
	CMUX_MODE_ADVANCED
} tcore_cmux_mode;

/* CMUX Channel */
typedef struct cmux_channel TcoreCmuxChannel;

/* Global MUX Object */
typedef struct cmux_obj TcoreCmuxObject;

/* CMUX setup complete callback prototype */
typedef void (*CmuxSetupCompleteCbFunc) (gpointer user_data);

/* CMUX setup callback prototype */
typedef void (*CmuxSetupCbFunc) (guint channel_id,
	TcoreHal *hal, gpointer user_data);

/* CMUX Channel close callback prototype */
typedef void (*CmuxChannelCloseCbFunc) (TcoreHal *hal, gpointer user_data);

/* CMUX initialization - Internal and Kernel */
TelReturn tcore_cmux_init(TcoreHal *phy_hal, guint frame_size,
	TcorePendingResponseCallback resp_cb, void *resp_cb_data);

/* Setup Internal CMUX */
TelReturn tcore_cmux_setup_internal_mux(tcore_cmux_mode mode,
	guint max_channels, guint cmux_buf_size, TcoreHal *phy_hal,
	CmuxSetupCbFunc channel_setup_cb, gpointer channel_setup_cb_data,
	CmuxSetupCompleteCbFunc setup_complete_cb, gpointer setup_complete_cb_data);

/* Close CMUX */
void tcore_cmux_close(TcoreHal *phy_hal,
	CmuxChannelCloseCbFunc channel_close_cb, gpointer channel_close_cb_data);

/* HAL Receive for Internal CMUX */
void tcore_cmux_rcv_from_hal(TcoreHal *hal, unsigned char *data, size_t length);

#endif  /* __MUX_H__ */
