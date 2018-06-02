#!/bin/sh

export TARGET=arm-none-eabi
export PREFIX=$(pwd)/build_newlib_cyg

cd newlib-cygwin
make clean
./configure \
 --target=$TARGET \
 --prefix=$PREFIX \
 --with-cpu=cortex-m4 \
 --with-fpu=fpv4-sp-d16 \
 --with-float=hard \
 --with-mode=thumb \
 --enable-interwork \
 --enable-multilib \
 --with-gnu-as \
 --with-gnu-ld \
 --disable-nls \
 --disable-newlib-supplied-syscalls \
 --enable-newlib-nano-malloc \
 --enable-newlib-nano-formatted-io \
 --with-newlib \
 --disable-newlib-io-c99-formats \
 --enable-newlib-reent-small 

make -j4 all
make install

#https://istarc.wordpress.com/2014/07/21/stm32f4-build-your-toolchain-from-scratch/
