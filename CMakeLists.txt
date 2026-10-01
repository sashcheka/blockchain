cmake_minimum_required(VERSION 3.16)
project(blockchain_lab VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenSSL 1.1.1 REQUIRED COMPONENTS Crypto)

function(enable_project_warnings target)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:Clang,GNU>:-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow>
        $<$<CXX_COMPILER_ID:MSVC>:/W4;/permissive->
    )
endfunction()

add_library(blockchain_core
    src/block.cpp
    src/blockchain.cpp
    src/hash.cpp
    src/transaction.cpp
)
target_include_directories(blockchain_core PUBLIC include)
target_link_libraries(blockchain_core PUBLIC OpenSSL::Crypto)
enable_project_warnings(blockchain_core)
