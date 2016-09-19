echo "running $1..."
DIR=stats
BC=stats/$1.bc
LS=stats/$1.ls.bc

fname=$DIR/$1
echo $BC
echo $LS

rm -f llvmprof.out result.l* 

clang -emit-llvm -o stats/$1.bc -c $1/src/$1.c

opt -loop-simplify < $BC > $LS

opt -insert-edge-profiling $fname.ls.bc -o $fname.profile.ls.bc

llc $fname.profile.ls.bc -o $fname.profile.ls.s

g++ -o $fname.profile $fname.profile.ls.s /opt/llvm/Release+Asserts/lib/libprofile_rt.so

./$fname.profile $2

opt -load $LLVM_DIR/lib583HW1.so -lamp-insts -insert-lamp-profiling -insert-lamp-loop-profiling -insert-lamp-init < $fname.ls.bc > $fname.lamp.bc

llc < $fname.lamp.bc > $fname.lamp.s

g++ -o $fname.lamp.exe $fname.lamp.s $HW1_PASS/tools/lamp-profiler/lamp_hooks.o

echo "profiling $1 with $2"
./stats/$1.lamp.exe $2 > /dev/null

echo "loading prof data $1"
echo "======================================================="
opt -load $LLVM_DIR/lib583HW1.so -lamp-inst-cnt -lamp-map-loop -lamp-load-profile -profile-loader -profile-info-file=llvmprof.out -p2 < $fname.ls.bc > /dev/null
