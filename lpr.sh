#!/bin/sh

EXPECTED_ARGS=1
E_BADARGS=1
SUCCESS=0

if [ $# -ne $EXPECTED_ARGS ]
then
    echo "Usage: ./lpr <input.img>"
    exit $E_BADARGS
fi

rm -f output.txt
./0_locateLP/locateLP $1 > /dev/null 2>&1
for i in "polyContour*.png"; do
	./1_text_isolation/txtiso.out $i > /dev/null 2>&1
	./2_character_segmentation/charsegm.out final.png > /dev/null 2>&1
	rm final.png
	./3_ocr/ocr.out letter*.png
	rm letter*.png
done

echo "\n\033[0;31m$(tput bold)TODO\033[0m:$(tput sgr0) 0_locateLP produces possibly many output files."
echo "Need to verify that code $(tput bold)WILL$(tput sgr0) work for many iterations."
echo "\033[1;33m$(tput bold)This is a warning!$(tput sgr0)\033[0m"
echo "\033[1;37m$(tput bold)Exiting now!$(tput sgr0)\033[0m"

echo "-------------------------------"
echo "Greek License Plate: "
cat output.txt | grep '[Α-Ω0-9]' | perl -p -e 's/[^Α-Ω0-9]+//g'
printf "\n"
echo "-------------------------------"

rm output.txt

exit $SUCCESS
