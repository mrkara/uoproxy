/*
 * uoproxy
 * $Id$
 *
 * (c) 2005 Max Kellermann <max@duempel.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <sys/types.h>

#include "packets.h"

const size_t packet_lengths[0x100] = {
    [0x0B] = 7,
    [0xBA] = 6,
    [0xBA] = 6,
    [0x25] = 20,
    [0x24] = 7,
    [0x3B] = 8,
    [0x3B] = 8,
    [0xAF] = 13,
    [0xC8] = 2,
    [0xAA] = 5,
    [0x95] = 9,
    [0xC9] = 6,
    [0xCA] = 6,
    [0x2C] = 2,
    [0xC6] = 1,
    [0x4F] = 2,
    [0x4E] = 6,
    [0x4E] = 6,
    [0x2E] = 15,
    [0x27] = 2,
    [0xD1] = 2,
    [0x65] = 4,
    [0x97] = 2,
    [0xCB] = 7,
    [0xC7] = 49,
    [0xC7] = 49,
    [0xC0] = 36,
    [0xC0] = 36,
    [0x85] = 2,
    [0xC0] = 36,
    [0xC0] = 36,
    [0xC0] = 36,
    [0x24] = 7,
    [0x24] = 7,
    [0x25] = 20,
    [0x72] = 5,
    [0x2F] = 10,
    [0x1D] = 5,
    [0x1D] = 5,
    [0x76] = 16,
    [0x7B] = 2,
    [0x77] = 17,
    [0x99] = 26,
    [0x6C] = 19,
    [0x6C] = 19,
    [0x23] = 26,
    [0x88] = 66,
    [0x53] = 2,
    [0x54] = 12,
    [0x6D] = 3,
    [0x5B] = 4,
    [0xBC] = 3,
    [0xB9] = 3,
    [0xA1] = 9,
    [0xA1] = 9,
    [0xA2] = 9,
    [0xA2] = 9,
    [0xA3] = 9,
    [0xA3] = 9,
    [0x2D] = 17,
    [0x2D] = 17,
    [0x38] = 7,
    [0x6E] = 14,
    [0x20] = 19,
    [0x73] = 2,
    [0x21] = 8,
    [0x22] = 3,
    [0x1B] = 37,
    [0x55] = 1,
    [0x82] = 2,
    [0x15] = 9,
    [0x2B] = 2,
    [0x8C] = 11,
    [0x00] = 104,
    [0x01] = 5,
    [0x02] = 7,
    [0x04] = 2,
    [0x05] = 5,
    [0x06] = 5,
    [0x07] = 7,
    [0x08] = 14,
    [0x09] = 5,
    [0x0A] = 11,
    [0x13] = 10,
    [0x14] = 6,
    [0x22] = 3,
    [0x2C] = 2,
    [0x34] = 10,
    [0x47] = 11,
    [0x48] = 73,
    [0x58] = 106,
    [0x5D] = 73,
    [0x61] = 9,
    [0x6C] = 19,
    [0x72] = 5,
    [0x73] = 2,
    [0x75] = 35,
    [0x79] = 9,
    [0x7E] = 2,
    [0x7D] = 13,
    [0x80] = 62,
    [0x83] = 39,
    [0x91] = 65,
    [0x95] = 9,
    [0x9B] = 258,
    [0x9D] = 51,
    [0xA0] = 3,
    [0xA4] = 149,
    [0xA7] = 4,
    [0xB5] = 64,
    [0xB6] = 9,
    [0xBB] = 9,
    [0xC8] = 2,
    [0xC9] = 6,
    [0xCA] = 6,
    [0xcf] = 62,
    [0xD1] = 2,
    [PCK_AOSObjProp] = 9,
};
