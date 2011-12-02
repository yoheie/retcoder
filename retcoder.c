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
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif
#include "libcrlf.h"

static int retcoder_check_options(int argc, char *argv[]);
static void retcoder_print_version(FILE *pf);
static void retcoder_print_usage(FILE *pf);
static void retcoder_main(const char *filename);
static int retcoder_getc(void);
static int retcoder_putc(int c);
static int retcoder_put_crlf(crlf_type_t type);
static void retcoder_print_input_type(FILE *pf, const char *filename);

static struct {
	int show_help;
	int show_version;
	int check_only;
	int show_input_type;
	const char *replace_str;
	int escape;
} retcoder_params = {
	0,
	0,
	0,
	0,
	NULL,
	0
};

static FILE *p_retcoder_input_file;
static unsigned int retcoder_input_type_flags;

int main(int argc, char *argv[])
{
	int n;

	n = retcoder_check_options(argc, argv);

	if (n < 1) {
		/* Parameter Error */
		fprintf(stderr, "error: invalid option\n");
		fprintf(stderr, "\n");
		retcoder_print_usage(stderr);
		return 1;
	}

	if (retcoder_params.show_help) {
		retcoder_print_version(stdout);
		fprintf(stdout, "\n");
		retcoder_print_usage(stdout);
		return 0;
	}

	if (retcoder_params.show_version) {
		retcoder_print_version(stdout);
		return 0;
	}

#ifdef _WIN32
	if (!retcoder_params.check_only) {
		_setmode(_fileno(stdout), _O_BINARY);
	}
#endif
	if (n < argc) {
		/* Read from file */
		for ( ; n < argc; n++) {
			p_retcoder_input_file = fopen(argv[n], "rb");
			if (p_retcoder_input_file == NULL) {
				fprintf(stderr, "error: file %s not found!!\n", argv[n]);
			}
			else {
				retcoder_main(argv[n]);
			}
		}
	}
	else {
		/* Read from stdin */
#ifdef _WIN32
		_setmode(_fileno(stdin), _O_BINARY);
#endif
		p_retcoder_input_file = stdin;
		retcoder_main("stdin");
	}

	return 0;
}

static int retcoder_check_options(int argc, char *argv[])
{
	int n;
	int m;

	for (n = 1; n < argc; n++) {
		if (argv[n][0] == '-') {
			if (argv[n][1] == '-') {
				/* Long options */
				if (argv[n][2] == '\0') {
					n++;
					break;
				}
				else if (strcmp(&argv[n][2], "help") == 0) {
					retcoder_params.show_help = 1;
				}
				else if (strcmp(&argv[n][2], "version") == 0) {
					retcoder_params.show_version = 1;
				}
				else if (strcmp(&argv[n][2], "check") == 0) {
					retcoder_params.check_only = 1;
				}
				else if (strcmp(&argv[n][2], "verbose") == 0) {
					retcoder_params.show_input_type = 1;
				}
				else if (strcmp(&argv[n][2], "mac") == 0) {
					retcoder_params.replace_str = "\r";
					retcoder_params.escape = 0;
				}
				else if (strcmp(&argv[n][2], "unix") == 0) {
					retcoder_params.replace_str = "\n";
					retcoder_params.escape = 0;
				}
				else if (strcmp(&argv[n][2], "dos") == 0) {
					retcoder_params.replace_str = "\r\n";
					retcoder_params.escape = 0;
				}
				else if (strcmp(&argv[n][2], "cr") == 0) {
					retcoder_params.replace_str = "\r";
					retcoder_params.escape = 0;
				}
				else if (strcmp(&argv[n][2], "lf") == 0) {
					retcoder_params.replace_str = "\n";
					retcoder_params.escape = 0;
				}
				else if (strcmp(&argv[n][2], "crlf") == 0) {
					retcoder_params.replace_str = "\r\n";
					retcoder_params.escape = 0;
				}
				else if (strcmp(&argv[n][2], "space") == 0) {
					retcoder_params.replace_str = " ";
					retcoder_params.escape = 0;
				}
				else if (strcmp(&argv[n][2], "tab") == 0) {
					retcoder_params.replace_str = "\t";
					retcoder_params.escape = 0;
				}
				else if (strcmp(&argv[n][2], "none") == 0) {
					retcoder_params.replace_str = "";
					retcoder_params.escape = 0;
				}
				else if (strncmp(&argv[n][2], "str", 3) == 0) {
					if (argv[n][5] == '\0') {
						if (n < argc - 1) {
							n++;
							retcoder_params.replace_str = argv[n];
							retcoder_params.escape = 0;
						}
						else {
							return -1;
						}
					}
					else if (argv[n][5] == '=') {
						retcoder_params.replace_str = &argv[n][6];
						retcoder_params.escape = 0;
					}
					else {
						return -1;
					}
				}
				else if (strncmp(&argv[n][2], "estr", 4) == 0) {
					if (argv[n][6] == '\0') {
						if (n < argc - 1) {
							n++;
							retcoder_params.replace_str = argv[n];
							retcoder_params.escape = 1;
						}
						else {
							return -1;
						}
					}
					else if (argv[n][6] == '=') {
						retcoder_params.replace_str = &argv[n][7];
						retcoder_params.escape = 1;
					}
					else {
						return -1;
					}
				}
				else {
					return -1;
				}
			}
			else if (argv[n][1] != '\0') {
				/* Short options */
				m = 1;
				while (argv[n][m] != '\0') {
					switch (argv[n][m]) {
					case 'h':
						retcoder_params.show_help = 1;
						break;
					case 'c':
						retcoder_params.check_only = 1;
						break;
					case 'v':
						retcoder_params.show_input_type = 1;
						break;
					case 'm':
						retcoder_params.replace_str = "\r";
						retcoder_params.escape = 0;
						break;
					case 'u':
						retcoder_params.replace_str = "\n";
						retcoder_params.escape = 0;
						break;
					case 'd':
						retcoder_params.replace_str = "\r\n";
						retcoder_params.escape = 0;
						break;
					case 's':
						retcoder_params.replace_str = " ";
						retcoder_params.escape = 0;
						break;
					case 't':
						retcoder_params.replace_str = "\t";
						retcoder_params.escape = 0;
						break;
					case 'n':
						retcoder_params.replace_str = "";
						retcoder_params.escape = 0;
						break;
					default:
						return -1;
					}
					m++;
				}
			}
			else {
				return -1;
			}
		}
		else {
			break;
		}
	}

	return n;
}

static const char retcoder_version_string[] =
"retcoder - return code checking/converting utility\n"
"  version " VERSION "\n"
;

static const char retcoder_usage_string[] =
" usage:\n"
"  retcoder [options] [file ...]\n"
" options:\n"
"  -m, --mac, --cr      convert return code to mac(cr)\n"
"  -u, --unix, --lf     convert return code to unix(lf)\n"
"  -d, --dos, --crlf    convert return code to dos(cr+lf)\n"
"  -s, --space          convert return code to single space\n"
"  -t, --tab            convert return code to single tab\n"
"  -n, --none           remove return code\n"
"  --str <string>, --str=<string>\n"
"                       convert return code to <string>\n"
"  --estr <string>, --estr=<string>\n"
"                       convert return code to <string>, escape sequence enabled\n"
"                         \\a: BEL (07h) \\b: BS  (08h) \\t: HT  (09h)\n"
"                         \\n: LF  (0Ah) \\v: VT  (0Bh) \\f: FF  (0Ch)\n"
"                         \\r: CR  (0Dh) \\\\: Backslash (5Ch)\n"
"                         \\@: input return code\n"
"                         \\NNN or \\0NNN: octal NNN (1 to 3 digits)\n"
"                         \\xnn: hexadecimal nn (1 or 2 digits)\n"
"  -v, --verbose        print input return code type to stderr\n"
"  -c, --check          print input return code type to stdout, and not convert\n"
"  --version            print version\n"
"  -h, --help           print help\n"
"  --                   no more options\n"
;

static void retcoder_print_version(FILE *pf)
{
	fprintf(pf, retcoder_version_string);
}

static void retcoder_print_usage(FILE *pf)
{
	fprintf(pf, retcoder_usage_string);
}

static void retcoder_main(const char *filename)
{
	retcoder_input_type_flags = 0;
	crlf_convert(
			retcoder_getc,
			retcoder_params.check_only ? NULL : retcoder_putc,
			retcoder_put_crlf
			);
	if (retcoder_params.check_only) {
		retcoder_print_input_type(stdout, filename);
	}
	else if (retcoder_params.show_input_type) {
		retcoder_print_input_type(stderr, filename);
	}
}

static int retcoder_getc(void)
{
	int ret;

	ret = getc(p_retcoder_input_file);

	if (ret == EOF) {
		if (ferror(p_retcoder_input_file) != 0) {
			fprintf(stderr, "error: failed to get character.\n");
		}
	}

	return ret;
}

static int retcoder_putc(int c)
{
	int ret;

	ret = putc(c, stdout);

	if (ret == EOF) {
		fprintf(stderr, "error: failed to put character.\n");
	}

	return ret;
}

static int retcoder_put_crlf(crlf_type_t type)
{
	int ret = 0;
	const char *replace_str;
	int escape;
	int c;

	retcoder_input_type_flags |= (1u << type);

	if (retcoder_params.check_only) {
		if ((retcoder_input_type_flags & ~(1u << type)) != 0) {
			ret = EOF;
		}
	}
	else {
		if (retcoder_params.replace_str == NULL) {
			switch (type) {
			case CRLF_TYPE_CR:   replace_str = "\r";   break;
			case CRLF_TYPE_LF:   replace_str = "\n";   break;
			case CRLF_TYPE_CRLF: replace_str = "\r\n"; break;
			}
			escape = 0;
		}
		else {
			replace_str = retcoder_params.replace_str;
			escape = retcoder_params.escape;
		}
		if (escape) {
			while (*replace_str != '\0') {
				if (*replace_str == '\\') {
					replace_str++;
					switch(*replace_str) {
					case 'a':
						c = putchar('\a');
						break;
					case 'b':
						c = putchar('\b');
						break;
					case 't':
						c = putchar('\t');
						break;
					case 'n':
						c = putchar('\n');
						break;
					case 'v':
						c = putchar('\v');
						break;
					case 'f':
						c = putchar('\f');
						break;
					case 'r':
						c = putchar('\r');
						break;
					case '\\':
						c = putchar('\\');
						break;
					case '@':
						switch (type) {
						case CRLF_TYPE_CR:
							c = putchar('\r');
							break;
						case CRLF_TYPE_LF:
							c = putchar('\n');
							break;
						case CRLF_TYPE_CRLF:
							c = putchar('\r');
							if (c != EOF) {
								c = putchar('\n');
							}
							break;
						}
						break;
					case 'x':
						replace_str++;
						if ((('0' <= *replace_str) && (*replace_str <= '9'))
						 || (('A' <= *replace_str) && (*replace_str <= 'F'))
						 || (('a' <= *replace_str) && (*replace_str <= 'f'))
						) {
							int i;
							int h = 0;
							for (i = 0; i < 2; i++) {
								if (('0' <= *replace_str) && (*replace_str <= '9')) {
									h = h * 16 + (*replace_str - '0');
									replace_str++;
								}
								else if (('A' <= *replace_str) && (*replace_str <= 'F')) {
									h = h * 16 + (*replace_str - 'A') + 10;
									replace_str++;
								}
								else if (('a' <= *replace_str) && (*replace_str <= 'f')) {
									h = h * 16 + (*replace_str - 'a') + 10;
									replace_str++;
								}
								else {
									break;
								}
							}
							replace_str--;
							c = putchar(h);
						}
						else {
							replace_str--;
							c = putchar('\\');
							if (c != EOF) {
								c = putchar('x');
							}
						}
						break;
					default:
						if (('0' <= *replace_str) && (*replace_str <= '7')) {
							int i;
							int o = 0;
							if (*replace_str == '0') {
								replace_str++;
							}
							for (i = 0; i < 3; i++) {
								if (('0' <= *replace_str) && (*replace_str <= '7')) {
									o = o * 8 + (*replace_str - '0');
									replace_str++;
								}
								else {
									break;
								}
							}
							replace_str--;
							c = putchar(o);
						}
						else {
							replace_str--;
							c = putchar('\\');
						}
						break;
					}
				}
				else {
					c = putchar(*replace_str);
				}
				if (c == EOF) {
					fprintf(stderr, "error: failed to put character.\n");
					ret = EOF;
					break;
				}
				replace_str++;
			}
		}
		else {
			if (printf("%s", replace_str) < 0) {
				fprintf(stderr, "error: failed to put character.\n");
				ret = EOF;
			}
		}
	}

	return ret;
}

static void retcoder_print_input_type(FILE *pf, const char *filename)
{
	const char *type_str;

	if (retcoder_input_type_flags == 0) {
		type_str = "NORC";
	}
	else if ((retcoder_input_type_flags & ~(1u << CRLF_TYPE_CR)) == 0) {
		type_str = "MAC";
	}
	else if ((retcoder_input_type_flags & ~(1u << CRLF_TYPE_LF)) == 0) {
		type_str = "UNIX";
	}
	else if ((retcoder_input_type_flags & ~(1u << CRLF_TYPE_CRLF)) == 0) {
		type_str = "DOS";
	}
	else {
		type_str = "MIXED";
	}

	fprintf(pf, "%s:\t%s\n", filename, type_str);
}
