# CMakeList.txt : CMake project for ECLTest3, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

get_property(c_known_features GLOBAL PROPERTY CMAKE_C_KNOWN_FEATURES)

project("ECLTest3" VERSION 0.1.0.0 LANGUAGES NONE)
enable_language(C)

set(CMAKE_C_STANDARD 23)

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -march=native")
#SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -mtune=native")
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} /std:c2x")
add_compile_definitions(__MMX__=1)
add_compile_definitions(__SSE__=1)
add_compile_definitions(__SSE2__=1)
add_compile_definitions(__SSE3__=1)
add_compile_definitions(__SSSE3__=1)
add_compile_definitions(__SSE4A__=1)
add_compile_definitions(__SSE4_1__=1)
add_compile_definitions(__SSE4_2__=1)
add_compile_definitions(__AVX__=1)
add_compile_definitions(__F16C__=1)
add_compile_definitions(__BMI__=1)
add_compile_definitions(__LZCNT__=1)
add_compile_definitions(__POPCNT__=1)
add_compile_definitions(__FMA__=1)
add_compile_definitions(__FMA4__=1)
add_compile_definitions(__XOP__=1)
add_compile_definitions(__TBM__=1)
add_compile_definitions(__LWP__=1)
add_compile_definitions(__PCLMUL__=1)
add_compile_definitions(__AES__=1)
add_compile_definitions(__FSGSBASE__=1)
add_compile_definitions(__FXSR__=1)
add_compile_definitions(__XSAVEOPT__=1)

set_source_files_properties(*.c PROPERTIES LANGUAGE C)

# Add source to this project's executable.
add_executable (ECLTest3 "ECLTest3.c" "ECLTest3.h"
                         "Tokenizer.c" "Tokenizer.h"
                         "ECL_util.h"
                         "SquitLib/SquitUtil.c" "SquitLib/SquitUtil.h"
                         "Mapping.c"
                         "Symbols.c" "Symbols.h"
                         "StringStream.c" "StringStream.h"
                         "ArgParsing.c" "ArgParsing.h"
                         "_RegisterWatch.h"
                         "SquitLib/SquitStrings.c" "SquitLib/SquitStrings.h"
                         "__TEST.c" "__TEST.h"   "SquitLib/SquitObjects.h" "SquitLib/SquitTypes.h" "SquitLib/SquitIO.h" "SquitLib/SquitIO.c")

set_target_properties (ECLTest3 PROPERTIES
  C_STANDARD 23
)

#target_compile_options(ECLTest3 PRIVATE /std:C2x )

# TODO: Add tests and install targets if needed.
