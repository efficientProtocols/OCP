#
#  (C) Copyright Neda Communications, Inc 1995. All rights reserved.
# 
#  All Rights Reserved.
# 
#  This software is furnished under a license and use, duplication,
#  disclosure and all other uses are restricted to the rights specified
#  in the written license between the licensee and copyright holders.
#
#
#  File name: makefile
#
#  Description: LSROS Provider
#
# RCS  Revision: $Id: makefile,v 1.1.1.1 1998/02/01 00:47:19 mohsen Exp $
#
# :::::::::::::

PDT = all
#PDT1 = $(BIN_PATH)/relidusr
PDT1 = single.full
PDT2 = $(BIN_PATH)/oldrelid
PUB_H =
LCL_H = 
LCL_C = main.c
C_SRC =  ${PUB_H} ${LCL_H} ${LCL_C}
OBJS  = main.o 

GF = $(LIBS_PATH)/gf.a
RELID = $(LIBS_PATH)/relid.a
SF = $(LIBS_PATH)/sf.a
#CPR = $(LIBS_PATH)/cpr.a
LIBS =   $(RELID)  $(GF) ${SF}

#

#$(PDT):   $(PDT1) $(PDT2)
$(PDT):   $(PDT1)

$(PDT1):       $(OBJS) 
	 ${LK} -o ${PDT1} ${OBJS} ${LIBS}

$(PDT2):       oldmain.o ${LIBS}
	 ${LK} -o ${PDT2} oldmain.o ${LIBS}
#

PFILE=pfile.exe


DOWNLOAD : 
	$(PFILE) COPY ${PDT1}  "$(PFILE_DEST)WCE:\windows\mb${PDT1}"

RUN :
	$(PFILE) RUN  "$(PFILE_DEST)WCE:\windows\mb${PDT1}"

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
#

