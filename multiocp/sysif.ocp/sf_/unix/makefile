#
#  Description: 
#
#
# RCS  Revision: $Id: makefile,v 1.2 1995/11/27 10:19:32 mohsen Exp $
#
# :::::::::::::

LB= ar r
#RANLIB= ranlib
RANLIB= echo
PUB_H =
LCL_H = 
LCL_C = sf_qu.c
C_SRC =  ${PUB_H} ${LCL_H} ${LCL_C}
OBJS  = sf_qu.o
#
GF = $(LIBS_PATH)/gf.a
LIBS =  $(GF)
PKG= $(LIBS_PATH)/sf.a

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
e_libs:
	@echo ${LIBS}
e_pdt:
	@echo ${PDT}
e_ds:
	@echo ${DS}
#
tags:  ${LCL_C}
	ctags $(LCL_C)

