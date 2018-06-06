cmake_minimum_required(VERSION 3.5)

PROJECT (DPFORAM)

FILE(GLOB libdpf
  "libdpf/*.h"
  "libdpf/*.cpp"
  )
FILE(GLOB src
  "*.h"
  "*.cpp"
  )

ADD_EXECUTABLE(libdpf_test ${libdpf} test/libdpf_test.cpp)
TARGET_COMPILE_OPTIONS(libdpf_test PUBLIC -fkeep-inline-functions -maes)
TARGET_LINK_LIBRARIES(libdpf_test crypto)

ADD_EXECUTABLE(fss_test ${libdpf} fss.h fss.cpp util.h util.cpp test/fss_test.cpp)
TARGET_COMPILE_OPTIONS(fss_test PUBLIC -g -Wall -fkeep-inline-functions -maes)
TARGET_LINK_LIBRARIES(fss_test crypto)

ADD_EXECUTABLE(dpforam ${src} ${libdpf})
TARGET_COMPILE_OPTIONS(dpforam PUBLIC -g -Wall -fkeep-inline-functions -maes)
TARGET_LINK_LIBRARIES(dpforam cryptopp crypto)