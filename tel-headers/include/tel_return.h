/*
 * tel-headers
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

#ifndef __TEL_RETURN_H__
#define __TEL_RETURN_H__

/**
*  @addtogroup TAPI_COMMON
*  @{
*
*  @file tel_return.h
*  @brief TAPI return values
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum TelReturn
 * TAPI return values
 */
typedef enum {
	TEL_RETURN_SUCCESS, /**< Operation completed successfully */
	TEL_RETURN_FAILURE, /**< Operation Failed */
	TEL_RETURN_INVALID_PARAMETER, /**< Invalid input parameters */
	TEL_RETURN_MEMORY_FAILURE, /**< Memory allocation failed */
	TEL_RETURN_OPERATION_NOT_SUPPORTED, /**< Operation not supported */
	TEL_RETURN_UNKNOWN_FAILURE /**< Unknown failure */
} TelReturn;

#ifdef __cplusplus
}
#endif

/** @}*/

#endif /* __TEL_RETURN_H__ */
