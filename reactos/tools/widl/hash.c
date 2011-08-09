/*
 * Oleaut32 hash functions
 *
 * Copyright 1999 Corel Corporation
 * Copyright 2001-2003 Jon Griffiths
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#include <stdio.h>
#include <stdarg.h>

#include <nls.h>

#include "widltypes.h"
#include "hash.h"

static const unsigned char Lookup_16[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

 /* Windows */
 0x7F, 0x7F, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x7F, 0x89, 0x53, 0x8B, 0x8C,
 0x7F, 0x7F, 0x7F, 0x7F, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x96, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x7F, 0x7F, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x96, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0x55, 0x55, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0x4F, 0xF7, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x55, 0xDE, 0x55,

 /* Mac */
 0x41, 0x41, 0x43, 0x45, 0x4E, 0x4F, 0x55, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0x4F, 0x55, 0x55, 0x55, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0x41, 0x4F, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0x41, 0x4F, 0xBD, 0x41, 0x4F, 0xC0,
 0xC1, 0xC2, 0xC3, 0x46, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0x41, 0x41, 0x4F,
 0xCE, 0xCE, 0xD0, 0xD0, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0x55, 0x55, 0xDA,
 0xDB, 0xDC, 0xDD, 0x3F, 0x3F, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0x41, 0x45, 0x41,
 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4F, 0x4F, 0x3F, 0x4F, 0x55, 0x55, 0x55,
 0x49, 0x7F, 0xF7, 0x7F, 0xF9, 0xFA, 0xFB, 0x3F, 0xFD, 0xFE, 0x7F
};

static const unsigned char Lookup_32[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

 /* Windows */
 0x7F, 0x7F, 0x82, 0x7F, 0x84, 0x85, 0x86, 0x87, 0x7F, 0x89, 0x53, 0x8B, 0x53,
 0x54, 0x5A, 0x5A, 0x7F, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x96, 0x7F, 0x99,
 0x53, 0x9B, 0x53, 0x54, 0x5A, 0x5A, 0xA0, 0x7F, 0xA2, 0x4C, 0xA4, 0x41, 0xA6,
 0xA7, 0xA8, 0xA9, 0x53, 0xAB, 0xAC, 0x96, 0xAE, 0x5A, 0xB0, 0xB1, 0xB2, 0x4C,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x41, 0x53, 0xBB, 0x4C, 0xBD, 0x4C, 0x5A, 0x52,
 0x41, 0x41, 0x41, 0x41, 0x4C, 0x43, 0x43, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49,
 0x49, 0x44, 0xD0, 0x4E, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x52, 0x55, 0x55,
 0x55, 0x55, 0x59, 0x54, 0xDF, 0x52, 0x41, 0x41, 0x41, 0x41, 0x4C, 0x43, 0x43,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x44, 0xD0, 0x4E, 0x4E, 0x4F, 0x4F,
 0x4F, 0x4F, 0xF7, 0x52, 0x55, 0x55, 0x55, 0x55, 0x59, 0x54, 0xFF,

 /* Mac */
 0x41, 0x41, 0x41, 0x45, 0x41, 0x4F, 0x55, 0x41, 0x41, 0x43, 0x41, 0x43, 0x43,
 0x43, 0x45, 0x5A, 0x5A, 0x44, 0x49, 0x44, 0x45, 0x45, 0x45, 0x4F, 0x45, 0x4F,
 0x4F, 0x4F, 0x55, 0x45, 0x45, 0x55, 0xA0, 0xA1, 0x45, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0x45, 0xAC, 0xAD, 0x47, 0x49, 0x49, 0x49, 0xB2, 0xB3,
 0x49, 0x4B, 0xB6, 0xB7, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4E, 0x4E,
 0x4E, 0xC2, 0xC3, 0x4E, 0x4E, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0x4E, 0x4F, 0x4F,
 0x4F, 0x4F, 0xD0, 0xD0, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0x4F, 0x52, 0x52,
 0x52, 0xDC, 0xDD, 0x52, 0x52, 0x52, 0x53, 0xE2, 0xE3, 0x53, 0x53, 0x53, 0x41,
 0x54, 0x54, 0x49, 0x5A, 0x5A, 0x55, 0x4F, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x55,
 0x55, 0x55, 0x55, 0x59, 0x59, 0x4B, 0x5A, 0x4C, 0x4C, 0x47, 0xFF
};

static const unsigned char Lookup_48[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

 /* Windows */
 0x7F, 0x7F, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x7F, 0x89, 0x53, 0x8B, 0x8C,
 0x7F, 0x7F, 0x7F, 0x7F, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x96, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x7F, 0x7F, 0x59, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x96, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0x55, 0x59, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0x4F, 0xF7, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x59, 0xDE, 0x59,

 /* Mac */
 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C,
 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAB, 0xAD, 0xAE, 0xAE, 0xB0, 0xB1, 0xB2, 0xB3,
 0xA7, 0xB5, 0xB6, 0xB7, 0xB8, 0xB8, 0xBA, 0xBA, 0xBC, 0xBC, 0xBE, 0xBE, 0xB7,
 0xC1, 0xC2, 0xC3, 0x46, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCB, 0xCD,
 0xCD, 0xC1, 0xD0, 0xD0, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD8, 0xDA,
 0xDA, 0xDC, 0xDD, 0xDD, 0x9F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94,
 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F
};

static const unsigned char Lookup_64[128 * 3] = {
 /* Common */
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x96, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0x55, 0x55, 0xDE, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

 /* Mac */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x96, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0x55, 0x55, 0xDE, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char Lookup_80[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55,

 /* Mac */
 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55,
};

static const unsigned char Lookup_112[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55,

 /* Mac */
 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55,
};

static const unsigned char Lookup_128[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x00,

/* Windows */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x00, 0x8B, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x00, 0x99,
 0x00, 0x9B, 0x00, 0x00, 0x00, 0x00, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x00, 0xAB, 0xAC, 0x2D, 0xAE, 0x2D, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xBA,
 0xA2, 0xC2, 0xC3, 0xC4, 0xB8, 0xC6, 0xB9, 0xC8, 0xBA, 0xCA, 0xCB, 0xCC, 0xCD,
 0xCE, 0xBC, 0xD0, 0xD1, 0x00, 0xD3, 0xD4, 0xBE, 0xD6, 0xD7, 0xD8, 0xBF, 0xBA,
 0xBE, 0xA2, 0xB8, 0xB9, 0xBA, 0xBE, 0xA2, 0xC2, 0xC3, 0xC4, 0xB8, 0xC6, 0xB9,
 0xC8, 0xBA, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xBC, 0xD0, 0xD1, 0xD3, 0xD3, 0xD4,
 0xBE, 0xD6, 0xD7, 0xD8, 0xBF, 0xBA, 0xBE, 0xBC, 0xBE, 0xBF, 0x00,

 /* Mac */
 0x41, 0x31, 0x32, 0x45, 0x33, 0x4F, 0x55, 0x87, 0x41, 0x41, 0x41, 0x00, 0x8C,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x92, 0x93, 0x49, 0x49, 0x96, 0x97, 0x98, 0x4F,
 0x4F, 0x9B, 0x3F, 0x55, 0x55, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xAB, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xB0,
 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0x09, 0xBD, 0xCC, 0xB0,
 0xB6, 0xCF, 0x2D, 0x2D, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xB8, 0xAB, 0xC3, 0xBD,
 0xB6, 0xB8, 0xAB, 0xC3, 0xBF, 0xBD, 0xB0, 0xB5, 0xBE, 0xA2, 0xB6, 0xBC, 0xA1,
 0xB8, 0xAB, 0xA5, 0xBA, 0xA4, 0xBB, 0xC1, 0xC3, 0xA6, 0xBF, 0xC4, 0xAA, 0xC6,
 0xA3, 0xBF, 0xAA, 0xCC, 0xBD, 0xB7, 0xAB, 0xBD, 0xAB, 0xBD, 0x3F,
};

static const unsigned char Lookup_144[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x00,

/* Windows */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0xC1, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43, 0x45, 0xC9, 0x45, 0x45, 0x49, 0xCD,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0xD3, 0x4F, 0x4F, 0xD6, 0xD7, 0xD6, 0x55, 0xDA,
 0x55, 0x55, 0xDD, 0xDE, 0xDF, 0x41, 0xC1, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43,
 0x45, 0xC9, 0x45, 0x45, 0x49, 0xCD, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0xD3, 0x4F,
 0x4F, 0xD6, 0xF7, 0xD6, 0x55, 0xDA, 0x55, 0x55, 0xDD, 0xDE, 0x59,

 /* Mac */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0xC1, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43, 0x45, 0xC9, 0x45, 0x45, 0x49, 0xCD,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0xD3, 0x4F, 0x4F, 0xD6, 0xD7, 0xD6, 0x55, 0xDA,
 0x55, 0x55, 0xDD, 0xDE, 0xDF, 0x41, 0xC1, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43,
 0x45, 0xC9, 0x45, 0x45, 0x49, 0xCD, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0xD3, 0x4F,
 0x4F, 0xD6, 0xF7, 0xD6, 0x55, 0xDA, 0x55, 0x55, 0xDD, 0xDE, 0x59,
};

static const unsigned char Lookup_160[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x00,

/* Windows */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0xC7, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0xD6, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0xDC, 0xDD, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0xC7,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0xD6, 0xF7, 0x4F, 0x55, 0x55, 0x55, 0xDC, 0xDD, 0xDE, 0x59,

 /* Mac */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0xC7, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0xD6, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0xDC, 0xDD, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0xC7,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0xD6, 0xF7, 0x4F, 0x55, 0x55, 0x55, 0xDC, 0xDD, 0xDE, 0x59,
};

static const unsigned char Lookup_176[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x00,

 /* Windows */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0xC4, 0xC5, 0xC4, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0xD6, 0xD7, 0xD6, 0x55, 0x55,
 0x55, 0x59, 0x59, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0xC4, 0xC5, 0xC4, 0x43,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0xD6, 0xF7, 0xD6, 0x55, 0x55, 0x55, 0x59, 0x59, 0xDE, 0x59,

 /* Mac */
 0x80, 0x81, 0x43, 0x45, 0x4E, 0x85, 0x59, 0x41, 0x41, 0x41, 0x80, 0x41, 0x81,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4E, 0x4F, 0x4F, 0x4F,
 0x85, 0x4F, 0x55, 0x55, 0x55, 0x59, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0x80, 0x85, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0x41, 0x4F, 0xBD, 0x80, 0x85, 0xC0,
 0xC1, 0xC2, 0xC3, 0x46, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0x09, 0x41, 0x41, 0x4F,
 0xCE, 0xCE, 0x2D, 0x2D, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0x59, 0x59, 0xDA,
 0xDB, 0xDC, 0xDD, 0x3F, 0x3F, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0x41, 0x45, 0x41,
 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4F, 0x4F, 0x3F, 0x4F, 0x55, 0x55, 0x55,
 0x49, 0x00, 0xF7, 0x00, 0xF9, 0xFA, 0xFB, 0x3F, 0xFD, 0xFE, 0x00
};

static const unsigned char Lookup_208[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x80, 0x81, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x5E, 0x89, 0x8A, 0x8B, 0x8C,
 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
 0x9A, 0x9B, 0x8C, 0x9D, 0x00, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
 0xC1, 0xC2, 0xC1, 0xC1, 0xC1, 0xC1, 0xC7, 0xC8, 0xC9, 0xC9, 0xCB, 0xCC, 0xCD,
 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
 0xDB, 0x00, 0xDD, 0xDE, 0xDF, 0x41, 0xE1, 0x41, 0xE3, 0xE4, 0xE5, 0xE6, 0x43,
 0x45, 0x45, 0x45, 0x45, 0xEC, 0xEC, 0x49, 0x49, 0xF0, 0xF1, 0xF2, 0xF3, 0x4F,
 0xF5, 0xF6, 0xF7, 0xF8, 0x55, 0xFA, 0x55, 0x55, 0x00, 0x00, 0xFF,

 /* Mac */
 0x41, 0x81, 0x43, 0x45, 0x4E, 0x4F, 0x55, 0x41, 0x41, 0x41, 0x41, 0x8B, 0x8C,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x93, 0x49, 0x49, 0x4E, 0x4F, 0x98, 0x4F,
 0x4F, 0x9B, 0x55, 0x55, 0x55, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
 0xC1, 0xC2, 0xC1, 0xC1, 0xC1, 0xC1, 0xC7, 0xC8, 0xC9, 0xC9, 0xCB, 0xCC, 0xCD,
 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0x00, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
 0xE8, 0xE9, 0xE9, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4,
 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

static const unsigned char Lookup_224[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x80, 0x81, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x5E, 0x89, 0x8A, 0x8B, 0x8C,
 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD,
 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
 0xE8, 0xE9, 0xEA, 0xEA, 0xEC, 0xED, 0xED, 0xEF, 0xEF, 0xF1, 0xF2, 0xF3, 0xF3,
 0xF5, 0xF5, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0x00, 0x00, 0xFF,

 /* Mac */
 0x41, 0x41, 0x43, 0x45, 0x4E, 0x4F, 0x55, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0x4F, 0x55, 0x55, 0x55, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD,
 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
 0xE8, 0xE9, 0xEA, 0xEA, 0xEC, 0xED, 0xED, 0xEF, 0xEF, 0xF1, 0xF2, 0xF3, 0xF3,
 0xF5, 0xF5, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

/***********************************************************************
 *		lhash_val_of_name_sys
 *
 * Copy of oleaut32.LHashValOfNameSysA
 * Produce a string hash value.
 *
 * PARAMS
 *  skind [I] Type of the system.
 *  lcid  [I] Locale id for the hash.
 *  lpStr [I] String to hash.
 *
 * RETURNS
 *  Success: The hash value of the string.
 *  Failure: 0, if lpStr is NULL.
 *
 * NOTES
 *  This function produces a two part hash: The high word is based on
 *  skind and lcid, while the low word is based on a repeated string
 *  hash of skind/str.
 */
unsigned int lhash_val_of_name_sys( syskind_t skind, LCID lcid, LPCSTR lpStr)
{
  ULONG nOffset, nMask = skind == SYS_MAC ? 1 : 0;
  ULONG nHiWord, nLoWord = 0x0deadbee;
  const unsigned char *str = (const unsigned char *)lpStr, *pnLookup = NULL;

  if (!str)
    return 0;

  switch (PRIMARYLANGID(LANGIDFROMLCID(lcid)))
  {
  default:
    fprintf(stderr, "Unknown lcid %x, treating as latin-based, please report\n", lcid);
    /* .. Fall Through .. */
  case LANG_AFRIKAANS:  case LANG_ALBANIAN:   case LANG_ARMENIAN:
  case LANG_ASSAMESE:   case LANG_AZERI:      case LANG_BASQUE:
  case LANG_BELARUSIAN: case LANG_BENGALI:    case LANG_BULGARIAN:
  case LANG_CATALAN:    case LANG_DANISH:     case LANG_DIVEHI:
  case LANG_DUTCH:      case LANG_ENGLISH:    case LANG_ESTONIAN:
  case LANG_FAEROESE:   case LANG_FINNISH:    case LANG_FRENCH:
  case LANG_GALICIAN:   case LANG_GEORGIAN:   case LANG_GERMAN:
  case LANG_GUJARATI:   case LANG_HINDI:      case LANG_INDONESIAN:
  case LANG_ITALIAN:    case LANG_KANNADA:    case LANG_KASHMIRI:
  case LANG_KAZAK:      case LANG_KONKANI:    case LANG_KYRGYZ:
  case LANG_LATVIAN:    case LANG_LITHUANIAN: case LANG_MACEDONIAN:
  case LANG_MALAY:      case LANG_MALAYALAM:  case LANG_MANIPURI:
  case LANG_MARATHI:    case LANG_MONGOLIAN:  case LANG_NEPALI:
  case LANG_ORIYA:      case LANG_PORTUGUESE: case LANG_PUNJABI:
  case LANG_ROMANIAN:   case LANG_SANSKRIT:   case LANG_SERBIAN:
  case LANG_SINDHI:     case LANG_SLOVENIAN:  case LANG_SWAHILI:
  case LANG_SWEDISH:    case LANG_SYRIAC:     case LANG_TAMIL:
  case LANG_TATAR:      case LANG_TELUGU:     case LANG_THAI:
  case LANG_UKRAINIAN:  case LANG_URDU:       case LANG_UZBEK:
  case LANG_VIETNAMESE: case LANG_MALTESE:    case LANG_IRISH:
  case LANG_SAMI:       case LANG_UPPER_SORBIAN: case LANG_TSWANA:
  case LANG_XHOSA:      case LANG_ZULU:       case LANG_WELSH:
  case LANG_BRETON:     case LANG_NEUTRAL:
/* some languages not in all windows versions or ReactOS */
#ifdef LANG_GAELIC
  case LANG_GAELIC:
#endif
#ifdef LANG_TAJIK
  case LANG_TAJIK:
#endif
#ifdef LANG_ROMANSH
  case LANG_ROMANSH:
#endif
#ifdef LANG_SUTU
  case LANG_SUTU:
#endif
#ifdef LANG_TSONGA
  case LANG_TSONGA:
#endif
#ifdef LANG_VENDA
  case LANG_VENDA:
#endif
#ifdef LANG_ESPERANTO
  case LANG_ESPERANTO:
#endif
#ifdef LANG_WALON
  case LANG_WALON:
#endif
#ifdef LANG_CORNISH
  case LANG_CORNISH:
#endif
    nOffset = 16;
    pnLookup = Lookup_16;
    break;
  case LANG_CZECH:  case LANG_HUNGARIAN:  case LANG_POLISH:
  case LANG_SLOVAK: case LANG_SPANISH:
    nOffset = 32;
    pnLookup = Lookup_32;
    break;
  case LANG_HEBREW:
    nOffset = 48;
    pnLookup = Lookup_48;
    break;
  case LANG_JAPANESE:
    nOffset = 64;
    pnLookup = Lookup_64;
    break;
  case LANG_KOREAN:
    nOffset = 80;
    pnLookup = Lookup_80;
    break;
  case LANG_CHINESE:
    nOffset = 112;
    pnLookup = Lookup_112;
    break;
  case LANG_GREEK:
    nOffset = 128;
    pnLookup = Lookup_128;
    break;
  case LANG_ICELANDIC:
    nOffset = 144;
    pnLookup = Lookup_144;
    break;
  case LANG_TURKISH:
    nOffset = 160;
    pnLookup = Lookup_160;
    break;
  case LANG_NORWEGIAN:
    if (SUBLANGID(LANGIDFROMLCID(lcid)) == SUBLANG_NORWEGIAN_NYNORSK)
    {
      nOffset = 176;
      pnLookup = Lookup_176;
    }
    else
    {
      nOffset = 16;
      pnLookup = Lookup_16;
    }
    break;
  case LANG_ARABIC:
  case LANG_FARSI:
    nOffset = 208;
    pnLookup = Lookup_208;
    break;
  case LANG_RUSSIAN:
    nOffset = 224;
    pnLookup = Lookup_224;
    break;
  }

  nHiWord = (nOffset | nMask) << 16;

  while (*str)
  {
    nLoWord = 37 * nLoWord + pnLookup[*str > 0x7f && nMask ? *str + 0x80 : *str];
    str++;
  }
  /* Constrain to a prime modulo and sizeof(WORD) */
  nLoWord = (nLoWord % 65599) & 0xffff;

  return nHiWord | nLoWord;
}
