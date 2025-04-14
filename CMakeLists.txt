cmake_minimum_required(VERSION 3.15)
project(ParallelSortLab)

set(CMAKE_CXX_STANDARD 17)

#Компілятор LLVM (з підтримкою OpenMP)
set(CMAKE_C_COMPILER "/opt/homebrew/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")

#Прапори для OpenMP
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/opt/homebrew/opt/libomp/lib -lomp")

#Прапори для MPI
find_package(MPI REQUIRED)

#OpenMP таргет
add_executable(openmp main.cpp)
target_link_libraries(openmp)

#MPI таргет
add_executable(mpi main_mpi.cpp)
target_link_libraries(mpi MPI::MPI_CXX)
