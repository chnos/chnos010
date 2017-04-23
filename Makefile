
#アプリ追加時に書き足すべき箇所	
# ファイル生成規則 2箇所
# コマンド
#  full, clan_full, src_only_full

TARGET   = chnos

TOOLPATH = ../z_tools/
INCPATH  = ../z_tools/CHNOSProject/
SRCROOT  = ./

include $(SRCROOT)tooldef.txt

# デフォルト動作

default :
	$(MAKE) full
	$(MAKE) chnos.img

# ファイル生成規則

chnos.img : chnos/chnipl.bin chnos/chnos.sys chnos/chnipl.nas \
		asmtest/asmtest.chn \
		bug1/bug1.chn \
		crack1/crack1.chn \
		crack2/crack2.chn \
		crack3/crack3.chn \
		crack4/crack4.chn \
		ctest/ctest.chn
	$(EDIMG)   imgin:../z_tools/fdimg0at.tek \
		wbinimg src:chnos/chnipl.bin len:512 from:0 to:0 \
		copy from:chnos/chnos.sys to:@: \
		copy from:chnos/chnipl.nas to:@: \
		copy from:asmtest/asmtest.chn to:@: \
		copy from:bug1/bug1.chn to:@: \
		copy from:crack1/crack1.chn to:@: \
		copy from:crack2/crack2.chn to:@: \
		copy from:crack3/crack3.chn to:@: \
		copy from:crack4/crack4.chn to:@: \
		copy from:ctest/ctest.chn to:@: \
		imgout:chnos.img

# コマンド
run : $(TARGET).img
	$(COPY) $(TARGET).img $(TOOLPATH)qemu/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)qemu

brun : $(TARGET).img
	$(COPY) $(TARGET).img $(TOOLPATH)bochs/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)bochs

install : $(TARGET).img
	$(COPY) $(TARGET).img $(TOOLPATH)fdwrite/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)fdwrite

iso : $(TARGET).img
	$(COPY) $(TARGET).img $(TOOLPATH)makeiso/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)makeiso
	$(COPY) $(TOOLPATH)makeiso/$(TARGET).iso $(TARGET).iso

full :
	$(MAKE) -C chnos
	$(MAKE) -C asmtest
	$(MAKE) -C bug1
	$(MAKE) -C crack1
	$(MAKE) -C crack2
	$(MAKE) -C crack3
	$(MAKE) -C crack4
	$(MAKE) -C ctest
	$(MAKE) chnos.img

run_full :
	$(MAKE) full
	$(COPY) chnos.img ..\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../z_tools/qemu

install_full :
	$(MAKE) full
	$(IMGTOL) chnos.img a:

run_os :
	$(MAKE) -C chnos
	$(MAKE) run

clean :
# 何もしない

src_only :
	$(MAKE) clean
	-$(DEL) chnos.img

clean_full :
	$(MAKE) -C asmtest		clean
	$(MAKE) -C bug1			clean
	$(MAKE) -C crack1		clean
	$(MAKE) -C crack2		clean
	$(MAKE) -C crack3		clean
	$(MAKE) -C crack4		clean
	$(MAKE) -C ctest		clean
	$(MAKE) -C chnos		clean

src_only_full :
	$(MAKE) -C asmtest		src_only
	$(MAKE) -C bug1			src_only
	$(MAKE) -C crack1		src_only
	$(MAKE) -C crack2		src_only
	$(MAKE) -C crack3		src_only
	$(MAKE) -C crack4		src_only
	$(MAKE) -C ctest		src_only
	$(MAKE) -C chnos		src_only
	-$(DEL) chnos.img

refresh :
	$(MAKE) full
	$(MAKE) clean_full
	-$(DEL) chnos.img
