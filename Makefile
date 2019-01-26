# -----------------------------------------------------------------------------
# 
#                              M A K E F I L E
# 
# zum HHUos der Lehrveranstaltung 'Betriebssystem-Entwicklung'.
# -----------------------------------------------------------------------------
#
# Compilieren: ‘make' 
# Ausfuehren mit Qemu: ‘make qemu’  
# GDB mit Qemu: ‘make qemu-gdb &’ und danach ‘make gdb’
# DDD mit Qemu: ‘make qemu-gdb &’ und danach ‘make ddd’
#
# Aufraeumen mit: ‘make clean’
#
# Achtung: Erzeugen von bootfaehigen Medien erfordert das Anpassen folgender
#	   Variablen: DRIVE_FD und DRIVE_HD. Aufpassen, ansonsten wir evt.
#	   die System-Disk zerstoert.
#
# Erzeugen einer bootfaehigen Diskette mit: ‘make bootdisk’
# Erzeugen eines bootfaehigen USB-Sticks mit: ‘make bootdisk-usb’
# Erzeugen eines bootfaehigen Festplatte mit: ‘make bootdisk-hd’
#
# -----------------------------------------------------------------------------
# Autor: Olaf Spinzcyk, TU Dortmund
#

# -----------------------------------------------------------------------------
# Liste der Quelltexte:

STARTUP_SOURCE = ./startup.asm
CC_SOURCES = $(shell find . -name "*.cc")
C_SOURCES = $(shell find . -name "*.c")
ASM_SOURCES = $(shell find ./kernel -name "*.asm")

# Einstellungen in Abhaengigkeit vom Generierungssystem:
#
#    ASMOBJFORMAT: Ausgabeformat fuer den Assembler. Das Format muss dem
#                  jeweiligen Format des verwendeten C++ Compilers angepasst
#                  werde, damit gemischte Objektdateien gelinkt werden koennen.
#    OBJDIR:       Verzeichnis, in dem die Objektdateien abgelegt werden
#                  sollen.
#    DEPDIR:       Verzeichnis, in dem die Abhaengigkeitsdateien abgelegt werden
#                  sollen
#    DRIVE_FD:     Diskettenlaufwerk, auf das das System-Image geschrieben wird
#                  (Target: bootdisk)
#    DRIVE_HD:     Festplatten-/USB-Device, auf das das System-Image
#                  geschrieben wird (Target: bootdisk-hd)
#    DELETE:       Name des Kommandos zum Loeschen von Dateien
#    ASM:          Zu benutzender Assembler
#    CC/CXX:       Zu benutzender C/C++-Compiler
#    CFLAGS:       Flags fuer den C-Compileraufruf
#    CXXFLAGS:     Flags fuer den C++-Compileraufruf
#    LD:           Zu benutzender Linker
#    LDFLAGS:      Flags fuer den Linkeraufruf
#    LDLIBS:       Dazuzulinkende Bibliotheken
#    LDHEAD:       Am Anfang zu linkende Dateien
#    LDTAIL:       Am Ende zu linkende Dateien
#    BOOT:         Das Verzeichnis zu Bootsektor und Setup-Code
#    TOOLS:        Das Verzeichnis mit dem Build-Tool

VERBOSE = @
ASMOBJFORMAT = elf
OBJDIR = ./build
DEPDIR = ./dep
DRIVE_FD = /dev/sdb
# ACHTUNG: ein falsch angegebenes Laufwerk kann dazu fuehren, dass Daten auf dem
# spezifizierten Laufwerk verloren gehen! Nicht mit root-Rechten ausfuehren!
DRIVE_HD = /dev/sdz
DELETE = rm
ASM = nasm
CC ?= gcc
CXX ?= g++
CFLAGS := $(CFLAGS) -m32 -Wall -Wno-write-strings -fno-stack-protector -nostdlib -I. -g #-DDEBUG 
CXXFLAGS := $(CFLAGS) -Wno-non-virtual-dtor -fno-threadsafe-statics -fno-use-cxa-atexit -fno-rtti -fno-exceptions 

BOOT = ../boot
TOOLS = ../tools
# BIOS-dev.code:total-tracks:-heads:-sectors:start-track:-head:-sector
# Default-Werte fuer Boot von Floppy (USB/HD erkennt bootsect.asm selbst):
BOOTDEVICE = 0:80:2:18:0:0:1

# -------------------------------------------------------------------------
# Namen der Unterverzeichnisse mit den Quelltexten

VPATH = $(sort $(dir $(STARTUP_SOURCE) $(CC_SOURCES) $(C_SOURCES) $(ASM_SOURCES)))

# -------------------------------------------------------------------------
# Listen mit den Objektdateien, die beim Kompilieren entstehen:

FIRST_OBJECT = $(addprefix $(OBJDIR)/,$(patsubst %.asm,_%.o, $(notdir $(STARTUP_SOURCE))))
C_OBJECTS = $(notdir $(C_SOURCES:.c=.o))
CC_OBJECTS = $(notdir $(CC_SOURCES:.cc=.o))

DEP_FILES = $(patsubst %.o,$(DEPDIR)/%.d,$(C_OBJECTS))
DEP_FILES += $(patsubst %.o,$(DEPDIR)/%.d,$(CC_OBJECTS))

ASM_OBJECTS = $(patsubst %.asm,_%.o, $(notdir $(ASM_SOURCES)))
OBJPRE = $(addprefix $(OBJDIR)/,$(ASM_OBJECTS) $(C_OBJECTS) $(CC_OBJECTS))

# --------------------------------------------------------------------------
# Default targets (einfaches Image, Image fuer USB Sticks, Image fuer VMWare
# und Boot CDs)

all: $(OBJDIR)/bootdisk.img $(OBJDIR)/bootdisk.vmi $(OBJDIR)/bootdisk-hd.vmi

# --------------------------------------------------------------------------
# Regeln zur Erzeugung der Abhaengigkeitsdateien

$(DEPDIR)/%.d : %.c
	@echo "DEP		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) $(CFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

$(DEPDIR)/%.d : %.cc
	@echo "DEP		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) $(CXXFLAGS) -MM -MT $(OBJDIR)/$*.o -MF $@ $<

# --------------------------------------------------------------------------
# Regeln zur Erzeugung der Objektdateien

$(OBJDIR)/%.o : %.c
	@echo "CC		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o : %.cc
	@echo "CXX		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) -c $(CXXFLAGS) -o $@ $<

$(OBJDIR)/_%.o : %.asm
	@echo "ASM		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(ASM) -f $(ASMOBJFORMAT) -o $@ $<

# --------------------------------------------------------------------------
# Gelinktes System

$(OBJDIR)/system: $(FIRST_OBJECT) $(OBJPRE)
	@echo "LD		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(CXX) $(CXXFLAGS) -static -e startup -T sections -o $(OBJDIR)/system $(FIRST_OBJECT) $(OBJPRE)

# --------------------------------------------------------------------------
# 'system.img' enthaelt die zu ladenden Sections des eigentlichen Systems

$(OBJDIR)/system.img : $(OBJDIR)/system
	@echo "OBJCOPY		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) objcopy -O binary $< $@

# --------------------------------------------------------------------------
# 'bootdisk.img' besteht aus dem 'system.img', das um den (parametrisierten)
# Bootblock und den Setup-Code erweitert wurde.

$(OBJDIR)/bootdisk.img : $(OBJDIR)/system.img $(TOOLS)/build $(BOOT)/bootsect $(BOOT)/setup
	@echo "BUILD		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(TOOLS)/build $(BOOT)/bootsect $(BOOT)/setup $< $(BOOTDEVICE) $@

# 'bootdisk.vmi' ist eine bootdisk, die auf 1.44 MB mit 0 aufgefuellt ist,
# damit VMware damit klarkommt. Des weiteren wird dieses Image benoetigt, um
# bootfaehige CDs zu erstellen.
# bootdisk-hd.img wird auf etwa 1M mit 0 aufgefuellt, da qemu diese Groesse
# auf eine Festplatten-Laufwerkscharakteristik zurueckrechnen kann.

$(OBJDIR)/bootdisk.vmi:  $(OBJDIR)/bootdisk.img
	dd if=$< of=$@ bs=1474560 conv=sync

$(OBJDIR)/bootdisk-hd.vmi:  $(OBJDIR)/bootdisk.img
	dd if=$< of=$@ bs=$$((2016*512)) conv=sync

# --------------------------------------------------------------------------
# 'clean' loescht das generierte System, die Objektdateien und die
# Abhaengigkeitsdateien

clean:	
	@echo "RM		$(OBJDIR)"
	$(VERBOSE) rm -rf $(OBJDIR)
	@echo "RM		$(DEPDIR)"
	$(VERBOSE) rm -rf $(DEPDIR)
	$(MAKE) -C $(BOOT) clean

# --------------------------------------------------------------------------
# 'bootdisk' erzeugt zunaechst das System, falls das noch nicht geschehen ist.
# Danach wird eine Bootdiskette mit dem System erzeugt.

bootdisk: $(OBJDIR)/bootdisk.img
	@echo "CP		$<"
	$(VERBOSE) cp $< $(DRIVE_FD)

# --------------------------------------------------------------------------
# 'bootdisk-hd' erzeugt zunaechst das System, falls das noch nicht geschehen
# ist. Danach wird das System auf das spezifizierte Laufwerk geschrieben,
# welches sowohl eine Festplatte, als auch ein USB-Stick sein kann.
# ACHTUNG: ein falsch angegebenes Laufwerk kann dazu fuehren, dass Daten auf dem
# spezifizierten Laufwerk verloren gehen! Nicht mit root-Rechten ausfuehren!

bootdisk-usb: bootdisk-hd
bootdisk-hd: $(OBJDIR)/bootdisk.img
	@echo "CP		$<"
	$(VERBOSE) cp $< $(DRIVE_HD)

# --------------------------------------------------------------------------
# 'qemu' ruft den qemu-Emulator mit dem System auf.

qemu: $(OBJDIR)/bootdisk.vmi
	qemu-system-i386 -fda $(OBJDIR)/bootdisk.vmi -boot a -k en-us -soundhw pcspk -vga std

# --------------------------------------------------------------------------
# 'qemu-gdb' ruft den qemu-Emulator mit aktiviertem GDB-Stub mit dem System
# auf, sodass es per GDB oder DDD inspiziert werden kann.

qemu-gdb: $(OBJDIR)/bootdisk.vmi
	$(VERBOSE) echo "break main" > /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "target remote 127.0.0.1:1234" >> /tmp/gdbcommands.$(shell id -u)
	$(VERBOSE) echo "continue" >> /tmp/gdbcommands.$(shell id -u)
	qemu-system-i386  -fda $(OBJDIR)/bootdisk.vmi -boot a -k en-us -s -S -soundhw pcspk -vga std

# --------------------------------------------------------------------------
# 'gdb' startet den GDB-Debugger und verbindet sich mit dem GDB-Stub des vorher
# gestarteten Qemu.

gdb:
	gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

# --------------------------------------------------------------------------
# 'ddd' startet den DDD-Debugger und verbindet sich mit dem GDB-Stub des vorher
# gestarteten Qemu.

ddd:
	ddd --gdb -x /tmp/gdbcommands.$(shell id -u) $(OBJDIR)/system

# --------------------------------------------------------------------------
# 'build'-Tool kompilieren.

$(TOOLS)/build: $(TOOLS)/build.c
	cd $(TOOLS) && $(CC) -o $@ $<

# --------------------------------------------------------------------------
# Bootsektor und Protected-Mode-Setup-Code kompilieren.

$(BOOT)/bootsect $(BOOT)/setup: $(BOOT)/bootsect.asm $(BOOT)/setup.asm
	$(MAKE) -C $(BOOT)

# --------------------------------------------------------------------------
# Einbindung der Abhaengigkeitsdateien

ifneq ($(MAKECMDGOALS),clean)
-include $(DEP_FILES)
endif

.PHONY: clean bootdisk bootdisk-hd bootdisk-usb gdb ddd
