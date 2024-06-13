/* SPDX-License-Identifier: BSD-3-Clause */
/*****************************************************************************
* Copyright 2024, Fraunhofer Institute for Secure Information Technology SIT.
* All rights reserved.
****************************************************************************/

/**
* @file fingerprinter.c
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

#include <fingerprinter.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_system.h"


#define SAMPLE_DIVIDER (4096)

#define V_REF (3.3)

typedef enum {
	IN,
	OUT,
} GPIOMode;

void print_string(void * uart, char const * string) {
	if (uart != NULL && string != NULL) {
		HAL_UART_Transmit(uart, (uint8_t *) string,
					strlen(string), 100);

	}
}

static void set_gpio_mode (GPIO_TypeDef * pin_bank,
		unsigned int pin, GPIOMode mode) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	switch (mode) {
	case IN:
		GPIO_InitStruct.Pin = pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		break;
	case OUT:
		GPIO_InitStruct.Pin = pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		break;
	}
	HAL_GPIO_Init(pin_bank, &GPIO_InitStruct);
}

static void print_samples(Fingerprinter * fingerprint, size_t sample_number) {
	print_string(fingerprint->uart, "--- Sensor: ");
	print_string(fingerprint->uart, fingerprint->name);
	print_string(fingerprint->uart, "\r\n");

	print_string(fingerprint->uart, "Delta T: ");
	char delta_t_buff [20];
	snprintf(delta_t_buff, 20, "%lu", fingerprint->delta_t[sample_number]);
	print_string(fingerprint->uart, delta_t_buff);
	print_string(fingerprint->uart, "\r\n");

	for (size_t i = 0; i < fingerprint->sample_size; i++){
		double val = (double)((double)fingerprint->samples[i + (sample_number * fingerprint->sample_size)] /
				(double)(SAMPLE_DIVIDER) * V_REF);
		snprintf(delta_t_buff, 20, "%1.2f", val);
		print_string(fingerprint->uart, delta_t_buff);
		print_string(fingerprint->uart, "\r\n");
	}
}

static void measure(Fingerprinter * fingerprint, size_t sample_number) {
	// Measure the start time
	TIM_HandleTypeDef * timer = (TIM_HandleTypeDef *)
			fingerprint->timer;

	HAL_TIM_Base_Stop(timer);
	__HAL_TIM_SET_COUNTER(timer, 0);
	HAL_TIM_Base_Start(timer);

	unsigned long start_micros =
			__HAL_TIM_GET_COUNTER(timer);

	// Do the measurement
	for (size_t i = 0; i < fingerprint->sample_size; i++){
		HAL_ADC_Start(fingerprint->adc);
		while (HAL_ADC_PollForConversion(fingerprint->adc,
				1000000) != HAL_OK);
		HAL_ADC_Stop(fingerprint->adc);
		fingerprint->samples[i + (sample_number * fingerprint->sample_size)] = HAL_ADC_GetValue(fingerprint->adc);
	}

	// Measure the end time and compute the difference
	unsigned long end_micros =  __HAL_TIM_GET_COUNTER(timer);
	fingerprint->delta_t[sample_number] = end_micros - start_micros;
}

static void setup(Fingerprinter * fingerprint) {
	char * empty_row = "\r\n";
	char * begin_message = "--- Begin analogue fingerprinting\r\n";
	HAL_UART_Transmit(fingerprint->uart, (uint8_t *) empty_row,
			strlen(empty_row), 100);
	HAL_UART_Transmit(fingerprint->uart, (uint8_t *) begin_message,
			strlen(begin_message), 100);
}

static void teardown(Fingerprinter * fingerprint, int op_pin_mode) {
	char * end_message = "--- End analogue fingerprinting\r\n";
	HAL_UART_Transmit(fingerprint->uart, (uint8_t *) end_message,
			strlen(end_message), 1000);

	// Disable Test Pin domain and enable Operation pin domain
	set_gpio_mode(fingerprint->test_pin_bank,
				fingerprint->test_pin, IN);
	set_gpio_mode(fingerprint->op_pin_bank,
			fingerprint->op_pin, op_pin_mode);
}

void init_fingerprinter(Fingerprinter * fingerprint, const char * name, void * test_pin_bank,
		unsigned int test_pin, void * op_pin_bank, unsigned int op_pin, void * uart,
		void * timer, void * adc, unsigned int sample_size, unsigned int num_of_samples) {
	if (fingerprint != NULL) {
		fingerprint->name = name;
		fingerprint->test_pin_bank = test_pin_bank;
		fingerprint->test_pin = test_pin;
		fingerprint->op_pin_bank = op_pin_bank;
		fingerprint->op_pin = op_pin;
		fingerprint->timer = timer;
		fingerprint->uart = uart;
		fingerprint->adc = adc;
		fingerprint->timer = timer;
		fingerprint->sample_size = sample_size;
		fingerprint->num_of_samples = num_of_samples;
		fingerprint->samples = (unsigned int*) malloc(num_of_samples * sample_size * sizeof(unsigned int));
		fingerprint->delta_t = (unsigned long*) malloc(num_of_samples * sizeof(unsigned long));
	}
}

void get_and_print_fingerprint(Fingerprinter * fingerprint, int op_pin_mode) {
	if (fingerprint != NULL) {
		GPIO_TypeDef * op_pin_bank = (GPIO_TypeDef*) fingerprint->op_pin_bank;
		GPIO_TypeDef * test_pin_bank = (GPIO_TypeDef*) fingerprint->test_pin_bank;
		setup(fingerprint);
		for (size_t sample = 0; sample < fingerprint->num_of_samples; sample++) {
			// Draw the line low
			set_gpio_mode(op_pin_bank, fingerprint->op_pin, OUT);
			set_gpio_mode(test_pin_bank, fingerprint->test_pin, OUT);
			HAL_GPIO_WritePin(op_pin_bank, fingerprint->op_pin,
					GPIO_PIN_RESET);
			HAL_GPIO_WritePin(test_pin_bank, fingerprint->test_pin,
								GPIO_PIN_RESET);
			HAL_Delay(100);

			set_gpio_mode(op_pin_bank, fingerprint->op_pin, IN);
			HAL_Delay(100);
			HAL_GPIO_WritePin(test_pin_bank, fingerprint->test_pin,
											GPIO_PIN_SET);

			measure(fingerprint, sample);

			print_samples(fingerprint, sample);
		}
		teardown(fingerprint, op_pin_mode);
	}
}

void get_fingerprint(Fingerprinter * fingerprint, int op_pin_mode){
	if (fingerprint != NULL) {
		GPIO_TypeDef * op_pin_bank = (GPIO_TypeDef*) fingerprint->op_pin_bank;
		GPIO_TypeDef * test_pin_bank = (GPIO_TypeDef*) fingerprint->test_pin_bank;
		for (size_t sample = 0; sample < fingerprint->num_of_samples; sample++) {
			// Draw the line low
			set_gpio_mode(op_pin_bank, fingerprint->op_pin, OUT);
			set_gpio_mode(test_pin_bank, fingerprint->test_pin, OUT);
			HAL_GPIO_WritePin(op_pin_bank, fingerprint->op_pin,
					GPIO_PIN_RESET);
			HAL_GPIO_WritePin(test_pin_bank, fingerprint->test_pin,
								GPIO_PIN_RESET);
			HAL_Delay(100);

			set_gpio_mode(op_pin_bank, fingerprint->op_pin, IN);
			HAL_Delay(100);
			HAL_GPIO_WritePin(test_pin_bank, fingerprint->test_pin,
											GPIO_PIN_SET);

			measure(fingerprint, sample);
		}
		// Disable Test Pin domain and enable Operation pin domain
		set_gpio_mode(fingerprint->test_pin_bank, fingerprint->test_pin, IN);
		set_gpio_mode(fingerprint->op_pin_bank, fingerprint->op_pin, op_pin_mode);
	}
}
