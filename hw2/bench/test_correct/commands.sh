## Remove the output files
rm llvmprof.out
rm -rf *bc *profile* *ll *.dot *.png *.*.png *.slicm*

# NAME=correct1
NAME=main
if [ ! -z "$1" ]; then
  echo "$1 is arg"
  NAME=$1
fi

## Get the simple profile data
clang -emit-llvm -o $NAME.bc -c $NAME.c
opt -dot-cfg < $NAME.bc > $NAME.dot >& /dev/null
dot -Tpng -o $NAME.png cfg.main.dot

opt -loop-simplify < $NAME.bc > $NAME.ls.bc
echo "loop simplify done...!"

# edge insert
opt -insert-edge-profiling $NAME.ls.bc -o $NAME.profile.ls.bc
llc $NAME.profile.ls.bc -o $NAME.profile.ls.s
g++ -o $NAME.profile $NAME.profile.ls.s $LLVM_OPT/Release+Asserts/lib/libprofile_rt.so
./$NAME.profile
echo "edge insertion done..!"

## Get Lamp profile
# opt -load $LLVM_DIR/lib583HW2.so -lamp-insts -insert-lamp-profiling -insert-lamp-loop-profiling -insert-lamp-init < $NAME.ls.bc > $NAME.lamp.bc 
# llc < $NAME.lamp.bc > $NAME.lamp.s
# g++ -o $NAME.lamp.exe $NAME.lamp.s $HW2_PASS/tools/lamp-profiler/lamp_hooks.o
# ./$NAME.lamp.exe

## Run your pass
opt -basicaa -load $LLVM_DIR/lib583HW2.so -profile-loader -profile-info-file=llvmprof.out -slicm < $NAME.ls.bc > $NAME.slicm.bc
# opt -basicaa -load $LLVM_DIR/lib583HW2.so -lamp-inst-cnt -lamp-map-loop -lamp-load-profile -profile-loader -profile-info-file=llvmprof.out -slicm < $NAME.ls.bc > $NAME.slicm.bc

## Compare original and modified IR
llvm-dis $NAME.ls.bc
llvm-dis $NAME.slicm.bc

## Generate executables and ensure that your modified IR generates correct output
llc $NAME.bc -o $NAME.s
g++ -o $NAME $NAME.s
echo "right answer is] "
./$NAME
echo "]========================"
llc $NAME.slicm.bc -o $NAME.slicm.s
g++ -o $NAME.slicm $NAME.slicm.s
./$NAME.slicm

# diff $NAME.png $NAME.slicm.png 

## Once you are sure that your pass works then use mem2reg pass to convert some intermediate load stores to register transfers
opt -basicaa -load $LLVM_DIR/lib583HW2.so -slicm -mem2reg < $NAME.ls.bc > $NAME.slicm.bc

echo "===========done compiling...==========="
opt -dot-cfg < $NAME.slicm.bc >& /dev/null
dot -Tpng -o $NAME.slicm.png cfg.main.dot

llc $NAME.slicm.bc -o $NAME.slicm.s
g++ -o $NAME.slicm $NAME.slicm.s
./$NAME.slicm


rm $NAME
