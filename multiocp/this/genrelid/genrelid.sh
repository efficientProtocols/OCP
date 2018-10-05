#!/bin/ksh
#
# RCS: $Id: genrelid.sh,v 1.3 1998/02/03 08:28:21 mohsen Exp $
#
# Syntax:
# Description:
#
#

#
# Customize the following on a per-product basis
# Default Values are set below.
# 

RELID_RELINFO_FILE=${KSHCURENVBASE}/relnotes/moduleName.tex
RELID_TEMPLATE_FILE=./relid.tpl
STATUS_STRING="Beta: derivative work based on sources which were released on"
#STATUS_STRING="released on"
outFile=relid_p.c

alias TM_trace='echo "TM_ file=$0 lineNo=$LINENO: "'
alias LOG_message='echo'
alias EH_oops='echo "EH_ OOPS file=$0 lineNo=$LINENO: "'
alias EH_problem='echo "EH_ PROBLEM file=$0 lineNo=$LINENO: "'

toFrontSlash() {
  echo $1 | sed -e "s@\\\\@/@g"
}


set -- `getopt  o:t:r:s: $*`

if [ $? -ne 0 ]
then 
    print >&2 "Usage: $0 [-t relidTemplateFile] [-r relInfoFile] [-s statusString]"
    print >&2 "statusString: is text for either derivative work or final release"
    exit
fi

for i in $* 
do
    case $i in
    -t)   RELID_TEMPLATE_FILE=$2; shift 2;;
    -r)   RELID_RELINFO_FILE=$2; shift 2;;
    -s)   STATUS_STRING=$2; shift 2;;
    -o)   outFile=$2; shift 2;;
    --)   shift; break;;
    esac
done


systemName=`uname -n`
osType=`uname -s`


if [ "${osType}" = "SunOS" ]
then
  LOG_message "RELID_RELINFO_FILE is set to: $RELID_RELINFO_FILE"
fi


#
# grovel around for the release information
#

RELID_PROD_NAME=`grep RELID-INFO-Product-Name $RELID_RELINFO_FILE | cut -d: -f2`
RELID_REVNUM=`grep RELID-INFO-Rev-Number $RELID_RELINFO_FILE | cut -d$ -f2 | cut -d' ' -f 2`
#RELID_REVNUM="0.${RELID_REVNUM}"
RELID_REL_DATE=`grep RELID-INFO-Rel-Date $RELID_RELINFO_FILE | cut -d$ -f2 | cut -d' ' -f 2,3`
RELID_CVS_TAG=`grep RELID-INFO-CVS-Tag $RELID_RELINFO_FILE | cut -d$ -f2 | tr -s ' ' | cut -d' ' -f 2`
RELID_BUILD_USER=${USER}
RELID_BUILD_HOST="`uname -n` `uname -svrm`"
RELID_BUILD_DATE=`date | tr -s ' '`

if [ "${RELID_PROD_NAME}" = "" ]
then
    RELID_PROD_NAME="product unspecified"
fi

if [ "${RELID_REVNUM}" = "" ]
then
    RELID_REVNUM="revision unspecified"
fi

if [ "${RELID_REL_DATE}" = "" ]
then
    RELID_REL_DATE="release date unspecified"
fi


if [ "${RELID_CVS_TAG}" = "" ]
then
    RELID_CVS_TAG="CVS tag unspecified"
fi

if [ "${osType}" != "SunOS" ]
then
  CURENVBASE=`toFrontSlash ${KSHCURENVBASE}`
fi


LOG_message "   RELID-INFO-Product-Name=${RELID_PROD_NAME}"
LOG_message "   RELID-INFO-Rev-Number=${RELID_REVNUM}"
LOG_message "   RELID-INFO-Rel-Date=${RELID_REL_DATE}"
LOG_message "   RELID-INFO-CVS-Tag=${RELID_CVS_TAG}"
LOG_message "   RELID-INFO-BUILD-DATE=${RELID_BUILD_DATE}"
LOG_message "   RELID-INFO-BUILD-USER=${RELID_BUILD_USER}"
LOG_message "   RELID-INFO-BUILD-HOST=${RELID_BUILD_HOST}"
LOG_message "   RELID-INFO-BUILD-DIR=${CURENVBASE}"

#
# 

RELID_NOTICE="${RELID_PROD_NAME} ${RELID_REVNUM} (${RELID_CVS_TAG}) \\\\n\
${STATUS_STRING} ${RELID_REL_DATE}.\\\\n\
Built on ${RELID_BUILD_DATE} by ${RELID_BUILD_USER} on ${RELID_BUILD_HOST} \\\\n\
in the ${CURENVBASE} directory.\\\\n"

sed -e "s@%RELID_NOTICE%@${RELID_NOTICE}@" ${RELID_TEMPLATE_FILE} > ${outFile}

#
# if the sed seperator shows up in the RELID_NOTICE
# escape it like so
#
#RELID_REL_DATE=`echo $RELID_REL_DATE | sed -e "s/\//\\\\\\\\\//g"`
#sed -e "s/%RELID_NOTICE%/${RELID_NOTICE}/" ${RELID_TEMPLATE_FILE}  > ${outFile}
