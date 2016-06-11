#!/bin/sh

EXPECTED_ARGS=1
E_BADARGS=1
SUCCESS=0

mkdir -p /tmp/glpr-output
rm -f /tmp/glpr-output/*

if [ $# -ne $EXPECTED_ARGS ]
then
   echo "Usage: $0 <input.img>"
   exit $E_BADARGS
fi

#./video_cap/video.out $1
./0_locate_lp/loclp.out $1
./1_text_isolation/txtiso.out /tmp/glpr-output/contour0.jpg
./2_morphology_normalization/morphology.out /tmp/glpr-output/cleanIm.jpg
#./3_character_segmentation/charsegm.out /tmp/glpr-output/pic1.jpg
./4_ocr/ocr.out /tmp/glpr-output/pic1.jpg

# echo "-------------------------------"
# echo "Greek License Plate: "
# cat /tmp/glpr-output/output.txt | grep '[A-Z0-9]' | perl -p -e 's/[^A-Z0-9]+//g'
# printf "\n"
# echo "-------------------------------"

exit $SUCCESS
