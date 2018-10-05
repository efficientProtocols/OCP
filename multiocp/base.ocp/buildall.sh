#!/bin/ksh

#
# RCS Revision: $Id: buildall.sh,v 1.1 1998/02/26 06:04:09 mohsen Exp $
#

. ${CURENVBASE}/tools/ocp-lib.sh

. ${CURENVBASE}/tools/buildallhead.sh


###### DO NOT EDIT ABOVE THIS LINE ######


HERE=`pwd`

TM_trace 9 ${MKP}

#
# First Build everything that is common and portable
#


buildAndRecord ${HERE}/pf_/portable "${MKP}"
buildAndRecord ${HERE}/getopt/portable "${MKP}"
buildAndRecord ${HERE}/alloc/hosted "${MKP}"
buildAndRecord ${HERE}/buf/hosted "${MKP}"
buildAndRecord ${HERE}/cfg/hosted "${MKP}"
buildAndRecord ${HERE}/eh_/unix "${MKP}"
buildAndRecord ${HERE}/pn_/unix "${MKP}"
buildAndRecord ${HERE}/profile/hosted "${MKP}"
buildAndRecord ${HERE}/qu_/portable "${MKP}"
buildAndRecord ${HERE}/seq_/hosted "${MKP}"
buildAndRecord ${HERE}/str/hosted "${MKP}"
#buildAndRecord ${HERE}/tm_/hosted "${MKP}"
buildAndRecord ${HERE}/log_/ontopof "${MKP}"
buildAndRecord ${HERE}/bs_/portable "${MKP}"
buildAndRecord ${HERE}/fsm_/portable "${MKP}"
buildAndRecord ${HERE}/lic_/hosted "${MKP}"
buildAndRecord ${HERE}/vsprintf/portable "${MKP}"
buildAndRecord ${HERE}/sbm_/hosted "${MKP}"

#
# Now do all the Flavor specific work
#

#
# Do all the Flavor specific work
#

for i in ${mkpFlavor}
do
  case ${i} in
    'ocp+sol2+gcc.sh')
      buildAndRecord ${HERE}/tm_/hosted "${MKP}"
      ;;
    'ocp+dos+bc45.sh')
      ;;
    'win32.sh')
      buildAndRecord ${HERE}/tm_/wce "${MKP}"
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
