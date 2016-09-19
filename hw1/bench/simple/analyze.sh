clang -emit-llvm -o simple.bc -c src/simple.c
opt -insert-edge-profiling simple.bc -o simple.profile.bc
llc simple.profile.bc -o simple.profile.s
g++ -o simple.profile simple.profile.s /opt/llvm/Release+Asserts/lib/libprofile_rt.so
./simple.profile
