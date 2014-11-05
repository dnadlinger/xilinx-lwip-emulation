CC=gcc
CFLAGS=-g -Wall -pedantic
LDFLAGS=-lpthread -lutil
ARFLAGS=rs

APPSRC=../xsdk/src
LIBDIR=lib
LWIPDIR=$(LIBDIR)/lwip-1.4.1/src
LWIPUNIXDIR=$(LIBDIR)/lwip-contrib-1.4.1/ports/unix

CFLAGS:=$(CFLAGS) -Iinclude -Isrc \
	-I$(LWIPDIR)/include -I$(LWIPUNIXDIR)/include -I$(LWIPDIR)/include/ipv4 \
	-I$(LWIPUNIXDIR)/proj/minimal -I$(APPSRC) -Dlinux
APP_CFLAGS= $(CFLAGS) -std=c99

# Source files for the LWIP Unix port.

COREFILES=$(LWIPDIR)/core/mem.c $(LWIPDIR)/core/memp.c $(LWIPDIR)/core/netif.c \
	$(LWIPDIR)/core/pbuf.c $(LWIPDIR)/core/raw.c $(LWIPDIR)/core/stats.c \
	$(LWIPDIR)/core/sys.c $(LWIPDIR)/core/tcp.c $(LWIPDIR)/core/tcp_in.c \
	$(LWIPDIR)/core/tcp_out.c $(LWIPDIR)/core/udp.c $(LWIPDIR)/core/dhcp.c \
	$(LWIPDIR)/core/init.c $(LWIPDIR)/core/timers.c $(LWIPDIR)/core/def.c
CORE4FILES=$(wildcard $(LWIPDIR)/core/ipv4/*.c) $(LWIPDIR)/core/ipv4/inet.c \
	$(LWIPDIR)/core/ipv4/inet_chksum.c
NETIFFILES=$(LWIPDIR)/netif/etharp.c $(LWIPDIR)/netif/slipif.c
ARCHFILES=$(wildcard $(LWIPUNIXDIR)/*.c) $(LWIPUNIXDIR)/netif/tapif.c \
	$(LWIPUNIXDIR)/netif/tunif.c $(LWIPUNIXDIR)/netif/unixif.c \
	$(LWIPUNIXDIR)/netif/list.c $(LWIPUNIXDIR)/netif/tcpdump.c

LWIPFILES=$(COREFILES) $(CORE4FILES) $(NETIFFILES) $(ARCHFILES)
LWIPFILESW=$(wildcard $(LWIPFILES))
LWIPOBJS=$(notdir $(LWIPFILESW:.c=.o))
LWIPLIB=liblwip4.a

# Emulation source files.

EMUFILES=$(wildcard src/*.c) $(LWIPUNIXDIR)/proj/minimal/mintapif.c
EMUFILESW=$(wildcard $(EMUFILES))
EMUOBJS=$(notdir $(EMUFILESW:.c=.o))
EMULIB=libemu.a

# Source files for the application itself.

APPFILES=$(APPSRC)/main.c
APPOBJS=$(notdir $(APPFILES:.c=.o))

.PHONY: all clean depend
all: xilinx_hardware_emu
clean:
	rm -f *.o $(LWIPLIB) $(EMULIB) xilinx_hardware_emu .depend

%.o:
	$(CC) $(CFLAGS) -c $(<:.o=.c)

$(APPOBJS) $(EMUOBJS):
	$(CC) $(APP_CFLAGS) -c $(<:.o=.c)

$(LWIPLIB): $(LWIPOBJS)
	$(AR) $(ARFLAGS) $(LWIPLIB) $?

$(EMULIB): $(EMUOBJS)
	$(AR) $(ARFLAGS) $(EMULIB) $?

depend dep: .depend
include .depend
.depend: $(LWIPFILES) $(EMUFILES) $(APPFILES)
	$(CC) $(CFLAGS) -MM $^ > .depend || rm -f .depend

xilinx_hardware_emu: .depend $(LWIPLIB) $(EMULIB) $(APPOBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o xilinx_hardware_emu $(APPOBJS) $(EMULIB) $(LWIPLIB)
