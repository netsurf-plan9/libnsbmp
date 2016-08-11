#!/bin/sh 

TEST_PATH=$1
TEST_OUT=${TEST_PATH}/ppm

mkdir -p ${TEST_OUT}

bmpdecode()
{
    OUTF=$(basename ${1} .bmp)
    ${TEST_PATH}/test_decode_bmp ${1} > ${TEST_OUT}/${OUTF}.ppm
}

icodecode()
{
    OUTF=$(basename ${1} .ico)
    ${TEST_PATH}/test_decode_ico ${1} > ${TEST_OUT}/${OUTF}.ppm
}

# standard bitmap suite
for BMP in $(ls test/bmpsuite/*.bmp);do
    bmpdecode ${BMP}
done

# test bitmaps
for ICO in $(ls test/bmp/*.bmp);do
    bmpdecode ${ICO}
done

# test icons
for ICO in $(ls test/icons/*.ico);do
    icodecode ${ICO}
done
