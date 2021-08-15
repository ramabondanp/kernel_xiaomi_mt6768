  #!/bin/bash
  mkdir ${PWD}/clang-13
   ! [[ -f ${PWD}/clang-r383902b1.tar.gz ]] && wget https://android.googlesource.com/platform/prebuilts/clang/host/linux-x86/+archive/refs/heads/android11-qpr3-release/clang-r383902b1.tar.gz -P ${PWD}
  tar -C ${PWD}/clang-13/ -zxvf ${PWD}/clang-r383902b1.tar.gz

