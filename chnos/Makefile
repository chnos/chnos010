
#OBJS_BOOTPACK = naskfunc.obj hankaku.obj gdtidt.obj graphic.obj grap_08.obj grap_16.obj grap_32.obj init.obj intrpt.obj io.obj memory.obj paging.obj sheet.obj fifo.obj mtask.obj timer.obj keyboard.obj mouse.obj window.obj console.obj file.obj uuid.obj api.obj bootpack.obj
OBJS_BOOTPACK = nasfunc0.obj nasfunc1.obj cfunc.obj dsctbl.obj emu86.obj emu86asm.obj error.obj intrpt.obj keyboard.obj memory.obj serial.obj timer.obj vgatmode.obj xception.obj bootpack.obj
TOOLPATH = ../../z_tools/
INCPATH  = ../../z_tools/CHNOSProject/

MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
CC1      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2CHN  = $(TOOLPATH)bim2chn.exe
RULEFILE = $(INCPATH)CHNOSP.rul
EDIMG    = $(TOOLPATH)edimg.exe bias:9
IMGTOL   = $(TOOLPATH)RWFD.EXE
MAKEFONT = $(TOOLPATH)makefont.exe
COPY     = copy
DEL      = del
SJISCONV = $(TOOLPATH)sjisconv.exe -s

# デフォルト動作

default :
	$(MAKE) img

# ファイル生成規則

chnipl.bin : chnipl.nas Makefile
	$(NASK) chnipl.nas chnipl.bin chnipl.lst

hankaku.bin : hankaku.txt Makefile
	$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj : hankaku.bin Makefile
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

asmhead.bin : asmhead.nas Makefile
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

bootpack.gas : bootpack.c Makefile
	$(CC1) -o bootpack.gas bootpack.c

bootpack.nas : bootpack.gas Makefile
	$(GAS2NASK) bootpack.gas bootpack.nas

bootpack.obj : bootpack.nas Makefile
	$(NASK) bootpack.nas bootpack.obj bootpack.lst

nasfunc0.obj : nasfunc0.nas Makefile
	$(NASK) nasfunc0.nas nasfunc0.obj nasfunc0.lst

bootpack.bim : $(OBJS_BOOTPACK) Makefile
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)
# 3MB+64KB=3136KB

bootpack.chn : bootpack.bim Makefile
	$(BIM2CHN) bootpack.bim bootpack.chn 0

chnos.sys : asmhead.bin bootpack.chn Makefile
	copy /B asmhead.bin+bootpack.chn chnos.sys

chnos.img : chnipl.bin chnos.sys Makefile
	$(EDIMG)   imgin:../../z_tools/fdimg0at.tek \
		wbinimg src:chnipl.bin len:512 from:0 to:0 \
		copy from:chnos.sys to:@: \
		copy from:chnipl.nas to:@: \
		imgout:chnos.img

#一般規則

%.ca : %.c Makefile
	$(SJISCONV) $*.c $*.ca

%.gas : %.ca core.h Makefile
	$(CC1) -o $*.gas $*.ca

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst



# コマンド

img :
	$(MAKE) chnos.img

run :
	$(MAKE) img
	$(COPY) chnos.img ..\..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../../z_tools/qemu
run_b :
	$(MAKE) img
	$(COPY) chnos.img ..\..\z_tools\bochs\fdimage0.bin
	$(MAKE) -C ../../z_tools/bochs


run_cd :
	$(MAKE) iso
	$(MAKE) -C ../../z_tools/qemu_iso

install :
	$(MAKE) img
	$(IMGTOL) chnos.img a:

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.chn
	-$(DEL) chnos.sys

src_only :
	$(MAKE) clean
	-$(DEL) chnos.img

iso :
	$(MAKE)
	..\..\z_tools\mkisofs.exe -v -iso-level 1 -b chnos.img -o ..\..\z_tools\qemu_iso\chnos.iso .

