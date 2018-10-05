#!/bin/ksh

#
# RCS Revision: $Id: buildall.sh,v 1.1 1998/02/26 06:05:17 mohsen Exp $
#

. ${CURENVBASE}/tools/ocp-lib.sh

. ${CURENVBASE}/tools/buildallhead.sh


###### DO NOT EDIT ABOVE THIS LINE ######


HERE=`pwd`

TM_trace 9 ${MKP}

#
# First Build everything that is common and portable
#

buildAndRecord ${HERE}/single.full.hosted "${MKP}"

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
      ;;
    'ocp+dos+bc45.sh')
      ;;
    'win32.sh')
      ;;
    'ocp+ae+bc45.sh')
      buildAndRecord ${HERE}/simple.ae "${MKP}"
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
