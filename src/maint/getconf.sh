#!/bin/bash

#"Who built, where, with what, what CVS versions, which options"
gc_logfile="$1"
gc_repository="$2"
gc_CC="$3"
gc_CF="$4"
gc_CX="$5"
gc_CXF="$6"
gc_LD="$7"

gc_date=`date +%F-%Hh%M`
gc_line="++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
gc_intro="+++++++ Logging Mogo game generation $gc_date ++++++++"

function log(){
    echo "$*" >> $gc_logfile
}
log $gc_line
log $gc_intro

# Check for last created game
gc_lastone=$(ls -alrt $gc_repository/mogo* | tail -n 1)
gc_owner=$(echo $gc_lastone | awk '{print $3}')
gc_date=$(echo $gc_lastone | awk '{print $6$7$8}')
gc_size=$(echo $gc_lastone | awk '{print $5}')
gc_location=$(echo $gc_lastone | awk '{print $9}')
gc_name=$(basename $gc_location)
gc_md5sum=$(md5sum $gc_location | awk '{print $1}')
gc_machine=`hostname`
log "gogame : $gc_name"
log "built  : $gc_owner"
log "machn  : $gc_machine"
log "date   : $gc_date"
log "locate : $gc_location"
log "size   : $gc_size"
log "md5sum : $gc_md5sum"

# Check if CVS available
gc_cvsurl=$(cat $(find CVS -name "Root" | head -n1) | cut -d@ -f2 | cut -d: -f1)
ping -W1 -c1 $gc_cvsurl &> /dev/null
if [ $? -eq 0 ]; then
	# do CVS work
    st=$(cvs status -v createMakefile.sh | grep "Sticky Tag" | awk '{print $3'})
    if [ "$st" == "(none)" ]; then
	st="working on the head"
	wr=$(cvs status -v createMakefile.sh | grep "Working revision" | awk '{print $3'})
	log "cvs    : tag.$st rev.$wr"
    else
	log "cvs    : tag.$st"
    fi    
else
    log "[warning] couldn't reach CVS repository, skipped this part"
fi

log "++++++++++"

# Check for compilation values
log "CC      : $gc_CC"
log "CFLAGS  : $gc_CF" 
log "CXX     : $gc_CX"
log "CXXFGS  : $gc_CXF"
log "LDFLAGS : $gc_LD"

log $gc_line
log ""
