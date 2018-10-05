#
#  Description: 
#
#
# SCCS Revision: %W%    Released: %G%
#
# :::::::::::::
SHELL=/bin/sh
LB=ar r
CFLAGS = -I../include
LIBS_PATH = /usr/mohsen/libc
RANLIB = echo


PKG= $(LIBS_PATH)/lsroprot.a
PUB_H= 
LCL_H= 
LCL_C=  upper.c
C_SRC=  $(PUB_H) $(LCL_H) $(LCL_C)
#
OBJS=  upper.o
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
