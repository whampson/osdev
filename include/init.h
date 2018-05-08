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
 * File: init.h
 * Desc: Early kernel initialization macros. Kept in a separate file so they can
 *       be used with linker scripts.
 *----------------------------------------------------------------------------*/

#ifndef __INIT_H__
#define __INIT_H__

#define KERNEL_START        0x400000    /* 4 MiB */
#define KERNEL_STACK_BASE   0x800000    /* 8 MiB */
#define ENTRY_POINT         kernel_init

#define NUM_VEC 256

#endif /* __INIT_H__ */
