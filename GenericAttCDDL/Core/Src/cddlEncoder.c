/* SPDX-License-Identifier: BSD-3-Clause */
/*****************************************************************************
* Copyright 2024, Fraunhofer Institute for Secure Information Technology SIT.
* All rights reserved.
****************************************************************************/

/**
* @file cddlEncoder.c
* @author Anselm Angert (anselm.angert@sit.fraunhofer.de)
* @brief
* @version 1.0
* @date 2024-05-04
*
* @copyright Copyright 2024, Fraunhofer Institute for Secure Information
* Technology SIT. All rights reserved.
*
* @license BSD 3-Clause "New" or "Revised" License (SPDX-License-Identifier:
* BSD-3-Clause).
*/

#include <stdio.h>//snprintf
#include "stm32l4xx_hal.h"

#include <cddlEncoder.h>
//#define CALCULATE_BUF_SIZE //determine the required size of EngineBuffer

void encodeTime(QCBOREncodeContext *pCtx, struct Time *time, bool openInMap, int mapValue, UART_HandleTypeDef *huart) {
	if (openInMap) {
		QCBOREncode_OpenArrayInMapN(pCtx, mapValue);
	}else {
		QCBOREncode_OpenArray(pCtx);//?start-time: Time
	}
	if (time->Time_seconds_choice == Time_seconds_uint_c) {
		QCBOREncode_AddUInt64(pCtx, time->Time_seconds_uint);
	}else if (time->Time_seconds_choice == Time_seconds_float_c) {
		QCBOREncode_AddDouble(pCtx, time->Time_seconds_float);//encodes floating-point number with minimal possible precision (lowest possible space without lost of precision)
	}else {
		char string_buf [60];
		snprintf(string_buf, 60, "[ERROR] invalid value for time.Time_seconds_choice: %d\n", time->Time_seconds_choice);
		print_string(huart, string_buf);
	}
	QCBOREncode_AddInt64(pCtx, time->Time_unit_mult);
	QCBOREncode_CloseArray(pCtx);//?start-time: Time
}

void encodeParams(QCBOREncodeContext *pCtx, struct Params *nvPair, UART_HandleTypeDef *huart) {
	QCBOREncode_OpenArray(pCtx);//...-params: [ * NameValuePair ]
	for (size_t j = 0; j < nvPair->Params_m_count; j++) {
		struct NameValuePair *tmpNV = &(nvPair->Params_NameValuePair_m[j]);
		QCBOREncode_AddText(pCtx, tmpNV->NameValuePair_name);
		switch (tmpNV->NameValuePair_value.AnyType_union_choice) {
		case AnyType_tstr_c:
			QCBOREncode_AddText(pCtx, tmpNV->NameValuePair_value.AnyType_tstr);
			break;

		case AnyType_bstr_c:
			QCBOREncode_AddBytes(pCtx, tmpNV->NameValuePair_value.AnyType_bstr);
			break;

		case AnyType_int_c:
			QCBOREncode_AddInt64(pCtx, tmpNV->NameValuePair_value.AnyType_int);
			break;

		case AnyType_uint_c:
			QCBOREncode_AddUInt64(pCtx, tmpNV->NameValuePair_value.AnyType_uint);
			break;

		case AnyType_float_c:
			QCBOREncode_AddDouble(pCtx, tmpNV->NameValuePair_value.AnyType_float);
			break;

		case AnyType_bool_c:
			QCBOREncode_AddBool(pCtx, tmpNV->NameValuePair_value.AnyType_bool);
			break;

		case AnyType_empty_c:
			break;

		case AnyType_not_supported_type_c:
		default:
			char string_buf [80];
			snprintf(string_buf, 80, "[ERROR] unsupported value for NameValuePair_value.AnyType_union_choice: %d\n", tmpNV->NameValuePair_value.AnyType_union_choice);
			print_string(huart, string_buf);
			break;
		}
	}
	QCBOREncode_CloseArray(pCtx);//...-params: [ * NameValuePair ]
}

void encodeNumericalValue(QCBOREncodeContext *pCtx, struct NumericalValue_value_r *nv, UART_HandleTypeDef *huart) {
	if (nv->NumericalValue_value_choice == NumericalValue_value_int_c) {
		QCBOREncode_AddInt64(pCtx, nv->NumericalValue_value_int);
	}else if (nv->NumericalValue_value_choice == NumericalValue_value_float_c) {
		QCBOREncode_AddDouble(pCtx, nv->NumericalValue_value_float);
	}else {
		char string_buf [60];
		snprintf(string_buf, 60, "[ERROR] invalid value for NumericalValue_value_choice: %d\n", nv->NumericalValue_value_choice);
		print_string(huart, string_buf);
	}
}

QCBORError encodeAnalogMeasurement(UsefulBuf EngineBuffer, struct AnalogMeasurement *dataIn, UsefulBufC *EncodedCBOR, UART_HandleTypeDef *huart) {
	QCBOREncodeContext EncodeCtx;
#ifndef CALCULATE_BUF_SIZE
	QCBOREncode_Init(&EncodeCtx, EngineBuffer);
#else
	QCBOREncode_Init(&EncodeCtx, SizeCalculateUsefulBuf);
#endif
	QCBOREncode_OpenArray(&EncodeCtx);//AnalogMeasurement
	QCBOREncode_AddUInt64(&EncodeCtx, dataIn->AnalogMeasurement_version_tag);
	encodeTime(&EncodeCtx, &(dataIn->AnalogMeasurement_start_time), false, 0, huart);//start-time: Time
	QCBOREncode_OpenArray(&EncodeCtx);//measurements: [ * MeasurementSeries ]
	for (size_t i = 0; i < dataIn->AnalogMeasurement_measurements_MeasurementSeries_m_count; i++) {
		struct MeasurementSeries *tmpMs = &(dataIn->AnalogMeasurement_measurements_MeasurementSeries_m[i]);
		QCBOREncode_OpenArray(&EncodeCtx);//target: Target
		QCBOREncode_AddText(&EncodeCtx, tmpMs->MeasurementSeries_target.Target_id);//works like QCBOREncode_AddSZString; CBOR major type 3
		if (tmpMs->MeasurementSeries_target.Target_config_params_present) {
			encodeParams(&EncodeCtx, &(tmpMs->MeasurementSeries_target.Target_config_params), huart);//config-params: [ * NameValuePair ]
		}
		QCBOREncode_CloseArray(&EncodeCtx);//target: Target
		if (tmpMs->MeasurementSeries_env_params_present) {
			encodeParams(&EncodeCtx, &(tmpMs->MeasurementSeries_env_params), huart);//?env-params: [ * NameValuePair ]
		}
		if (tmpMs->MeasurementSeries_start_time_present) {
			encodeTime(&EncodeCtx, &(tmpMs->MeasurementSeries_start_time), false, 0, huart);//?start-time: Time
		}
		//encode `unit: Unit` as one giant enum
		if (tmpMs->MeasurementSeries_unit.Unit_choice == Unit_UnitElectricalSi_m_c) {
			QCBOREncode_AddUInt64(&EncodeCtx, tmpMs->MeasurementSeries_unit.Unit_UnitElectricalSi_m);
		}else {
			QCBOREncode_AddUInt64(&EncodeCtx, Unit_UNIT_UNDEFINED_c);//manually hardcoded as 0 in header
		}
		QCBOREncode_AddInt64(&EncodeCtx, tmpMs->MeasurementSeries_unit_multiple);//unit-multiple: UnitMultiple
		if (tmpMs->MeasurementSeries_union_choice == MeasurementSeries_union_RegularMeasurementSeries_c) {//RegularMeasurementSeries
			QCBOREncode_OpenMap(&EncodeCtx);//measurements: RegularMeasurementSeries
			QCBOREncode_OpenArrayInMapN(&EncodeCtx, values_map);//values => [ * NumericalValue ]
			for (size_t j = 0; j < tmpMs->MeasurementSeries_union_RegularMeasurements.RegularMeasurementSeries_values_NumericalValue_m_count; j++) {
				encodeNumericalValue(&EncodeCtx, &(tmpMs->MeasurementSeries_union_RegularMeasurements.RegularMeasurementSeries_values_NumericalValue_m[j]), huart);
			}
			QCBOREncode_CloseArray(&EncodeCtx);//values => [ * NumericalValue ]
			struct interval_frequency_duration_r *tmpIFD = &(tmpMs->MeasurementSeries_union_RegularMeasurements.RegularMeasurementSeries_interval_frequency_duration_m);
			if (tmpIFD->interval_frequency_duration_choice == interval_frequency_duration_interval_c) {//interval => Time
				encodeTime(&EncodeCtx, &(tmpIFD->interval_frequency_duration_interval), true, interval_frequency_duration_interval_c, huart);
			}else if (tmpIFD->interval_frequency_duration_choice == interval_frequency_duration_frequency_c) {//frequency => Frequency
				QCBOREncode_OpenArrayInMapN(&EncodeCtx, interval_frequency_duration_frequency_c);//Frequency
				if (tmpIFD->interval_frequency_duration_frequency.Frequency_hertz_choice == Frequency_hertz_uint_c) {
					QCBOREncode_AddUInt64(&EncodeCtx, tmpIFD->interval_frequency_duration_frequency.Frequency_hertz_uint);
				}else if (tmpIFD->interval_frequency_duration_frequency.Frequency_hertz_choice == Frequency_hertz_float_c) {
					QCBOREncode_AddDouble(&EncodeCtx, tmpIFD->interval_frequency_duration_frequency.Frequency_hertz_float);
				}else {
					char string_buf [60];
					snprintf(string_buf, 60, "[ERROR] invalid value for Frequency_hertz_choice: %d\n", tmpIFD->interval_frequency_duration_frequency.Frequency_hertz_choice);
					print_string(huart, string_buf);
				}
				QCBOREncode_AddInt64(&EncodeCtx, tmpIFD->interval_frequency_duration_frequency.Frequency_unit_multiple);
				QCBOREncode_CloseArray(&EncodeCtx);//Frequency
			} else if (tmpIFD->interval_frequency_duration_choice == interval_frequency_duration_duration_c) {//duration => Time
				encodeTime(&EncodeCtx, &(tmpIFD->interval_frequency_duration_duration), true, interval_frequency_duration_duration_c, huart);
			}else {
				char string_buf [70];
				snprintf(string_buf, 70, "[ERROR] invalid value for interval_frequency_duration_choice: %d\n", tmpIFD->interval_frequency_duration_choice);
				print_string(huart, string_buf);
			}
			QCBOREncode_CloseMap(&EncodeCtx);//measurements: RegularMeasurementSeries
		}else if (tmpMs->MeasurementSeries_union_choice == MeasurementSeries_union_IrregularMeasurementSeries_c) {//IrregularMeasurementSeries
			QCBOREncode_OpenArray(&EncodeCtx);//measurements: IrregularMeasurementSeries
			for (size_t j = 0; j < tmpMs->MeasurementSeries_union_IrregularMeasurementSeries_m.IrregularMeasurementSeries_internal_l_count; j++) {
				struct IrregularMeasurementSeries_internal_l *tmpIms = &(tmpMs->MeasurementSeries_union_IrregularMeasurementSeries_m.IrregularMeasurementSeries_internal_l[j]);
				encodeTime(&EncodeCtx, &(tmpIms->IrregularMeasurementSeries_internal_l_current_time), false, 0, huart);//current-time: Time
				encodeNumericalValue(&EncodeCtx, &(tmpIms->IrregularMeasurementSeries_internal_l_NumericalValue_m), huart);//NumericalValue
			}
			QCBOREncode_CloseArray(&EncodeCtx);//measurements: IrregularMeasurementSeries
		}else {
			char string_buf [70];
			snprintf(string_buf, 70, "[ERROR] invalid value for MeasurementSeries_union_choice: %d\n", tmpMs->MeasurementSeries_union_choice);
			print_string(huart, string_buf);
		}
	}
	QCBOREncode_CloseArray(&EncodeCtx);//measurements: [ * MeasurementSeries ]
	QCBOREncode_CloseArray(&EncodeCtx);//AnalogMeasurement
#ifndef CALCULATE_BUF_SIZE
	return QCBOREncode_Finish(&EncodeCtx, EncodedCBOR);
#else
	size_t uEncodedLen = 0;//only used for calculating size of Buffer
	QCBORError uErr = QCBOREncode_FinishGetSize(&EncodeCtx, &uEncodedLen);
	char string_buf [40];
	snprintf(string_buf, 40, "necessary size calculated as %u bytes\n", uEncodedLen);
	print_string(huart, string_buf);
	return uErr;
#endif
}


QCBORError convert_to_cbor(Fingerprinter *fingerprint, UsefulBufC *buffer) {
	struct AnalogMeasurement tmp = {
		.AnalogMeasurement_version_tag = 1,
		.AnalogMeasurement_start_time = {
			.Time_seconds_choice = Time_seconds_uint_c,
			.Time_seconds_uint = 0,
			.Time_unit_mult = UNIT_MULTIPLE_SI_MILLI_c
		},
		.AnalogMeasurement_measurements_MeasurementSeries_m_count = fingerprint->num_of_samples,
	};
	for (size_t i=0; i<fingerprint->num_of_samples; i++) {
		struct MeasurementSeries tmpMS = {
			.MeasurementSeries_target = {
				.Target_id = UsefulBuf_FROM_SZ_LITERAL(fingerprint->name),
				.Target_config_params_present = true,
				.Target_config_params = {
					.Params_m_count = 4,
					.Params_NameValuePair_m = {{
						.NameValuePair_name = UsefulBuf_FROM_SZ_LITERAL("test_pin"),
						.NameValuePair_value = {
							.AnyType_union_choice = AnyType_int_c,
							.AnyType_int = fingerprint->test_pin
						}
					}, {
						.NameValuePair_name = UsefulBuf_FROM_SZ_LITERAL("test_pin_bank"),
						.NameValuePair_value = {
							.AnyType_union_choice = AnyType_int_c,
							.AnyType_int = (unsigned long)fingerprint->test_pin_bank
						}
					}, {
						.NameValuePair_name = UsefulBuf_FROM_SZ_LITERAL("op_pin"),
						.NameValuePair_value = {
							.AnyType_union_choice = AnyType_int_c,
							.AnyType_int = fingerprint->op_pin
						}
					}, {
						.NameValuePair_name = UsefulBuf_FROM_SZ_LITERAL("op_pin_bank"),
						.NameValuePair_value = {
							.AnyType_union_choice = AnyType_int_c,
							.AnyType_int = (unsigned long)fingerprint->op_pin_bank
						}
					}}
				}
			},
			.MeasurementSeries_env_params_present = false,
			.MeasurementSeries_env_params = {//humidity, temperature, ...
				.Params_m_count = 0,
			},
			.MeasurementSeries_start_time_present = false,
			.MeasurementSeries_unit = {
				.Unit_choice = Unit_UnitElectricalSi_m_c,
				.Unit_UnitElectricalSi_m = UNIT_ELECTRICAL_SI_NONE_c
			},
			.MeasurementSeries_unit_multiple = UNIT_MULTIPLE_SI_BASE_c,
			.MeasurementSeries_union_choice = MeasurementSeries_union_RegularMeasurementSeries_c,
		};
		struct RegularMeasurementSeries *tmpRegMS = &(tmpMS.MeasurementSeries_union_RegularMeasurements);
		tmpRegMS->RegularMeasurementSeries_values_NumericalValue_m_count = fingerprint->sample_size;
		for (size_t j=0; j<fingerprint->sample_size; j++) {
			struct NumericalValue_value_r tmpNv = INIT_NUMERICAL_VALUE_INT(fingerprint->samples[j + (i * fingerprint->sample_size)]);
			tmpRegMS->RegularMeasurementSeries_values_NumericalValue_m[j] = tmpNv;
		}
		tmpRegMS->RegularMeasurementSeries_interval_frequency_duration_m.interval_frequency_duration_choice = interval_frequency_duration_duration_c;
		tmpRegMS->RegularMeasurementSeries_interval_frequency_duration_m.interval_frequency_duration_duration.Time_seconds_choice = Time_seconds_uint_c;
		tmpRegMS->RegularMeasurementSeries_interval_frequency_duration_m.interval_frequency_duration_duration.Time_seconds_uint = fingerprint->delta_t[i];
		tmpRegMS->RegularMeasurementSeries_interval_frequency_duration_m.interval_frequency_duration_duration.Time_unit_mult = UNIT_MULTIPLE_SI_MILLI_c;
		tmp.AnalogMeasurement_measurements_MeasurementSeries_m[i] = tmpMS;
	}

	UsefulBuf_MAKE_STACK_UB(  EngineBuffer, 350);//determine size using CALCULATE_BUF_SIZE
	QCBORError err = encodeAnalogMeasurement(EngineBuffer, &tmp, buffer, fingerprint->uart);
	HAL_UART_Transmit(fingerprint->uart, (uint8_t *) buffer->ptr, buffer->len, 100);
	return err;
}
