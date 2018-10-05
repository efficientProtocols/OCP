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
# RCS: $Id: makefile,v 1.1.1.1 1997/10/21 18:24:54 mohsen Exp $
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
OBJS= notice.o nedatt.o neda.o att.o sierra.o
#
MAN= 

#
$(PKG):       $(OBJS)
	${LB}  ${PKG} ${OBJS}
	${RANLIB} ${PKG}
#
GF = $(LIBS_PATH)/gf.a
#SF = $(LIBS_PATH)/sf.a
#LIBS =   $(GF) $(SF) $(GF)
LIBS =   $(GF) 

# If everything worked the way it should under NT/Win95
#This is all we needed to do:

#notice.c:		notice_p.c
#	${CC} ${CFLAGS} -DSHOW_CRC -c notice_p.c -o notice_p.o
#	${LK} -o notice_p notice_p.o $(LIBS)
#	sed -e "s/XXX_NOTICE_LEN_XXX/`notice_p -l`/" \
#	    -e "s/XXX_NOTICE_CRC_XXX/`notice_p -c`/" \
#	    notice_p.c > notice.c
#	rm notice_p
#	echo "Generated notice.c"

notice.c:		notice_p.c CCnotice_p
	${LK} -o notice_p notice_p.o $(LIBS)
	${SHELL} sedit.sh notice_p > notice.c
	echo "Generated notice.c"

CCnotice_p:
	${MAKE} CFLAGS="${CFLAGS} -DSHOW_CRC" CC=${CC} notice_p.o

neda.c:		neda_p.c CCneda_p
	${LK} -o neda_p neda_p.o $(LIBS)
	${SHELL} sedit.sh neda_p > neda.c
	echo "Generated neda.c"

CCneda_p:
	${MAKE} CFLAGS="${CFLAGS} -DSHOW_CRC" CC=${CC} neda_p.o

nedatt.c:		nedatt_p.c CCnedatt_p
	${LK} -o nedatt_p nedatt_p.o $(LIBS)
	${SHELL} ./sedit.sh nedatt_p > nedatt.c
	echo "Generated nedatt.c"

CCnedatt_p:
	${MAKE} CFLAGS="${CFLAGS} -DSHOW_CRC" CC=${CC} nedatt_p.o


att.c:		att_p.c CCatt_p
	${LK} -o att_p att_p.o $(LIBS)
	${SHELL} ./sedit.sh att_p > att.c
	echo "Generated att.c"

CCatt_p:
	${MAKE} CFLAGS="${CFLAGS} -DSHOW_CRC" CC=${CC} att_p.o


sierra.c:		sierra_p.c CCsierra_p
	${LK} -o sierra_p sierra_p.o $(LIBS)
	${SHELL} ./sedit.sh sierra_p > sierra.c
	echo "Generated sierra.c"

CCsierra_p:
	${MAKE} CFLAGS="${CFLAGS} -DSHOW_CRC" CC=${CC} sierra_p.o

clean:
	-rm nedatt.c neda.c att.c sierra.c notice.c nedatt_p neda_p att_p sierra_p notice_p

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


