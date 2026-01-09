cmake_minimum_required(VERSION 3.20)
project(translator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Statikus linkelés bekapcsolása (hogy ne kelljenek a MinGW DLL-ek)
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")

add_executable(translator translator.cpp)

# CTranslate2 headers
target_include_directories(translator PRIVATE
        E:/CodeBlocks/modellLLM/ct2/CTranslate2/CTranslate2/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Linkelés
target_link_libraries(translator PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/libctranslate2.dll.a
        ${CMAKE_CURRENT_SOURCE_DIR}/libsentencepiece.dll.a
)

# DLL-ek másolása a build mappába
file(GLOB DLL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.dll")
foreach(DLL_FILE ${DLL_FILES})
    add_custom_command(TARGET translator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${DLL_FILE}"
        $<TARGET_FILE_DIR:translator>
    )
endforeach()
