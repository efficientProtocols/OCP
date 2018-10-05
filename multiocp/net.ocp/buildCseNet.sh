#!/bin/csh -f
# $Id: buildCseNet.sh,v 1.1 1995/11/28 07:41:15 mohsen Exp $

#set verbose

set CURENVSRC="${CURENVBASE}/../src"

#set MKP="${CURENVBASE}/bin/curenvmkp -k"
#set EXIT="echo"

set MKP="${CURENVBASE}/bin/curenvmkp"
set EXIT="exit"

alias make gmake

echo Building ${CURENVSRC}/cse/net
#
make -C ${CURENVSRC}/cse/net/bo -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/net/du -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/net/inet -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/net/sap -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/net/upq_bsd -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/net/udp_if -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/net/asn -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status
make -C ${CURENVSRC}/cse/net/lrop_eng -f ${CURENVSRC}/mkp.make mkp MKP=${MKP}
if ($status != 0) ${EXIT} $status

${EXIT} 0

