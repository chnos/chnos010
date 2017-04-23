# ÉâÉCÉuÉâÉäê∂ê¨ópMakefile

TARGET   = chnlibc.lib
OBJS     = startup.obj

TOOLPATH = ../../../z_tools/
INCPATH  = ../../include/

SJISCONV = $(TOOLPATH)sjisconv -s
CC1      = $(TOOLPATH)gocc1 -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask -a
NASK     = $(TOOLPATH)nask
GOLIB    = $(TOOLPATH)golib00
MAKE     = $(TOOLPATH)make -r
DELE     = del

ALL :
	$(MAKE) $(TARGET)

%.ca : %.c Makefile
	$(SJISCONV) $*.c $*.ca

%.gas : %.ca Makefile
	$(CC1) -o $*.gas $*.ca

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj

$(TARGET) : $(OBJS) Makefile
	$(GOLIB) out:$(TARGET) $(OBJS)

clean :
	-$(DELE) *.obj

src_only :
	$(MAKE) clean
	-$(DELE) $(TARGET)
