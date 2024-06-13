/* SPDX-License-Identifier: BSD-3-Clause */
/*****************************************************************************
* Copyright 2024, Fraunhofer Institute for Secure Information Technology SIT.
* All rights reserved.
****************************************************************************/

/**
* @file cddlEncoder.h
* @author Anselm Angert (anselm.angert@sit.fraunhofer.de)
* @brief
* @version 1.0
* @date 2024-05-15
*
* @copyright Copyright 2024, Fraunhofer Institute for Secure Information
* Technology SIT. All rights reserved.
*
* @license BSD 3-Clause "New" or "Revised" License (SPDX-License-Identifier:
* BSD-3-Clause).
*/

#ifndef INC_CDDLENCODER_H_
#define INC_CDDLENCODER_H_

#include "analogMeasurementTypes.h"
#include "qcbor.h"
#include "fingerprinter.h"

QCBORError encodeAnalogMeasurement(UsefulBuf EngineBuffer, struct AnalogMeasurement *dataIn, UsefulBufC *EncodedCBOR, UART_HandleTypeDef *huart);

QCBORError convert_to_cbor(Fingerprinter *fingerprint, UsefulBufC *buffer);

#endif /* INC_CDDLENCODER_H_ */
