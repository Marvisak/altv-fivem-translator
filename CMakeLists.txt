cmake_minimum_required(VERSION 3.8)

include(deps/cpp-sdk/CMakeLists.txt)

project(fivem-translator)

set(PROJECT_MODULE_NAME fivem-translator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PROJECT_SOURCE_FILES ./src/*.h ./src/*.cpp)
# We are not using the supplied CMakeLists.txt by FiveM lua libs because it compiles the library as a module instead of shared or static library
# This can be solved by changing the type of library from MODULE to STATIC, but this would require forking the repo and changing things and I am too lazy for that
# So we will just compile it ourselves
file(GLOB_RECURSE LUA_RAPIDJSON_FILES ./deps/lua-rapidjson/src/*.hpp ./deps/lua-rapidjson/src/*.cpp)
file(GLOB_RECURSE LUA_CMSGPACK_FILES ./deps/lua-cmsgpack/src/*.h ./deps/lua-cmsgpack/src/*.c)

set_source_files_properties(./deps/lua-cmsgpack/src/lua_cmsgpack.c PROPERTIES LANGUAGE CXX)

add_definitions(
	-D_HAS_AUTO_PTR_ETC
	-DMAKE_LIB
	-DLUA_SANBODX
	-DGLM_FORCE_DEFAULT_ALIGNED_GENTYPES
	-DLUA_GLM_INCLUDE_ALL
	-DLUA_COMPILED_AS_HPP
	-DGLM_FORCE_SSE2
	-DLUA_RAPIDJSON_SANITIZE_KEYS
	-DLUA_RAPIDJSON_ALLOCATOR
	-DLUA_MSGPACK_COMPAT
	-DGLM_ENABLE_EXPERIMENTAL
)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/deps
	# This is for FiveM lua libs
	${PROJECT_SOURCE_DIR}/deps/lua
	${PROJECT_SOURCE_DIR}/deps/rapidjson/include
	${PROJECT_SOURCE_DIR}/deps/msgpack-c/include
)

add_subdirectory(deps/lua)
add_subdirectory(deps/msgpack-c)

add_library(${PROJECT_MODULE_NAME} SHARED ${PROJECT_SOURCE_FILES})
add_library(lua-rapidjson STATIC ${LUA_RAPIDJSON_FILES})
add_library(lua-cmsgpack STATIC ${LUA_CMSGPACK_FILES})

add_dependencies(${PROJECT_MODULE_NAME} alt-sdk)

target_link_libraries(${PROJECT_MODULE_NAME} PRIVATE liblua_static lua-rapidjson msgpackc-static lua-cmsgpack)