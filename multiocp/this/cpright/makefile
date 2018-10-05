#
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
#
# RCS: $Id: makefile,v 1.3 1998/02/26 06:09:23 mohsen Exp $
#
# :::::::::::::
SHELL=/bin/sh
LB=ar r
CFLAGS = 
LIBS_PATH = 
RANLIB = echo


PKG= $(LIBS_PATH)/relid.a

PUB_H= 
LCL_H= 
LCL_C= notice.c nedatt.c neda.c att.c sierra.c
C_SRC=  $(PUB_H) $(LCL_H) $(LCL_C)
#
OBJS= notice.o neda.o  nedatt.o att.o # sierra.o
#
MAN= 

#
$(PKG):       $(OBJS)
	${LB}  ${PKG} ${OBJS}
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


