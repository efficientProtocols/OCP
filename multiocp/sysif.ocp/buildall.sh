#!/bin/ksh

#
# RCS Revision: $Id: buildall.sh,v 1.2 1999/09/19 18:37:15 mohsen Exp $
#

. ${CURENVBASE}/tools/ocp-lib.sh

. ${CURENVBASE}/tools/buildallhead.sh


###### DO NOT EDIT ABOVE THIS LINE ######

HERE=`pwd`

TM_trace 9 ${MKP}


#
# First Build everything that is common and portable
#

#
# Do all the Flavor specific work
#

for i in ${mkpFlavor}
do
  case ${i} in
    'ocp+sol2+gcc.sh')
      #
      buildAndRecord ${HERE}/sf_/unix "${MKP}"
      buildAndRecord ${HERE}/os/sol2  "${MKP}"
      buildAndRecord ${HERE}/sch_/unix "${MKP}"
      buildAndRecord ${HERE}/tmr_/unix.djl "${MKP}"
      buildAndRecord ${HERE}/outstrm_/outfile "${MKP}"
      buildAndRecord ${HERE}/nvm_/unix "${MKP}"
      ;;
    'ocp+dos+bc45.sh')
      ;;
    'win32.sh')
      buildAndRecord ${HERE}/sf_/unix "${MKP}"
      buildAndRecord ${HERE}/os/wce-1.0  "${MKP}"
      buildAndRecord ${HERE}/sch_/unix "${MKP}"
      buildAndRecord ${HERE}/tmr_/wce-1.0 "${MKP}"
      buildAndRecord ${HERE}/outstrm_/outfile "${MKP}"
      buildAndRecord ${HERE}/nvm_/unix "${MKP}"
      ;;
    'ocp+ae+bc45.sh')   
      ;;
    'purify')
      echo "NOTYET"
      exit
      ;;
    'os-winnt.sh'|'os-wince.sh'|'os-win95.sh')
      ;;
    'cpu-x86.sh'|'cpu-mips.sh'|'cpu-sh3.sh')
      ;;
    'pdt-uadevl.sh')
      ;;
    *)
      EH_oops
      exit 1
      ;;
  esac
done

exit 0

