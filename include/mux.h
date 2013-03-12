/*
 * libtcore
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
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

#ifndef __MUX_H__
#define __MUX_H__

#include "queue.h"

/* CMUX modes */
typedef enum {
	CMUX_MODE_BASIC = 0x00,
	CMUX_MODE_ADVANCED
} tcore_cmux_mode;

/* CMUX Channel */
typedef struct cmux_channel tcore_cmux_channel;

/* Global MUX Object */
typedef struct cmux_obj tcore_cmux_object;

/* Internal CMUX setup complete callback prototype */
typedef void (*cmux_setup_complete_cb_func) (gpointer user_data);

/* CMUX setup callback prototype */
typedef void (*cmux_setup_cb_func) (int channel_id, TcoreHal *hal,
									gpointer user_data);

/* CMUX initialization - Internal and Kernel */
TReturn tcore_cmux_init(TcoreHal *phy_hal, unsigned int frame_size,
					TcorePendingResponseCallback resp_cb, void *resp_cb_data);

/* Setup Internal CMUX */
TReturn tcore_cmux_setup_internal_mux(tcore_cmux_mode mode,
	int max_channels, unsigned int cmux_buf_size, TcoreHal *phy_hal,
	cmux_setup_cb_func channel_setup_cb, gpointer channel_setup_user_data,
	cmux_setup_complete_cb_func setup_complete_cb, gpointer setup_complete_user_data);

/* Close CMUX */
void tcore_cmux_close(TcoreHal *phy_hal);

/* HAL Receive for Internal CMUX */
void tcore_cmux_rcv_from_hal(TcoreHal *hal, unsigned char *data, size_t length);

#endif  /* __MUX_H__ */
