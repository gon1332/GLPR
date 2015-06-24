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
./1_text_isolation/txtiso.out $1 > /dev/null 2>&1
./2_character_segmentation/charsegm.out final.png > /dev/null 2>&1
rm final.png
for i in letter*.png;
do
    if [ "$i" \< "letter04.png" ]
    then
        tesseract $i outbase -l ell -psm 10 > /dev/null 2>&1
    else
        tesseract $i outbase -l eng -psm 6 > /dev/null 2>&1
    fi
    head outbase.txt >> output.txt
    rm $i
done

cp output.txt tmp.txt
cat /dev/null > output.txt
cat tmp.txt | perl -p -e 's/[^Α-Ω0-9]+//g' >> output.txt
head output.txt

rm outbase.txt
rm tmp.txt
exit $SUCCESS
