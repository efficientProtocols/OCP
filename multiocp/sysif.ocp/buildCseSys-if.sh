#!/bin/csh -f
# $Id: buildCseSys-if.sh,v 1.1 1995/11/28 07:41:17 mohsen Exp $

#set verbose

set CURENVSRC="${CURENVBASE}/../src"

set MKP="${CURENVBASE}/bin/curenvmkp -k"
set EXIT="echo"

#set MKP="${CURENVBASE}/bin/curenvmkp"
#set EXIT="exit"

alias make gmake

echo Building ${CURENVSRC}/cse/sys-if
#
make -C ${CURENVSRC}/cse/sys-if/sf -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/sys-if/os -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/sys-if/sch -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/sys-if/tmr -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status

${EXIT} 0
