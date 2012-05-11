# Copyright (c) 2011,2012 Yohei Endo <yoheie@gmail.com>
#
# This software is provided 'as-is', without any express or implied warranty. In
# no event will the authors be held liable for any damages arising from the use
# of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it freely,
# subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not
#     claim that you wrote the original software. If you use this software in a
#     product, an acknowledgment in the product documentation would be
#     appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not be
#     misrepresented as being the original software.
#
#     3. This notice may not be removed or altered from any source distribution.

VERSION = 2.1.0

DOCUMENTS = ChangeLog License Makefile
SOURCES   = retcoder.c libcrlf.c
HEADERS   = libcrlf.h

OBJECTS   = $(SOURCES:.c=.o)
DEPFILES  = $(SOURCES:.c=.d)
TARGET    = retcoder

DISTDIR    = retcoder-$(VERSION)
DISTFILE   = retcoder-$(VERSION).tar.gz

INSTALLDIR = /usr/local/bin

CC = gcc
LD = gcc
RM = rm -f

CFLAGS  = -DVERSION=\"$(VERSION)\"
LDFLAGS = -s

.PHONY:			all clean depend install dist

all:			$(TARGET)

$(TARGET):		$(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

%.o:			%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECTS):		Makefile

clean:
	$(RM) $(TARGET) $(OBJECTS) $(DEPFILES)

depend:			$(DEPFILES)

%.d:			%.c
	$(CC) $(CFLAGS) -MM -MT $(<:.c=.o) -MT $@ $< -o $@

$(DEPFILES):	Makefile

install:		$(TARGET)
	install -d -m 755 $(INSTALLDIR)
	install -m 755 -s $(TARGET) $(INSTALLDIR)/$(TARGET)

dist:
	rm -Rf $(DISTDIR)
	mkdir $(DISTDIR)
	cp -R $(DOCUMENTS) $(SOURCES) $(HEADERS) $(DISTDIR)
	tar zcf $(DISTFILE) $(DISTDIR)
	rm -Rf $(DISTDIR)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),dist)
include $(DEPFILES)
endif
endif
