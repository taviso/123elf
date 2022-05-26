/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2008-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains definitions for required C99 integer types.
 */

#ifndef __CACA_TYPES_H__
#define __CACA_TYPES_H__

typedef signed char _caca_int8_t;
typedef signed short _caca_int16_t;
typedef signed long int _caca_int32_t;
#   undef int8_t
#   define int8_t _caca_int8_t
#   undef int16_t
#   define int16_t _caca_int16_t
#   undef int32_t
#   define int32_t _caca_int32_t
typedef unsigned char _caca_uint8_t;
typedef unsigned short _caca_uint16_t;
typedef unsigned long int _caca_uint32_t;
#   undef uint8_t
#   define uint8_t _caca_uint8_t
#   undef uint16_t
#   define uint16_t _caca_uint16_t
#   undef uint32_t
#   define uint32_t _caca_uint32_t
typedef long int _caca_intptr_t;
typedef unsigned long int _caca_uintptr_t;
#   undef intptr_t
#   define intptr_t _caca_intptr_t
#   undef uintptr_t
#   define uintptr_t _caca_uintptr_t
typedef int _caca_ssize_t;
typedef unsigned int _caca_size_t;
#   undef ssize_t
#   define ssize_t _caca_ssize_t
#   undef size_t
#   define size_t _caca_size_t

#endif

