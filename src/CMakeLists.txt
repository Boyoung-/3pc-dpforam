cmake_minimum_required(VERSION 3.5)

project(DPFORAM)

find_package(OpenSSL REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_C_FLAGS "-Wall -O3 -maes")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -fkeep-inline-functions")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

file(GLOB libdpf
	"libdpf/*.h"
	"libdpf/*.cpp"
)
file(GLOB src
	"*.h"
	"*.cpp"
)
  
macro(add_test _name)
	add_executable(${_name} "test/${_name}.cpp" ${libdpf} ${src})
	target_link_libraries(${_name} crypto cryptopp) 
endmacro()

add_test(fss_test)
add_test(proto_test)