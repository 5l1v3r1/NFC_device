#!/bin/bash
#gentagdb database build script
echo "cscope -q -R -b"
cscope -q -R -b
echo "ctags -L cscope.files"
ctags -L cscope.files

