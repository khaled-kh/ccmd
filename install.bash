#/bin/bash
# This script install ccmd, you need root privilages
# Run: sudo ./install.bash

# 1. create empty .ccmd file at home directory

echo '' > ~/.ccmd ;

# 2. compile ccmd.c using gcc compiler

gcc ccmd.c -o ccmd ;

# 3. (root) copy ccmd (binary) to /bin/

sudo cp ccmd /bin/ ;

# 4. (root) install needed scripts to function

sudo ccmd -i ;

# 5. print app info and help

ccmd -h ;
