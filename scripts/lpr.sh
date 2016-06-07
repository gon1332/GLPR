#!/bin/sh

EXPECTED_ARGS=1
E_BADARGS=1
SUCCESS=0

rm -f output/*

if [ $# -ne $EXPECTED_ARGS ]
then
   echo "Usage: $0 <input.img>"
   exit $E_BADARGS
fi

#./video_cap/video.out $1
./0_locate_lp/loclp.out $1
./1_text_isolation/txtiso.out output/contour0.jpg
./2_morphology_normalization/morphology.out output/cleanIm.jpg
#./3_character_segmentation/charsegm.out output/pic1.jpg
./4_ocr/ocr.out output/pic1.jpg

# echo "-------------------------------"
# echo "Greek License Plate: "
# cat output/output.txt | grep '[A-Z0-9]' | perl -p -e 's/[^A-Z0-9]+//g'
# printf "\n"
# echo "-------------------------------"

exit $SUCCESS
