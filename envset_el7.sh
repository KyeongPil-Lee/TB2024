#! /bin/bash

# When using CVMFS
source /cvmfs/sft.cern.ch/lcg/views/LCG_102/x86_64-centos7-gcc11-opt/setup.sh

export INSTALL_DIR_PATH=$PWD/install

export PATH=$PATH:$INSTALL_DIR_PATH/lib64
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_DIR_PATH/lib64
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$INSTALL_DIR_PATH/lib64
export PYTHONPATH=$PYTHONPATH:$INSTALL_DIR_PATH/lib64
export YAMLPATH=/cvmfs/sft.cern.ch/lcg/releases/yamlcpp/0.6.3-d05b2/x86_64-centos7-gcc11-opt/lib

# -- for postprocess codes
export TB2024PPPATH=$PWD/analysis/postProcess
export ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:$TB2024PPPATH