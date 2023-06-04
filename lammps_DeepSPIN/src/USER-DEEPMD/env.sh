DEEPMD_ROOT=/data/home/scv6266/run/WORK/hzt/DeepSpin_final/deepmd_root
TENSORFLOW_INCLUDE_DIRS="/data/home/scv6266/run/test/tensorflow_root/include;/data/home/scv6266/run/test/tensorflow_root/include"
TENSORFLOW_LIBRARY_PATH="/data/home/scv6266/run/test/tensorflow_root/lib"

TF_INCLUDE_DIRS=`echo $TENSORFLOW_INCLUDE_DIRS | sed "s/;/ -I/g"`
TF_LIBRARY_PATH=`echo $TENSORFLOW_LIBRARY_PATH | sed "s/;/ -L/g"`
TF_RPATH=`echo $TENSORFLOW_LIBRARY_PATH | sed "s/;/ -Wl,-rpath=/g"`

NNP_INC=" -std=c++11 -DHIGH_PREC -DLAMMPS_VERSION_NUMBER=$(./lmp_version.sh) -I$DEEPMD_ROOT/include/ "
NNP_PATH=" -L$TF_LIBRARY_PATH -L$DEEPMD_ROOT/lib"
NNP_LIB=" -Wl,--no-as-needed -ldeepmd_cc -ltensorflow_cc -ltensorflow_framework -Wl,-rpath=$TF_RPATH -Wl,-rpath=$DEEPMD_ROOT/lib"
