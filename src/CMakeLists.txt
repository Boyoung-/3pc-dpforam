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

SET(CMAKE_C_FLAGS "-Wall -O3 -maes")
SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -fkeep-inline-functions")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

FILE(GLOB libdpf
  "libdpf/*.h"
  "libdpf/*.cpp"
  )
FILE(GLOB src
  "*.h"
  "*.cpp"
  )
  
macro (add_test _name)
	add_executable(${_name} "test/${_name}.cpp" ${libdpf} ${src})
	target_link_libraries(${_name} crypto cryptopp) 
endmacro()

add_test(fss_test)
add_test(proto_test)