# 
#  This software is furnished under a license and use, duplication,
#  disclosure and all other uses are restricted to the rights specified
#  in the written license between the licensee and copyright holders.
#
#
#  Description: 
#	
#
#
# RCS: $Id: makefile,v 1.1.1.1 1998/01/30 01:13:13 mohsen Exp $
#
# :::::::::::::


PDT = all
PUB_H =
LCL_H =
LCL_C = 
C_SRC = ${PUB_H} ${LCL_H} ${LCL_C}
SRC = \
	sysinit.c	\
	mpdiag.c  \
	main.c

OBJS = \
	sysinit.obj	\
	mpdiag.obj  \
	main.obj



C_SRC = ${PUB_H} ${LCL_H} ${LCL_C}
#
GF = $(LIBS_PATH)/gf.lib
RELID = $(LIBS_PATH)/relid.a
SF = $(LIBS_PATH)/sf.a
#CPR = $(LIBS_PATH)/cpr.a
LIBS =   $(RELID)  $(GF) aehw.lib #${SF}


#
# SYSLIBS: supplied by the make invoker
#


$(PDT):	simple1.axe 

#
simple1.rom:       $(OBJS) ${LIBS}
	 ${LK} -o simple1.rom ${OBJS} ${LIBS} ${SYSLIBS}

simple1.axe:       simple1.rom
         ${LOCATE} -f SRAM simple1

simple1.hex:       simple1.rom
	 ${LOCATE} -f FLASH simple1

e_pub_h:
	@echo ${PUB_H}
e_lcl_h:
	@echo ${LCL_H}
e_lcl_c:
	@echo ${LCL_C}
e_c_src:
	@echo ${C_SRC}
e_objs:
	@echo ${OBJS}
e_libs:
	@echo ${LIBS}
e_pdt:
	@echo ${PDT}

tags:  ${LCL_C}
	ctags $(LCL_C)

