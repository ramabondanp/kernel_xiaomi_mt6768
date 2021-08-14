  #!/bin/bash
  mkdir ${PWD}/clang-13
  mkdir ${PWD}/toolchain64
  mkdir ${PWD}/toolchain32
  ! [[ -f ${PWD}/clang-r428724.tar.gz ]] && wget https://android.googlesource.com/platform/prebuilts/clang/host/linux-x86/+archive/refs/heads/master/clang-r428724.tar.gz -P ${PWD}
  tar -C ${PWD}/clang-13/ -zxvf ${PWD}/clang-r428724.tar.gz

