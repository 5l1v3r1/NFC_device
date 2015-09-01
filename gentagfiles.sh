
    #!/bin/bash
    #gentagfiles file list build script
    set -x
    find | grep "\.c$\|\.cpp$\|\.h$" | grep "\.\/build" >> cscope.files
    find | grep "\.c$\|\.cpp$\|\.h$" | grep "\.\/main" >> cscope.files
    find | grep "\.c$\|\.cpp$\|\.h$" | grep "\.\/MyLib" >> cscope.files
    sort cscope.files > cscope.files.sorted
    mv cscope.files.sorted cscope.files

