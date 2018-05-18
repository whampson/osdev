/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
 * Copyright (C) 2018 Wes Hampson. All Rights Reserved.                       *
 *                                                                            *
 * This file is part of the Lyra operating system.                            *
 *                                                                            *
 * Lyra is free software: you can redistribute it and/or modify               *
 * it under the terms of version 2 of the GNU General Public License          *
 * as published by the Free Software Foundation.                              *
 *                                                                            *
 * See LICENSE in the top-level directory for a copy of the license.          *
 * You may also visit <https://www.gnu.org/licenses/gpl-2.0.txt>.             *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*-----------------------------------------------------------------------------
 * File: kbd.c
 * Desc: PS/2 keyboard driver.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <lyra/kbd.h>
#include <lyra/io.h>

/* PS/2 keyboard and controller I/O ports. */
#define PORT_KBD            0x60    /* PS/2 keyboard I/O */
#define PORT_CTL            0x64    /* PS/2 controller I/O */

/* PS/2 controller status register bitfields.
   The status register is accessed by reading from PORT_CTL. */
#define CTL_STS_OUTFULL     0x01    /* keyboard output buffer full */
#define CTL_STS_INFULL      0x02    /* keyboard input buffer full */
#define CTL_STS_TIMEOUTERR  0x40    /* timeout error */
#define CTL_STS_PARITYERR   0x80    /* parity error */

#define CTL_CFG_INTR1       0x01
#define CTL_CFG_INTR2       0x02
#define CTL_CFG_SYS         0x04
#define CTL_CFG_CLK         0x10
#define CTL_CFG_XLATE1      0x40
#define CTL_CFG_XLATE2      0x20

/* Controller command words.
   The command register is accessed by writing to PORT_CTL. */
#define CTL_CMD_RDCFG       0x20    /* read controller configuration port */
#define CTL_CMD_WRCFG       0x60    /* write controller configuration port */
#define CTL_CMD_SELFTEST    0xAA
#define CTL_CMD_TESTPORT1   0xAB
#define CTL_CMD_TESTPORT2   0xA9
#define CTL_CMD_KBDOFF      0xAD    /* disable keyboard */
#define CTL_CMD_KBDON       0xAE    /* enable keyboard */

/* Keyboard command words. */
#define KBD_CMD_SETLED      0xED    /* set LEDs */
#define KBD_CMD_ECHO        0xEE    /* echo (sends same byte back on success) */
#define KBD_CMD_SCANCODE    0xF0    /* get/set scancode set */
#define KBD_CMD_TYPEMATIC   0xF3    /* set typematic rate and delay */
#define KBD_CMD_SELFTEST    0xFF

/* Keyboard response words. */
#define KBD_RES_ECHO        0xEE    /* echo byte (see above) */
#define KBD_RES_ACK         0xFA    /* command acknowledged */
#define KBD_RES_RSND        0xFE    /* resend last command */

static void kbd_enable(void);
static void kbd_disable(void);
static void kbd_flush(void);
static int ctl_selftest(void);
static int kbd_selftest(void);
static int disable_translation(void);
static int kbd_reset(void);
static int setup_sc3(void);

static void ctl_outb(uint8_t data);
static void kbd_outb(uint8_t data);
static uint8_t kbd_inb(void);


void kbd_init(void)
{
    uint8_t data;

    kbd_disable();
    // ctl_selftest();
    // kbd_flush();
    // disable_translation();
    kbd_selftest();


    // /* Set scancode 3 */
    // while (inb(PORT_CTL) & 0x02);
    // outb(CMD_KBD_SC, PORT_KBD);
    // while (!(inb(PORT_CTL) & 0x01));
    // data = inb(PORT_KBD);
    // if (data != KBD_ACK) {
    //     puts("Error: ACK not received!\n");
    // }

    // while (inb(PORT_CTL) & 0x02);
    // outb(3, PORT_KBD);
    // while (!(inb(PORT_CTL) & 0x01));
    // data = inb(PORT_KBD);
    // if (data != KBD_ACK) {
    //     puts("Error: ACK not received!\n");
    // }

    // /* Set all keys to make/break */
    // while (inb(PORT_CTL) & 0x02);
    // outb(0xFA, PORT_KBD);
    // while (!(inb(PORT_CTL) & 0x01));
    // data = inb(PORT_KBD);
    // if (data != KBD_ACK) {
    //     puts("Error: ACK not received!\n");
    // }


    // /* Read scancode number */
    // while (inb(PORT_CTL) & 0x02);
    // outb(CMD_KBD_SC, PORT_KBD);
    // while (!(inb(PORT_CTL) & 0x01));
    // data = inb(PORT_KBD);
    // if (data != KBD_ACK) {
    //     puts("Error: ACK not received!\n");
    // }

    // while (inb(PORT_CTL) & 0x02);
    // outb(0, PORT_KBD);
    // while (!(inb(PORT_CTL) & 0x01));
    // data = inb(PORT_KBD);
    // if (data != KBD_ACK) {
    //     puts("Error: ACK not received!\n");
    // }

    // while (!(inb(PORT_CTL) & 0x01));
    // data = inb(PORT_KBD);

    // switch (data) {
    //     case 0x01:
    //         puts("Scancode 1!\n");
    //         break;
    //     case 0x02:
    //         puts("Scancode 2!\n");
    //         break;
    //     case 0x03:
    //         puts("Scancode 3!\n");
    //         break;
    //     default:
    //         puts("Unknown scancode?\n");
    //         break;
    // }

    kbd_enable();
}

void kbd_handle_interrupt(void)
{
    uint8_t data;
    data = inb(PORT_KBD);

    putchar((char) data);
}

static int ctl_selftest(void)
{
    ctl_outb(CTL_CMD_SELFTEST);
    if (kbd_inb() != 0x55) {
        puts("i8042: self-test failed!\n");
        return 1;
    }

    // ctl_outb(CTL_CMD_TESTPORT1);
    // if (kbd_inb() != 0x00) {
    //     puts("i8042: port 1 failure!\n");
    //     return 1;
    // }

    return 0;
}

static int disable_translation(void)
{
    uint8_t data;

    ctl_outb(CTL_CMD_RDCFG);
    data = kbd_inb();
    data &= ~0x40;
    ctl_outb(CTL_CMD_WRCFG);
    kbd_outb(data);

    return 0;
}

static int kbd_selftest(void)
{
    kbd_outb(KBD_CMD_SELFTEST);
    if (kbd_inb() != 0xAA) {
        puts("Keyboard self-test failed!\n");
        return 1;
    }

    return 0;
}

static void kbd_flush(void)
{
    while (inb(PORT_CTL) & CTL_STS_OUTFULL) {
        (void)kbd_inb();
    }
}

static void kbd_enable(void)
{
    ctl_outb(CTL_CMD_KBDON);
}

static void kbd_disable(void)
{
    ctl_outb(CTL_CMD_KBDOFF);
}

/* Output a byte to the PS/2 controller. */
static void ctl_outb(uint8_t data)
{
    /* Poll status register, wait until input buffer is empty before writing */
    while (inb(PORT_CTL) & CTL_STS_INFULL);
    outb(data, PORT_CTL);
}

/* Output a byte to the PS/2 keyboard. */
static void kbd_outb(uint8_t data)
{
    /* Poll status register, wait until input buffer is empty before writing */
    while (inb(PORT_CTL) & CTL_STS_INFULL);
    outb(data, PORT_KBD);
}

/* Read a byte from the PS/2 keyboard. */
static uint8_t kbd_inb(void)
{
    /* Poll status register, wait until output buffer is full before reading */
    while (!(inb(PORT_CTL) & CTL_STS_OUTFULL));
    return inb(PORT_KBD);
}
