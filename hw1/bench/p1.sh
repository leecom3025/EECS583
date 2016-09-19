#!/usr/bin/env bash
# bash script for hw1 pass1
echo "running for $1..."
DIR=stats
BC=$DIR/$1.bc

echo "running $1..."
clang -emit-llvm -o $BC -c $1/src/$1.c
echo "clang $1..."
opt -insert-edge-profiling $BC -o $DIR/$1.profile.bc
echo "opt $1..."
llc stats/$1.profile.bc -o stats/$1.profile.s
echo "llc $1..."
g++ -o stats/$1.profile stats/$1.profile.s /opt/llvm/Release+Asserts/lib/libprofile_rt.so
echo "g++ $1..."
./stats/$1.profile $2 # > /dev/null
echo "profile $1..."
mv llvmprof.out stats/$1.prof.out
echo "opting $1..."

opt -f -load $LLVM_DIR/p1.so -profile-loader -profile-info-file=stats/$1.prof.out -p1 < stats/$1.bc > /dev/null
