/*
 * olimex-irda.h
 *
 *  Created on: 29.09.2017
 *      Author: root
 */

#ifndef DRIVERS_MEDIA_RC_IRDA_OLIMEX_IRDA_H_
#define DRIVERS_MEDIA_RC_IRDA_OLIMEX_IRDA_H_


/* The device name */
#define IRDA_DEVICE_NAME	"olimex_irda"

/* IRDA+ protocol modes */
#define IRDA_MODE_RC5		0x00
#define IRDA_MODE_SIRC		0x01

/* IRDA+ operations */
#define IRDA_SET_MODE		0x01
#define IRDA_WRITE			0x02
#define IRDA_READ			0x03
#define IRDA_GET_ID			0x20
#define IRDA_SET_ADDRESS	0xF0

/* IRDA+ device identification */
#define IRDA_DEVICE_ID		0x54

/* IRDA+ I2C device address */
#define IRDA_DEVICE_ADDR	0x24

/* IRDA+ connected to UEXT1 ->
 * 	GPIO1_D1 (I2C0_SCL) -> RK3188-Pin => G21 -> RK30_PIN1_PD1
 * 	GPIO1_D0 (I2C0_SDA) -> RK3188-Pin => G22 -> RK30_PIN1_PD0
 * 	Interrupt using: GPIO1_A7/SPI0_CSN0 -> RK3188-Pin => AB12
 *    -> RK30_PIN1_PA7 (1*NUM_GROUP + PIN_BASE + 7)
 *    -> 1x32 + 160 + 7 = 199
 * */
#define IRDA_IRQ			199

/* Size of i/o command buffer */
#define CMD_BUFFER_SIZE		10

/* The private data structure that is
 * referenced within the I2C bus driver */
struct irda_i2c_priv {
    struct i2c_client *i2c_client;
    struct kobject *irda_kobj;
    struct class *irda_class;
    struct work_struct work;
#if defined(CONFIG_RC_CORE) || defined(CONFIG_RC_CORE_MODULE)
    struct rc_dev *rc;
#endif
#if defined(CONFIG_INPUT_CORE) || defined(CONFIG_INPUT_MODULE)
    struct input_dev *input;
#endif
    int irda_id;
    int cmd_id;
    int cmd_length;
    int cmd_mode;
    int cmd_pending;
    unsigned char buffer[CMD_BUFFER_SIZE];
    int irq;
};

#endif /* DRIVERS_MEDIA_RC_IRDA_OLIMEX_IRDA_H_ */
