/* SPDX-License-Identifier: BSD-3-Clause */
/*****************************************************************************
* Copyright 2024, Fraunhofer Institute for Secure Information Technology SIT.
* All rights reserved.
****************************************************************************/

/**
* @file analogMeasurementTypes.h
* @author Anselm Angert (anselm.angert@sit.fraunhofer.de)
* @author partially generated using zcbor version 0.8.1 (https://github.com/NordicSemiconductor/zcbor)
* @brief
* @version 1.0
* @date 2024-05-03
*
* @copyright Copyright 2024, Fraunhofer Institute for Secure Information
* Technology SIT. All rights reserved.
*
* @license BSD 3-Clause "New" or "Revised" License (SPDX-License-Identifier:
* BSD-3-Clause).
*/

#ifndef ANALOGMEASUREMENT_TYPES_H__
#define ANALOGMEASUREMENT_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "UsefulBuf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_MAX_QTY 20

enum UnitMultipleSi_r {
	UNIT_MULTIPLE_SI_YOCTO_c = -24,
	UNIT_MULTIPLE_SI_ZEPTO_c = -21,
	UNIT_MULTIPLE_SI_ATTO_c = -18,
	UNIT_MULTIPLE_SI_FEMTO_c = -15,
	UNIT_MULTIPLE_SI_PICO_c = -12,
	UNIT_MULTIPLE_SI_NANO_c = -9,
	UNIT_MULTIPLE_SI_MICRO_c = -6,
	UNIT_MULTIPLE_SI_MILLI_c = -3,
	UNIT_MULTIPLE_SI_CENTI_c = -2,
	UNIT_MULTIPLE_SI_DECI_c = -1,
	UNIT_MULTIPLE_SI_BASE_c = 0,
	UNIT_MULTIPLE_SI_DECA_c = 1,
	UNIT_MULTIPLE_SI_HECTO_c = 2,
	UNIT_MULTIPLE_SI_KILO_c = 3,
	UNIT_MULTIPLE_SI_MEGA_c = 6,
	UNIT_MULTIPLE_SI_GIGA_c = 9,
	UNIT_MULTIPLE_SI_TERA_c = 12,
	UNIT_MULTIPLE_SI_PETA_c = 15,
	UNIT_MULTIPLE_SI_EXA_c = 18,
	UNIT_MULTIPLE_SI_ZETTA_c = 21,
	UNIT_MULTIPLE_SI_YOTTA_c = 24,
};

//UnitMultiple is missing, but (currently) effectively identical to UnitMultipleSi
#define UnitMultiple UnitMultipleSi

enum UnitElectricalSi_r {
	UNIT_ELECTRICAL_SI_NONE_c = 1,
	UNIT_ELECTRICAL_SI_VOLTAGE_c = 2,
	UNIT_ELECTRICAL_SI_CURRENT_c = 3,
	UNIT_ELECTRICAL_SI_RESISTANCE_c = 4,
	UNIT_ELECTRICAL_SI_CONDUCTANCE_c = 5,
	UNIT_ELECTRICAL_SI_CAPACITANCE_c = 6,
	UNIT_ELECTRICAL_SI_CHARGE_c = 7,
	UNIT_ELECTRICAL_SI_INDUCTANCE_c = 8,
	UNIT_ELECTRICAL_SI_POWER_c = 9,
	UNIT_ELECTRICAL_SI_IMPEDANCE_c = 10,
	UNIT_ELECTRICAL_SI_FREQUENCY_c = 11,
};

struct Unit_r {
	union {
		enum UnitElectricalSi_r Unit_UnitElectricalSi_m;
	};
	enum {
		Unit_UNIT_UNDEFINED_c = 0,//value manually added
		Unit_UnitElectricalSi_m_c,
	} Unit_choice;
};


struct NumericalValue_value_r {
	union {
		int64_t NumericalValue_value_int;
		double NumericalValue_value_float;
	};
	enum {
		NumericalValue_value_int_c,
		NumericalValue_value_float_c,
	} NumericalValue_value_choice;
};

#define INIT_NUMERICAL_VALUE_INT(VALUE) { \
					.NumericalValue_value_choice = NumericalValue_value_int_c, \
					.NumericalValue_value_int = VALUE \
				}

struct Frequency {
	union {
		uint64_t Frequency_hertz_uint;
		double Frequency_hertz_float;
	};
	enum {
		Frequency_hertz_uint_c,
		Frequency_hertz_float_c,
	} Frequency_hertz_choice;
	enum UnitMultipleSi_r Frequency_unit_multiple;
};

struct Time {
	union {
		uint64_t Time_seconds_uint;
		double Time_seconds_float;
	};
	enum {
		Time_seconds_uint_c,
		Time_seconds_float_c,
	} Time_seconds_choice;
	enum UnitMultipleSi_r Time_unit_mult;
};

//implementation of internal array with any number of values
struct IrregularMeasurementSeries_internal_l {
	struct Time IrregularMeasurementSeries_internal_l_current_time;
	struct NumericalValue_value_r IrregularMeasurementSeries_internal_l_NumericalValue_m;
};

struct IrregularMeasurementSeries {
	struct IrregularMeasurementSeries_internal_l IrregularMeasurementSeries_internal_l[DEFAULT_MAX_QTY];
	size_t IrregularMeasurementSeries_internal_l_count;
};

struct interval_frequency_duration_r {
	union {
		struct Time interval_frequency_duration_interval;
		struct Frequency interval_frequency_duration_frequency;
		struct Time interval_frequency_duration_duration;
	};
	enum {
		interval_frequency_duration_interval_c = 1,
		interval_frequency_duration_frequency_c = 2,
		interval_frequency_duration_duration_c = 3,
	} interval_frequency_duration_choice;
};

#define values_map 0//required for RegularMeasurementSeries

struct RegularMeasurementSeries {
	struct NumericalValue_value_r RegularMeasurementSeries_values_NumericalValue_m[DEFAULT_MAX_QTY];
	size_t RegularMeasurementSeries_values_NumericalValue_m_count;
	struct interval_frequency_duration_r RegularMeasurementSeries_interval_frequency_duration_m;
};

struct AnyType {
	union {
		UsefulBufC AnyType_tstr;
		UsefulBufC AnyType_bstr;
		int64_t AnyType_int;
		uint64_t AnyType_uint;
		double AnyType_float;
		bool AnyType_bool;
	};
	enum {
		AnyType_empty_c = 0,
		AnyType_not_supported_type_c = 1,
		AnyType_tstr_c,
		AnyType_bstr_c,
		AnyType_int_c,
		AnyType_uint_c,
		AnyType_float_c,
		AnyType_bool_c,
	} AnyType_union_choice;
};


struct NameValuePair {
	UsefulBufC NameValuePair_name;
	struct AnyType NameValuePair_value;
};

struct Params {
	struct NameValuePair Params_NameValuePair_m[DEFAULT_MAX_QTY];
	size_t Params_m_count;
};

struct Target {
	UsefulBufC Target_id;
	struct Params Target_config_params;
	bool Target_config_params_present;
};

struct MeasurementSeries {
	struct Target MeasurementSeries_target;
	struct Params MeasurementSeries_env_params;
	bool MeasurementSeries_env_params_present;
	struct Time MeasurementSeries_start_time;
	bool MeasurementSeries_start_time_present;
	struct Unit_r MeasurementSeries_unit;
	enum UnitMultipleSi_r MeasurementSeries_unit_multiple;
	union {
		struct RegularMeasurementSeries MeasurementSeries_union_RegularMeasurements;
		struct IrregularMeasurementSeries MeasurementSeries_union_IrregularMeasurementSeries_m;
	};
	enum {
		MeasurementSeries_union_RegularMeasurementSeries_c,
		MeasurementSeries_union_IrregularMeasurementSeries_c,
	} MeasurementSeries_union_choice;
};

struct AnalogMeasurement {
	uint64_t AnalogMeasurement_version_tag;
	struct Time AnalogMeasurement_start_time;
	struct MeasurementSeries AnalogMeasurement_measurements_MeasurementSeries_m[DEFAULT_MAX_QTY];
	size_t AnalogMeasurement_measurements_MeasurementSeries_m_count;
};

#ifdef __cplusplus
}
#endif

#endif /* ANALOGMEASUREMENT_TYPES_H__ */
