/*
 * Copyright (c) 2011,2012 Yohei Endo <yoheie@gmail.com>
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
#include <stdio.h>
#include <wchar.h>
#include "libcrlf.h"

void crlf_convert(
		pf_crlf_getc_t pf_getc,
		pf_crlf_putc_t pf_putc,
		pf_crlf_put_crlf_t pf_put_crlf
		)
{
	int c;
	int prev_cr;

	prev_cr = 0;

	if (pf_getc != NULL) {
		do {
			c = (*pf_getc)();
			if (c == (unsigned char)'\n') {
				if (prev_cr) {
					prev_cr = 0;
					/* CRLF */
					if (pf_put_crlf != NULL) {
						if((*pf_put_crlf)(CRLF_TYPE_CRLF) == EOF) {
							break;
						}
					}
				}
				else {
					/* LF */
					if (pf_put_crlf != NULL) {
						if((*pf_put_crlf)(CRLF_TYPE_LF) == EOF) {
							break;
						}
					}
				}
			}
			else {
				if (prev_cr) {
					prev_cr = 0;
					/* CR */
					if (pf_put_crlf != NULL) {
						if((*pf_put_crlf)(CRLF_TYPE_CR) == EOF) {
							break;
						}
					}
				}
				if (c == (unsigned char)'\r') {
					prev_cr = 1;
				}
				else if (c != EOF) {
					/* Normal character */
					if (pf_putc != NULL) {
						if ((*pf_putc)(c) == EOF) {
							break;
						}
					}
				}
				else {
					break;
				}
			}
		} while (c != EOF);
	}
}

void crlf_convertw(
		pf_crlf_getwc_t pf_getwc,
		pf_crlf_putwc_t pf_putwc,
		pf_crlf_put_wcrlf_t pf_put_wcrlf
		)
{
	wint_t wc;
	int prev_cr;

	prev_cr = 0;

	if (pf_getwc != NULL) {
		do {
			wc = (*pf_getwc)();
			if (wc == L'\n') {
				if (prev_cr) {
					prev_cr = 0;
					/* CRLF */
					if (pf_put_wcrlf != NULL) {
						if((*pf_put_wcrlf)(CRLF_TYPE_CRLF) == WEOF) {
							break;
						}
					}
				}
				else {
					/* LF */
					if (pf_put_wcrlf != NULL) {
						if((*pf_put_wcrlf)(CRLF_TYPE_LF) == WEOF) {
							break;
						}
					}
				}
			}
			else {
				if (prev_cr) {
					prev_cr = 0;
					/* CR */
					if (pf_put_wcrlf != NULL) {
						if((*pf_put_wcrlf)(CRLF_TYPE_CR) == WEOF) {
							break;
						}
					}
				}
				if (wc == L'\r') {
					prev_cr = 1;
				}
				else if (wc != WEOF) {
					/* Normal character */
					if (pf_putwc != NULL) {
						if ((*pf_putwc)(wc) == WEOF) {
							break;
						}
					}
				}
				else {
					break;
				}
			}
		} while (wc != WEOF);
	}
}
