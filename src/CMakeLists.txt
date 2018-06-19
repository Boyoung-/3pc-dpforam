cmake_minimum_required(VERSION 3.5)

PROJECT (DPFORAM)

#[[
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
]]

FILE(GLOB libdpf
  "libdpf/*.h"
  "libdpf/*.cpp"
  )
FILE(GLOB src
  "*.h"
  "*.cpp"
  )

ADD_EXECUTABLE(libdpf_test ${libdpf} test/libdpf_test.cpp)
TARGET_COMPILE_OPTIONS(libdpf_test PUBLIC -g -Wall -fkeep-inline-functions -maes)
TARGET_LINK_LIBRARIES(libdpf_test crypto)

ADD_EXECUTABLE(fss_test ${libdpf} fss.h fss.cpp util.h util.cpp test/fss_test.cpp)
TARGET_COMPILE_OPTIONS(fss_test PUBLIC -g -Wall -fkeep-inline-functions -maes)
TARGET_LINK_LIBRARIES(fss_test crypto)

ADD_EXECUTABLE(dpforam ${src} ${libdpf})
TARGET_COMPILE_OPTIONS(dpforam PUBLIC -g -Wall -fkeep-inline-functions -maes)
TARGET_LINK_LIBRARIES(dpforam cryptopp crypto)