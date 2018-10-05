#!/bin/csh -f
# $Id: buildCseFoundation.sh,v 1.1 1995/11/28 07:41:13 mohsen Exp $

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
