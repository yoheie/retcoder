/*
 * Copyright (c) 2011 Yohei Endo <yoheie@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not
 *     be misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *     distribution.
 */
#ifndef LIBCRLF_H__
#define LIBCRLF_H__

#include <wchar.h>

typedef enum {
	CRLF_TYPE_CR,
	CRLF_TYPE_LF,
	CRLF_TYPE_CRLF
} crlf_type_t;

typedef int (*pf_crlf_getc_t)(void);
typedef int (*pf_crlf_putc_t)(int c);
typedef int (*pf_crlf_put_crlf_t)(crlf_type_t type);

typedef wint_t (*pf_crlf_getwc_t)(void);
typedef wint_t (*pf_crlf_putwc_t)(wchar_t c);
typedef wint_t (*pf_crlf_put_wcrlf_t)(crlf_type_t type);

void crlf_convert(
		pf_crlf_getc_t pf_getc,
		pf_crlf_putc_t pf_putc,
		pf_crlf_put_crlf_t pf_put_crlf
		);

void crlf_convertw(
		pf_crlf_getwc_t pf_getwc,
		pf_crlf_putwc_t pf_putwc,
		pf_crlf_put_wcrlf_t pf_put_wcrlf
		);

#endif /* LIBCRLF_H__ */
