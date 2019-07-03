#!/bin/bash 


cat > mogob.sh <<EOF
export MOGOBULL_ROOT="\$PWD"
export MOGOBULL_HOME="\$MOGOBULL_ROOT/distrib"
export PATH="\$MOGOBULL_HOME/bin:\$PATH"
export PATH="\$MOGOBULL_HOME/scripts:\$PATH"
export PATH="\$MOGOBULL_HOME/engines:\$PATH"
EOF

cat > mogob.module <<EOF
#%Module1.0#####################################################################
##
set     MOGOBULL_ROOT	   "Place-your-dir-here"
set 	MOGOBULL_HOME	   \$MOGOBULL_ROOT/distrib
module-verbosity {on}

prepend-path    PATH            \$MOGOBULL_HOME/bin
prepend-path    PATH            \$MOGOBULL_HOME/engines
prepend-path    PATH            \$MOGOBULL_HOME/scripts
EOF
