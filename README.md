Installation
------------
```
cd path-to-project/
cmake .
make
chmod +x scripts/*
```

Clean Project
-------------
`make clean`: Removes executables.
`clean_cmake_files.sh`: Removes all produced cmake files.

LPR
-------
`lpr.sh`: License plate recognition.

Usage:
```
./scripts/lpr.sh input/images/car4.jpg
```

Petarlinux Project
-----------------
Compile for the Petalinux Project:
```
./scripts/arm-xcompile.sh

```
