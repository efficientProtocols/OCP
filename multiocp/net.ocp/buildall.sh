#!/bin/ksh

#
# RCS Revision: $Id: buildall.sh,v 1.1 1998/02/26 06:04:34 mohsen Exp $
#

. ${CURENVBASE}/tools/ocp-lib.sh

. ${CURENVBASE}/tools/buildallhead.sh


###### DO NOT EDIT ABOVE THIS LINE ######


HERE=`pwd`

TM_trace 9 ${MKP}

#
# First Build everything that is common and portable
#

buildAndRecord ${HERE}/asn "${MKP}"
buildAndRecord ${HERE}/bo_/portable "${MKP}"
buildAndRecord ${HERE}/du_/unhosted "${MKP}"
buildAndRecord ${HERE}/inet/unix "${MKP}"
buildAndRecord ${HERE}/sap_/portable "${MKP}"
buildAndRecord ${HERE}/udp_pco "${MKP}"
#buildAndRecord ${HERE}/upq/sol2/upq_bsd "${MKP}"

exit 0
