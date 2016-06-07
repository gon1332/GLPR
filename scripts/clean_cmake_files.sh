#!/bin/sh

EXPECTED_ARGS=1
E_BADARGS=1
SUCCESS=0

if [ $# -ne $EXPECTED_ARGS ]
then
   echo "Usage: $0 <path-to-project>"
   exit $E_BADARGS
fi

# 0_locate_lp
rm -f $1/0_locate_lp/Makefile
rm -f $1/0_locate_lp/cmake_install.cmake
rm -f $1/0_locate_lp/CMakeCache.txt
rm -rf $1/0_locate_lp/CMakeFiles

# 1_text_isolation
rm -f $1/1_text_isolation/Makefile
rm -f $1/1_text_isolation/cmake_install.cmake
rm -f $1/1_text_isolation/CMakeCache.txt
rm -rf $1/1_text_isolation/CMakeFiles

# 2_morphology_normalization
rm -f $1/2_morphology_normalization/Makefile
rm -f $1/2_morphology_normalization/cmake_install.cmake
rm -f $1/2_morphology_normalization/CMakeCache.txt
rm -rf $1/2_morphology_normalization/CMakeFiles

# 3_character_segmentation
rm -f $1/3_character_segmentation/Makefile
rm -f $1/3_character_segmentation/cmake_install.cmake
rm -f $1/3_character_segmentation/CMakeCache.txt
rm -rf $1/3_character_segmentation/CMakeFiles

# 4_ocr
rm -f $1/4_ocr/Makefile
rm -f $1/4_ocr/cmake_install.cmake
rm -f $1/4_ocr/CMakeCache.txt
rm -rf $1/4_ocr/CMakeFiles

# LPR
rm -f $1/Makefile
rm -f $1/cmake_install.cmake
rm -f $1/CMakeCache.txt
rm -rf $1/CMakeFiles

exit $SUCCESS
