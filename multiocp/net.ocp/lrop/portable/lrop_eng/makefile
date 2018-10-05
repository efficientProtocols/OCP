#  Copyright (C) 1995,1996  AT&T Wireless Services, Inc. All rights reserved.
#  (C) Copyright Neda Communications, Inc 1995. All rights reserved.
# 
#  All Rights Reserved.
# 
#  This software is furnished under a license and use, duplication,
#  disclosure and all other uses are restricted to the rights specified
#  in the written license between the licensee and copyright holders.
#
#  File name: makefile
#
#  Description: LROP Module (LSROS Engine)
#
# RCS: $Id: makefile,v 1.23 1996/12/07 00:09:55 kamran Exp $
#
# :::::::::::::
SHELL=/bin/sh
LB=ar r
CFLAGS = 
LIBS_PATH = 
RANLIB = echo


PKG= $(LIBS_PATH)/lsroprot.a
PUB_H= 
LCL_H= 
LCL_C= invoke.c lrop_cfg.c layernm.c  \
	pduin.c pduout.c pdupr.c tmr_if.c \
  	clinvktd.c clperftd.c invokact.c perfact.c lrop_sap.c \
  	clinvtd2.c clpertd2.c invact2.c perfact2.c \
	userin.c userout.c keep.c lowerind.c  conffunc.c
C_SRC=  $(PUB_H) $(LCL_H) $(LCL_C)
#
OBJS= invoke.o lrop_cfg.o layernm.o \
	pduin.o pduout.o pdupr.o tmr_if.o \
	clinvktd.o clperftd.o invokact.o perfact.o lrop_sap.o \
  	clinvtd2.o clpertd2.o invact2.o perfact2.o \
	userin.o userout.o keep.o lowerind.o  conffunc.o
#
MAN= 

#
$(PKG):       $(OBJS)
	${LB} ${PKG} ${OBJS}
	${RANLIB} ${PKG}
#

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
e_pkg:
	@echo ${PKG}
e_man:
	@echo ${MAN}
#
ctags:  ${LCL_C}
	ctags ${LCL_C}


