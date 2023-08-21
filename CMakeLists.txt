cmake_minimum_required(VERSION 3.13)

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

# The _HAS_AUTO_PTR_ETC is here because grit version of lua-rapidjson uses ancient functions which arent supported from C++17, so we reenable them with this
add_definitions(
	-D_HAS_AUTO_PTR_ETC
	-DMAKE_LIB
	-DLUA_COMPILED_AS_HPP
	-DGLM_FORCE_SSE2
	-DLUA_RAPIDJSON_SANITIZE_KEYS
	-DLUA_RAPIDJSON_ALLOCATOR
	-DLUA_MSGPACK_COMPAT
)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/deps
	# This is for FiveM lua libs
	${PROJECT_SOURCE_DIR}/deps/lua
	${PROJECT_SOURCE_DIR}/deps/rapidjson/include
	${PROJECT_SOURCE_DIR}/deps/msgpack-c/include

	# CMake is weird
	${PROJECT_BINARY_DIR}/deps/msgpack-c/include
	${PROJECT_BINARY_DIR}/deps/msgpack-c/include/msgpack
)

set(LUA_SANDBOX ON)
set(GLM_FORCE_ALIGNED_GENTYPES ON)
set(MSGPACK_BUILD_EXAMPLES OFF)

# Enable this only if you have ZLIB installed
set(USE_ZLIB OFF)

# Enable this only if you have OpenSSL installed
# Enables using HTTPS in PerformHttpRequest
set(USE_TLS OFF)
set(USE_OPEN_SSL OFF)

add_subdirectory(deps/lua)
add_subdirectory(deps/msgpack-c)
add_subdirectory(deps/IXWebSocket)

add_library(${PROJECT_MODULE_NAME} SHARED ${PROJECT_SOURCE_FILES})
add_library(lua-rapidjson STATIC ${LUA_RAPIDJSON_FILES})
add_library(lua-cmsgpack STATIC ${LUA_CMSGPACK_FILES})

add_dependencies(${PROJECT_MODULE_NAME} alt-sdk)

target_link_libraries(${PROJECT_MODULE_NAME} PRIVATE liblua_static lua-rapidjson msgpack-c lua-cmsgpack ixwebsocket)