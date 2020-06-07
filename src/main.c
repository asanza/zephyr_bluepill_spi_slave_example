/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <zephyr.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 150

#ifndef DT_ALIAS_LED0_GPIOS_FLAGS
#define FLAGS 0
#else
#define FLAGS DT_ALIAS_LED0_GPIOS_FLAGS
#endif

/* Make sure the board's devicetree declares led0 in its /aliases. */
#ifdef DT_ALIAS_LED0_GPIOS_CONTROLLER
#define LED0 DT_ALIAS_LED0_GPIOS_CONTROLLER
#define PIN DT_ALIAS_LED0_GPIOS_PIN
#else
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0 ""
#define PIN 0
#endif

void main(void) {
  struct device *dev, *spi;
  bool led_is_on = true;
  int ret;

  dev = device_get_binding(LED0);
  if (dev == NULL) {
    return;
  }

  ret = gpio_pin_configure(dev, PIN, GPIO_OUTPUT_ACTIVE | FLAGS);
  if (ret < 0) {
    return;
  }

  spi = device_get_binding("SPI_1");

  // struct spi_cs_control cs_ctrl = {
  // 	.gpio_dev = DT_ALIAS_CS1_GPIOS_CONTROLLER,
  // 	.gpio_pin = DT_ALIAS_CS1_GPIOS_PIN,
  // 	.delay = 1,
  // };

  struct spi_config config = {
      .frequency = 4000000,
      .operation = SPI_OP_MODE_SLAVE | SPI_MODE_CPHA 
	  				| SPI_TRANSFER_MSB | SPI_WORD_SET(8) | SPI_LINES_SINGLE,
      .slave = 0,
      .cs = NULL,
  };

  char array[100];

  struct spi_buf buf[] = {
      {
          .buf = array,
          .len = sizeof(array),
      },
  };

  struct spi_buf_set rxbuf = {
      .buffers = buf,
      .count = 1,
  };


  while (1) {
    gpio_pin_set(dev, PIN, (int)led_is_on);
	spi_read(spi, &config, &rxbuf);
    led_is_on = !led_is_on;
    k_sleep(SLEEP_TIME_MS);
  }
}
