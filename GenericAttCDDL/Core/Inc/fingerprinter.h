/* SPDX-License-Identifier: BSD-3-Clause */
/*****************************************************************************
* Copyright 2024, Fraunhofer Institute for Secure Information Technology SIT.
* All rights reserved.
****************************************************************************/

/**
* @file fingerprinter.h
* @author Lukas Jäger (lukas.jaeger@sit.fraunhofer.de)
* @author Anselm Angert (anselm.angert@sit.fraunhofer.de)
* @brief
* @version 1.1
* @date 2020-04-24
*
* @copyright Copyright 2024, Fraunhofer Institute for Secure Information
* Technology SIT. All rights reserved.
*
* @license BSD 3-Clause "New" or "Revised" License (SPDX-License-Identifier:
* BSD-3-Clause).
*/

#ifndef FINGERPRINTER_H_
#define FINGERPRINTER_H_

typedef struct {
	const char * name;
	unsigned int test_pin;
	void * test_pin_bank;
	unsigned int op_pin;
	void * op_pin_bank;
	void * timer;
	void * adc;
	void * uart;
	unsigned int sample_size;
	unsigned int num_of_samples;
	unsigned int * samples;
	unsigned long * delta_t;
} Fingerprinter;

void print_string(void * uart, char const * string);

void init_fingerprinter(Fingerprinter * fingerprint, const char * name, void * test_pin_bank,
		unsigned int test_pin, void * op_pin_bank, unsigned int op_pin, void * uart,
		void * timer, void * adc, unsigned int sample_size, unsigned int num_of_samples);

void get_and_print_fingerprint(Fingerprinter * fingerprint, int op_pin_mode);

void get_fingerprint(Fingerprinter * fingerprint, int op_pin_mode);

#endif /* FINGERPRINTER_H_ */
