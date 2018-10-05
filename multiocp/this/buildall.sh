#!/bin/ksh

#
# RCS Revision: $Id: buildall.sh,v 1.1 1998/02/26 06:20:09 mohsen Exp $
#

. ${CURENVBASE}/tools/ocp-lib.sh

. ${CURENVBASE}/tools/buildallhead.sh


###### DO NOT EDIT ABOVE THIS LINE ######



HERE=`pwd`

TM_trace 9 ${MKP}

#
# First Build everything that is common and portable
#

buildAndRecord ${HERE}/genrelid "${MKP}"
buildAndRecord ${HERE}/cpright "${MKP}"
buildAndRecord ${HERE}/relidusr "${MKP}"

exit 0

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
      cd ${CURENVSRC}/base.ocp/alloc/hosted; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/buf/hosted; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/cfg/hosted; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/eh_/unix; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/pn_/unix; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/profile/hosted; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/qu_/portable; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/seq_/hosted; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/str/hosted; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/tm_/hosted; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/log_/ontopof; echo `pwd`; ${MKP}
      cd ${CURENVSRC}/base.ocp/bs_/portable; echo `pwd`; ${MKP}

      #
      cd  ${CURENVSRC}/sysif.ocp/sf_/unix; echo `pwd`; ${MKP}
      cd  ${CURENVSRC}/sysif.ocp/os/sol2  ; echo `pwd`; ${MKP}
      cd  ${CURENVSRC}/sysif.ocp/sch_/unix; echo `pwd`; ${MKP}
      cd  ${CURENVSRC}/sysif.ocp/tmr_/unix; echo `pwd`; ${MKP}
      cd  ${CURENVSRC}/sysif.ocp/outstrm_/outfile; echo `pwd`; ${MKP}

      cd ${CURENVSRC}/this/relidusr; echo `pwd`; ${MKP}

      cd ${CURENVSRC}/skeleton/single.full.hosted; echo `pwd`; ${MKP}

      ;;
    'ocp+dos+bc45.sh')
      cd ${CURENVSRC}/this/relidusr; echo `pwd`; ${MKP}
      ;;
    'win32.sh')
      cd ${CURENVSRC}/this/relidusr; echo `pwd`; ${MKP}
      ;;
    'ocp+ae+bc45.sh')   
      cd ${CURENVSRC}/skeleton/simple.ae; echo `pwd`; ${MKP}
      ;;
    'purify')
      echo "NOTYET"
      exit
      ;;
    'os-winnt.sh'|'os-wince.sh'|'os-win95.sh')
      ;;
    'cpu-x86.sh'|'cpu-mips.sh'|'cpu-sh3.sh')
      ;;
    *)
      EH_oops
      exit 1
      ;;
  esac
done

exit 0



#!/bin/csh -f
# $Id: buildall.sh,v 1.1 1998/02/26 06:20:09 mohsen Exp $

#set verbose

set CURENVSRC="${CURENVBASE}/../src"

set MKP="${CURENVBASE}/bin/curenvmkp -k"
set EXIT="echo"

#set MKP="${CURENVBASE}/bin/curenvmkp"
#set EXIT="exit"

alias make gmake


echo Building ${CURENVSRC}/cse/foundation
#
make -C ${CURENVSRC}/cse/foundation/alloc -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/buf -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/cfg -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/eh -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/pf -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/pn -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/profile -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/qu -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/seq -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/str -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/tm -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/foundation/bs -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status


${EXIT} 0
