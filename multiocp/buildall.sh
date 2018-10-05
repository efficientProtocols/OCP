#!/bin/ksh

#
# RCS Revision: $Id: buildall.sh,v 1.8 1998/02/26 06:03:59 mohsen Exp $
#

. ${CURENVBASE}/tools/ocp-lib.sh

. ${CURENVBASE}/tools/buildallhead.sh


###### DO NOT EDIT ABOVE THIS LINE ######

HERE=`pwd`

TM_trace 9 ${MKP}

buildAndRecord ${HERE}/base.ocp "${buildAllCmd}"
buildAndRecord ${HERE}/net.ocp "${buildAllCmd}"
buildAndRecord ${HERE}/sysif.ocp "${buildAllCmd}"
buildAndRecord ${HERE}/this "${buildAllCmd}"
buildAndRecord ${HERE}/skeleton "${buildAllCmd}"

exit 0

