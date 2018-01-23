/*
 * OLIMEX IRDA+ UEXT module driver
 *
 * by Bjoern Eschrich <bjoern.eschrich@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/completion.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/unistd.h>
#include <linux/kthread.h>
#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>

#if defined(CONFIG_LIRC) || defined(CONFIG_LIRC_MODULE)
#include <media/lirc.h>
#include <media/lirc_dev.h>
#endif

#if defined(CONFIG_RC_CORE) || defined(CONFIG_RC_CORE_MODULE)
#include <media/rc-core.h>
#endif

#if defined(CONFIG_INPUT_CORE) || defined(CONFIG_INPUT_MODULE)
#include <linux/input.h>
#endif

/* Our definitions */
#include "olimex-irda.h"

/* Since the reference to private data is stored within the I2C
   bus driver, we will store another reference within this driver
   so the sysfs related function may also access this data */
struct irda_i2c_priv *g_driver_priv = 0L;

/*************************************************
 * Description:
 */
static int irda_mod_i2c_cmd(struct irda_i2c_priv *priv)
{
	int rc;

    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);

    /* sane check */
    BUG_ON(!priv);
    BUG_ON(!priv->i2c_client);
    BUG_ON(!priv->buffer);

    if (priv->cmd_length == 0 || priv->cmd_id == 0)
    {
    	printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> Invalid command => 0x%x\n", __func__, priv->cmd_id);
    	return -EINVAL;
    }

    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> command=0x%x length=%d\n", __func__, priv->cmd_id, priv->cmd_length);

    /* set command to send */
    priv->buffer[0] = priv->cmd_id;
    priv->cmd_pending = 1;
    if ((rc = i2c_master_send(priv->i2c_client, priv->buffer, priv->cmd_length)) != priv->cmd_length) {
    	printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> i2c_master_send() failed! rc=%d\n", __func__, rc);
    	return rc;
    }

	/* delegate I2C transactions since hardware
	 * interrupts need to be handled very fast */
    schedule_work(&priv->work);

    return 0;
}

/*************************************************
 * Description:
 *     When the controller interrupt is asserted, this function is scheduled
 *     to be called to read the controller data within the function.
 */
static void irda_mod_i2c_read(struct work_struct *work)
{
    struct irda_i2c_priv *priv = container_of(work, struct irda_i2c_priv, work);
	int rc;

    BUG_ON(!priv);
    BUG_ON(!priv->i2c_client);
    BUG_ON(!priv->buffer);

    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> requested command=0x%x\n", __func__, priv->cmd_id);

    /* reset ... */
    priv->cmd_pending = 0;

    if ((rc = i2c_master_recv(priv->i2c_client, priv->buffer, CMD_BUFFER_SIZE)) <= 0) {
    	printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> i2c_master_recv() failed! rc=%d\n", __func__, rc);
    	return;
    }

    switch(priv->cmd_id)
    {
    	case IRDA_SET_MODE:
    		break;
    	case IRDA_WRITE:
    		break;
    	case IRDA_READ:
    		break;
    	case IRDA_GET_ID:
    		priv->irda_id = priv->buffer[0];
        	printk(KERN_DEBUG IRDA_DEVICE_NAME ": got device id => 0x%x\n", priv->irda_id);
        	break;
    	case IRDA_SET_ADDRESS:
    		break;
    	default:
        	printk(KERN_ERR IRDA_DEVICE_NAME ": Invalid command => 0x%x\n", priv->cmd_id);
        	break;
    }
}

/*************************************************
 * Description: SysFS
 *     Display value of "cmd_pending" variable to application that is
 *     requesting it's value.
 */
static ssize_t cmd_pending_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
	BUG_ON(!g_driver_priv);

	return sprintf(buf, "%d", g_driver_priv->cmd_pending);
}

/*************************************************
 * Description: SysFS
 *     Save value to "cmd_pending" variable from application that is
 *     requesting this.
 */
static ssize_t cmd_pending_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
	BUG_ON(!g_driver_priv);

	sscanf(buf, "%d", &g_driver_priv->cmd_pending);
    return count;
}

/*************************************************
 * Description: SysFS
 *     Display value of "cmd_mode" variable to application that is
 *     requesting it's value.
 */
static ssize_t cmd_mode_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
	BUG_ON(!g_driver_priv);

    return sprintf(buf, "%d", 0);
}

/*************************************************
 * Description: SysFS
 *     Save value to "cmd_mode" variable from application that is
 *     requesting this.
 */
static ssize_t cmd_mode_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
	BUG_ON(!g_driver_priv);

    sscanf(buf, "%d", &g_driver_priv->cmd_mode);

    return count;
}

/*************************************************
 * Description: SysFS
 *     Display value of "cmd_buffer" variable to application that is
 *     requesting it's value.
 */
static ssize_t cmd_buffer_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
	BUG_ON(!g_driver_priv);

	return sprintf(buf, "%s", g_driver_priv->buffer);
}

/*************************************************
 * Description: SysFS
 *    Save value to "cmd_buffer" variable from application that is
 *    requesting this.
 */
static ssize_t cmd_buffer_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	//int retval;

	printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
	BUG_ON(!g_driver_priv);

	/*TODO: WIP

    if ((retval = irda_mod_i2c_cmd(g_driver_priv)))
    {
        printk(KERN_ERR IRDA_DEVICE_NAME ": irda_mod_i2c_cmd() failed! cmd=0x%x rc=%d\n",
        	g_driver_priv->cmd_id, retval);
        return retval;
    }
	*/

    /*TODO: WIP -> make hex byte string */
    return (size_t) snprintf(g_driver_priv->buffer, CMD_BUFFER_SIZE, "%s", buf);
}

/*************************************************
 * Description:
 *     After the interrupt is asserted for the controller, this
 *     is the first function that is called. Since this is a time sensitive
 *     function, we need to immediately schedule work so the integrity of
 *     properly system operation. This function needs to be called to
 *     finish registering the driver.
 */
static irqreturn_t irda_mod_irq_handler(int irq, void *dev_id)
{
    struct irda_i2c_priv *priv = (struct irda_i2c_priv *)dev_id;
    int i;

    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);

    if (!priv)
    {
        printk(KERN_ERR IRDA_DEVICE_NAME ": IRQ handler has no private data.\n");
        return -EINVAL;
    }

    for (i = 0; i < CMD_BUFFER_SIZE; i++)
    {
    	priv->buffer[i] = 0;
    }

	/* delegate I2C transactions since hardware
	 * interrupts need to be handled very fast */
    schedule_work(&priv->work);

    return IRQ_HANDLED;
}

#if defined(CONFIG_RC_CORE) || defined(CONFIG_RC_CORE_MODULE)
/*************************************************
 * Description: Related to media/rc device i/o
 * funcitons. This function is called on every attempt
 * to close the current device and used for both debugging
 * purposes fullfilling an I2C driver function callback
 * requirement.
 */
static int irda_rc_open(struct rc_dev *dev)
{
    struct irda_i2c_priv *priv = dev->priv;

    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
    BUG_ON(!priv);

    return 0;
}

/*************************************************
 * Description: Related to media/rc device i/o
 * funcitons. This function is called on every attempt
 * to close the current device and used for both debugging
 * purposes fullfilling an I2C driver function callback
 * requirement.
 */
static void irda_rc_close(struct rc_dev *dev)
{
    struct irda_i2c_priv *priv = dev->priv;

    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
    BUG_ON(!priv);
}
#endif

#ifdef CONFIG_INPUT
/*************************************************
 * Description: Releted to input device I/O
 * functions. This function is called on every attempt
 * to close the current device and used for both debugging
 * purposes fullfilling an I2C driver function callback
 * requirement.
 */
static int irda_input_open(struct input_dev *dev)
{
    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
    return 0;
}

/*************************************************
 * Description: Releted to input device I/O
 * functions. This function is called on every attempt
 * to close the current device and used for both debugging
 * purposes fullfilling an I2C driver function callback
 * requirement.
 */
static void irda_input_close(struct input_dev *dev)
{
    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);
}
#endif

/*************************************************
 * SysFS:
 * Attribute definitions
 */

/* SysFS Attributes */
static struct kobj_attribute cmd_pending_attribute = {
	.attr = {
		.name = "cmd_pending",
		.mode = 0666,
	},
	.show	= cmd_pending_show,
	.store	= cmd_pending_store,
};

static struct kobj_attribute cmd_mode_attribute = {
	.attr = {
		.name = "cmd_mode",
		.mode = 0666,
	},
	.show	= cmd_mode_show,
	.store	= cmd_mode_store,
};

static struct kobj_attribute cmd_buffer_attribute = {
	.attr = {
		.name = "cmd_buffer",
		.mode = 0666,
	},
	.show	= cmd_buffer_show,
	.store	= cmd_buffer_store,
};

/*************************************************
 * SysFS:
 * Create a group of attributes so we may work with
 * them as a set.
 */
static struct attribute *attrs[] = {
    &cmd_pending_attribute.attr,
    &cmd_mode_attribute.attr,
    &cmd_buffer_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

/*************************************************
 * Description:
 *     Cleanup resources
 */
static void irda_driver_cleanup(struct irda_i2c_priv *priv)
{
	printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);

	/* Remove irq handler */
    if (priv->irq && priv->irq != INVALID_GPIO)
    {
    	printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> release IRQ %d\n", __func__, priv->irq);
    	free_irq(priv->irq, priv);
		priv->irq = INVALID_GPIO;
    }

    /* remove device */
#if defined(CONFIG_RC_CORE) || defined(CONFIG_RC_CORE_MODULE)
    if (priv->rc)
    {
    	printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> release RC part\n", __func__);
        priv->rc->priv = NULL;
        rc_unregister_device(priv->rc);
        priv->rc = NULL;
    }
#endif

#if defined(CONFIG_INPUT_CORE) || defined(CONFIG_INPUT_MODULE)
	printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> release input part\n", __func__);
    /* TODO: WIP */
#endif

    /* Remove kernel object instance */
	if (priv->irda_kobj)
	{
		printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> release sysfs part\n", __func__);
		sysfs_remove_group(priv->irda_kobj, &attr_group);
		if(priv->irda_kobj->state_initialized)
			kobject_put(priv->irda_kobj);
		priv->irda_kobj = NULL;
	}

	/* Remove device class instance */
	if (priv->irda_class)
	{
		printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> device class part\n", __func__);
		class_destroy(priv->irda_class);
		priv->irda_class = NULL;
	}

    /* release resources */
	g_driver_priv = NULL;
	kfree(priv);
}

/*************************************************
 * UEXT Module IRDA+ board info
 */
static struct i2c_board_info uext_irda_info = {
	I2C_BOARD_INFO(IRDA_DEVICE_NAME, IRDA_DEVICE_ADDR),
};

/* I2C addresses to scan */
static const unsigned short irda_addr_list[] = {
	IRDA_DEVICE_ADDR, I2C_CLIENT_END
};

/*************************************************
 * Description:
 *   Probe I2C bus and find our UEXT IRDA+ device.
 *   by sending command IRDA_GET_ID
 */
static int mod_irda_device_probe(struct i2c_adapter *adapter, unsigned short addr)
{
#if 0
	union i2c_smbus_data data;
	int retval;

	printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> device probe at: adapter #%d addr=0x%x\n", __func__, adapter->nr, addr);

	if (i2c_check_functionality(adapter, I2C_FUNC_SMBUS_QUICK) || (
					i2c_check_functionality(adapter, I2C_FUNC_SMBUS_READ_BYTE) &&
					i2c_check_functionality(adapter, I2C_FUNC_SMBUS_WRITE_BYTE)))
	{
		printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> probe with I2C_FUNC_SMBUS_QUICK\n", __func__);

		data.byte = IRDA_GET_ID;
		retval = i2c_smbus_xfer(adapter, addr, 0, I2C_SMBUS_WRITE, IRDA_GET_ID, I2C_SMBUS_QUICK, &data);
		if (retval)
		{
			printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> i2c_smbus_xfer(I2C_SMBUS_WRITE) failed. rc=%d\n", __func__, retval);
			goto not_found;
		}

		data.byte = 0;
		retval = i2c_smbus_xfer(adapter, addr, 0, I2C_SMBUS_READ, IRDA_GET_ID, I2C_SMBUS_QUICK, &data);
		if (retval)
		{
			printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> i2c_smbus_xfer(I2C_SMBUS_READ) failed. rc=%d\n", __func__, retval);
			goto not_found;
		}
		if (data.byte == IRDA_DEVICE_ID)
		{
			printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> probe result: 0x%02X\n", __func__, data.byte);
		}
		else
		{
			goto not_found;
		}
	}
	else if (i2c_check_functionality(adapter, I2C_FUNC_SMBUS_READ_BYTE))
	{
		printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> probe with i2c_probe_func_quick_read()\n", __func__);
		retval = i2c_probe_func_quick_read(adapter, addr);
		if (retval)
		{
			printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> i2c_probe_func_quick_read() failed. rc=%d\n", __func__, retval);
			goto not_found;
		}
	}
	/* no method will work */
	else {
		goto not_found;
	}

	return 1;

not_found:
#endif
	return 0;
}

#if 0
/*************************************************
 * Description:
 *   Iterate over all known I2C buses and find
 *   our UEXT IRDA+ device.
 */
static int mod_irda_i2c_probe(void)
{
	int retval = 0;
#if 1
    int bus_num, bus_num_found = -1;
	struct i2c_adapter *adapter;
	struct i2c_client *client;
    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> begin bus probe...\n", __func__);

    for(bus_num = 0; bus_num < 5 && bus_num_found == -1; bus_num++)
    {
		adapter = i2c_get_adapter(bus_num);
		if(IS_ERR(adapter) || !adapter)
		{
			printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> i2c_get_adapter() failed. bus_num=%d\n", __func__, bus_num);
			retval = -ENODEV;
			goto error_exit;
		}

		/* Get I2C client interface */
		client = i2c_new_probed_device(adapter, &uext_irda_info, irda_addr_list, NULL /*mod_irda_device_probe*/);
		if (IS_ERR(client) || !client)
		{
			printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> i2c_new_probed_device() failed. bus_num=%d\n", __func__, bus_num);
		}
		else
		{
			bus_num_found = bus_num;
			i2c_release_client(client);
			break;
		}

		i2c_put_adapter(adapter);
    }

    /* uext module found? */
    if (bus_num_found > -1)
    {
		printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> device found at bus_num=%d\n", __func__, bus_num_found);
        retval = 0;
    }
    /* nothing found */
    else {
    	retval = -ENODEV;
    }

error_exit:
	printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s -> end bus probe\n", __func__);
#endif
	return retval;
}
#endif

/*************************************************
 * Description:
 */
static int __devinit irda_mod_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int retval = 0;

    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);

    /* check private driver data */
    BUG_ON(!g_driver_priv);

    /* Is valid I2C client instance?*/
    if (!client) {
        printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> Client pointer is NULL\n", __func__);
        return -ENODEV;
    }

    /* Set I2C client instance */
    g_driver_priv->i2c_client = client;

    /* Set our private driver data to I2C client interface */
    i2c_set_clientdata(client, g_driver_priv);

    /* Set work queue to read data from I2C bus */
    INIT_WORK(&g_driver_priv->work, irda_mod_i2c_read);

    /* prepare device probe */
    g_driver_priv->cmd_id = IRDA_GET_ID;
    g_driver_priv->cmd_length = 1;

    /* Set gpio pin as our interrupt IRQF_TRIGGER_RISING*/
    retval = request_irq(IRDA_IRQ, irda_mod_irq_handler, IRQF_TRIGGER_PROBE, IRDA_DEVICE_NAME "-irq", g_driver_priv);
    if (retval)
    {
        printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> request_irq() failed! irq=%d rc=%d\n", __func__, g_driver_priv->irq, retval);
        g_driver_priv->irq = INVALID_GPIO; /* prevent kernel oops on cleanup */
        goto error;
    }
    g_driver_priv->irq = IRDA_IRQ;

    printk(KERN_INFO IRDA_DEVICE_NAME ": IRQ request ok. p->irq=%d c->irq=%d\n", g_driver_priv->irq, client->irq);

    g_driver_priv->cmd_id = IRDA_GET_ID;
    g_driver_priv->cmd_length = 1;
    retval = irda_mod_i2c_cmd(g_driver_priv);
	if (retval)
	{
        printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> irda_mod_i2c_cmd() failed! rc=%d\n", __func__, retval);
        goto error;
	}

    printk(KERN_INFO IRDA_DEVICE_NAME ": Driver registered.\n");
    return 0;

error:
	irda_driver_cleanup(g_driver_priv);
    return retval;
}

/*************************************************
 * Description:
 *     Unregister/remove the kernel driver from memory.
 */
static int __devexit irda_mod_remove(struct i2c_client *client)
{
    struct irda_i2c_priv *priv = (struct irda_i2c_priv *) i2c_get_clientdata(client);

    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);

    if (priv == NULL && g_driver_priv == NULL)
    {
        printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> No driver data structure allocated.\n", __func__);
        return -EINVAL;
    }

    /* should be normal way */
    if (priv)
    {
        irda_driver_cleanup(priv);
    }
    /* fallback...*/
    else if (g_driver_priv)
    {
        irda_driver_cleanup(g_driver_priv);
    }
    else
    {
        printk(KERN_ERR IRDA_DEVICE_NAME ": %s -> No private driver data!\n", __func__);
    }

    return 0;
}

/* This structure describe a list of
 * supported slave chips */
static const struct i2c_device_id irda_dev_id_table[] = {
    { IRDA_DEVICE_NAME, 0 },
    { }
};

/*************************************************
 * This is the initial set of information information the kernel has
 * before probing drivers on the system
 */
static struct i2c_driver irda_mod_driver = {
    .driver = {
        .name   	= IRDA_DEVICE_NAME,
    },
    .probe      	= irda_mod_probe,
    .remove     	= __devexit_p(irda_mod_remove),
    .suspend    	= NULL,
    .resume     	= NULL,
    .id_table		= irda_dev_id_table,
    .address_list	= irda_addr_list,
};

/*************************************************
 * Module init proc
 */
static int __init irda_mod_init(void)
{
    int retval = -ENODEV;

    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);

    /* Allocate private driver data:
     * Since I2C hold our private data structure internal we
     * hold private driver data structure for SysFS related
     * functions globaly too */
    g_driver_priv = kzalloc(sizeof(struct irda_i2c_priv), GFP_KERNEL);
    if (!g_driver_priv) {
        printk(KERN_ERR IRDA_DEVICE_NAME ": kzalloc error\n");
        return -ENOMEM;
    }

    /* Reset structure */
    memset(g_driver_priv, 0, sizeof(struct irda_i2c_priv));

    /* Set not yet! - will be set by probe */
    g_driver_priv->irq = INVALID_GPIO; /*  */

    /* create device class */
    g_driver_priv->irda_class = class_create(THIS_MODULE, IRDA_DEVICE_NAME);
    if (IS_ERR(g_driver_priv->irda_class)) {
        retval = PTR_ERR(g_driver_priv->irda_class);
        printk(KERN_ERR IRDA_DEVICE_NAME ": Cannot create MOD-IRDA+ driver class. rc=%d\n", retval);
        goto error_exit;
    }

    /* Creates a kobject that appears as a sub-directory under "/sys/kernel". */
    g_driver_priv->irda_kobj = kobject_create_and_add(IRDA_DEVICE_NAME, kernel_kobj);
    if (IS_ERR(g_driver_priv->irda_kobj))
    {
        retval = PTR_ERR(g_driver_priv->irda_kobj);
        printk(KERN_ERR IRDA_DEVICE_NAME ": Cannot create MOD-IRDA+ kobject. rc=%d\n", retval);
        goto error_exit;
    }

    /* Create the files associated with this kobject */
    if ((retval = sysfs_create_group(g_driver_priv->irda_kobj, &attr_group)))
    {
        printk(KERN_ERR IRDA_DEVICE_NAME ": Error registering driver's sysfs interface. rc=%d\n", retval);
        goto error_exit;
    }

	/* Add to i2c system */
	if ((retval = i2c_add_driver(&irda_mod_driver)))
	{
		printk(KERN_ERR IRDA_DEVICE_NAME ": MOD-IRDA+ I2C driver registration failed. rc=%d\n", retval);
		goto error_exit;
	}

#if 0
    /* Probe all known I2C busses to find the device */
	if ((retval = mod_irda_i2c_probe()))
	{
		printk(KERN_ERR IRDA_DEVICE_NAME ": Cannot find MOD-IRDA+ at any I2C bus. rc=%d\n", retval);
		goto error_exit;
	}
#endif

	/* Success */
    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s driver registed\n", __func__);
	return 0;

error_exit:
	irda_driver_cleanup(g_driver_priv);
    return retval;
}

/*************************************************
 * Module exit proc
 */
static void __exit irda_mod_exit(void)
{
    printk(KERN_DEBUG IRDA_DEVICE_NAME ": %s\n", __func__);

    /* remove I2C driver */
    i2c_del_driver(&irda_mod_driver);

    printk(KERN_INFO IRDA_DEVICE_NAME ": module unloaded\n");
}

/*************************************************
 * Kernel module definition
 */
MODULE_DESCRIPTION(IRDA_DEVICE_NAME " driver module");
MODULE_AUTHOR("Bjoern Eschrich");
MODULE_LICENSE("GPL");

module_init(irda_mod_init);
module_exit(irda_mod_exit);
