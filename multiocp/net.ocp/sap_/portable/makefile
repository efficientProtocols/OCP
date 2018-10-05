#
#  Description: 
#
#
# RCS: $Id: makefile,v 1.2 1995/12/28 07:06:19 mohsen Exp $
#
# :::::::::::::
#
# The following are expected to be over written by mkp,
# Defaults are:
SHELL = /bin/sh
MAKE = make	
CC = cc
AS = as -
LB = ar r
RANLIB = ranlib
LK = cc
ETAGS = etags
#
# Default destinations:
BASE = /usr/public
CFLAGS = -I. -I${BASE}/include
LIBS_PATH = ${BASE}/libc
BIN_PATH = ${BASE}/bin
MAN_PATH = ${BASE}/man
DATA_PATH = ${BASE}/lib
#
# Product Specific Stuff
#
PDT = $(BIN_PATH)/
PKG= $(LIBS_PATH)/gf.a
PUB_H =
LCL_H =
LCL_C =
C_SRC =  ${PUB_H} ${LCL_H} ${LCL_C}
OBJS = sap.o sapcmp.o sapget.o
MAN = 
INFO = 
#
GF = ${LIBS_PATH}/gf.a
LIBS = # ${GF}

#
#$(PDT):       $(OBJS) ${LIBS}
#	${LK} -o ${PDT} ${OBJS} ${LIBS} 
#
$(PKG):       $(OBJS)
	${LB} ${PKG} ${OBJS}
	${RANLIB} ${PKG}
#
all:	$(PROG1) $(PROG2) $(PROG3)

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
e_libs:
	@echo ${LIBS}
e_pdt:
	@echo ${PDT}
e_man:
	@echo ${MAN}
e_info:
	@echo ${INFO}
#
TAGS:  ${LCL_C}
	${ETAGS} ${LCL_C}

