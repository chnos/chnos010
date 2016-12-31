include makeset.txt

TARGET   = chnos

SRCROOT  = ../
TOOLPATH = $(SRCROOT)../z_tools/
INCPATH  = $(SRCROOT)../z_tools/CHNOSProject/

include $(SRCROOT)tooldef.txt

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

bootpack.gas : bootpack.ca $(HEADS_BOOTPACK) Makefile
	$(CC1) -o bootpack.gas bootpack.ca

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
	$(HARITOL) concat chnos.sys asmhead.bin bootpack.chn

chnos.img : chnipl.bin chnos.sys Makefile
	$(EDIMG)   imgin:../../z_tools/fdimg0at.tek \
		wbinimg src:chnipl.bin len:512 from:0 to:0 \
		copy from:chnos.sys to:@: \
		copy from:chnipl.nas to:@: \
		copy from:chnlogo.bmp to:@: \
		imgout:chnos.img

#一般規則

%.ca : %.c Makefile
	$(SJISCONV) $*.c $*.ca

%.gas : %.ca $(HEADS_BOOTPACK) Makefile
	$(CC1) -o $*.gas $*.ca

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

# コマンド

img :
	$(MAKE) chnos.img

run :
	$(MAKE) $(TARGET).img
	$(COPY) $(TARGET).img $(TOOLPATH)qemu/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)qemu

brun :
	$(MAKE) $(TARGET).img
	$(COPY) $(TARGET).img $(TOOLPATH)bochs/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)bochs

install :
	$(MAKE) $(TARGET).img
	$(COPY) $(TARGET).img $(TOOLPATH)fdwrite/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)fdwrite

iso :
	$(MAKE) $(TARGET).img
	$(COPY) $(TARGET).img $(TOOLPATH)makeiso/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)makeiso
	$(COPY) $(TOOLPATH)makeiso/$(TARGET).iso $(TARGET).iso

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) *.ca
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.chn
	-$(DEL) chnos.sys

src_only :
	$(MAKE) clean
	-$(DEL) chnos.img

