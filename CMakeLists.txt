cmake_minimum_required(VERSION 2.8.7)
project(SPLONEBOX)

option(USE_COVERAGE "Enable gcov (coverage) support." OFF)
option(CLANG_ADDRESS_SANITIZER "Enable clang address sanitizer." OFF)
option(CLANG_MEMORY_SANITIZER "Enable clang memory sanitizer." OFF)
option(CLANG_THREAD_SANITIZER "Enable clang thread sanitizer." OFF)
option(CLANG_ANALYZER "Enable clang static analyzer." OFF)

# Prefer our bundled versions of dependencies.
set(DEPS_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/.deps/usr" CACHE PATH "Path prefix for finding dependencies")
if(CMAKE_CROSSCOMPILING AND NOT UNIX)
  list(INSERT CMAKE_FIND_ROOT_PATH 0 ${DEPS_PREFIX})
  list(INSERT CMAKE_PREFIX_PATH 0 ${DEPS_PREFIX}/../host/bin)
else()
  list(INSERT CMAKE_PREFIX_PATH 0 ${DEPS_PREFIX})
  set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${DEPS_PREFIX}/lib/pkgconfig")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "CMAKE_BUILD_TYPE not given; setting to 'Debug'.")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
  SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
      Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wconversion")

add_definitions(-Wall -Wextra -pedantic -Wstrict-prototypes -std=gnu99
    -Wvariadic-macros -Wcast-align -Wshadow
    -Wmissing-field-initializers -Wmissing-format-attribute -Wfloat-equal
    -Wundef -Wpointer-arith -Wstrict-overflow=5 -Wswitch-default -Wswitch-enum
    -Wunreachable-code -Wformat-security)

set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g" CACHE STRING
    "Compiler flags for release builds with debug info." FORCE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(DEBUG 1)
else()
  set(DEBUG 0)
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang" AND CMAKE_GENERATOR MATCHES "Ninja" AND
    NOT CLANG_ANALYZER)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fcolor-diagnostics")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined")
  set(CMAKE_SHARED_LINKER_FLAGS
      "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
  set(CMAKE_MODULE_LINKER_FLAGS
      "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(BSD REQUIRED)
endif()
find_package(LIBUV REQUIRED)
find_package(MSGPACK REQUIRED)
find_package(HIREDIS REQUIRED)
find_package(CMOCKA REQUIRED)

include(CheckLibraryExists)

if((CLANG_ADDRESS_SANITIZER AND CLANG_MEMORY_SANITIZER)
    OR (CLANG_ADDRESS_SANITIZER AND CLANG_THREAD_SANITIZER)
    OR (CLANG_MEMORY_SANITIZER AND CLANG_THREAD_SANITIZER))
  message(FATAL_ERROR "Sanitizers cannot be enabled simultaneously.")
endif()

if(CLANG_ANALYZER)
  # Note that the default location of ccc-analyzer on Archlinux is used. this
  # may be different in other environments.
  set(CMAKE_C_COMPILER /usr/lib/clang/ccc-analyzer)
endif()

if((CLANG_ADDRESS_SANITIZER OR CLANG_MEMORY_SANITIZER OR CLANG_THREAD_SANITIZER)
    AND NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "Sanitizers are only supported for clang.")
endif()

if(CLANG_ADDRESS_SANITIZER)
  message(STATUS "Enabling clang address sanitizer and undefined behavior sanitizer.")
elseif(CLANG_MEMORY_SANITIZER)
  message(STATUS "Enabling Clang memory sanitizer.")
elseif(CLANG_MEMORY_SANITIZER)
  message(STATUS "Enabling Clang thread sanitizer.")
endif()

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

file(GLOB MANPAGES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  man/splonebox.1)

# splonebox target sources
set(SPLONEBOX-SOURCES
  src/main.c
  src/main.h
  src/sb-common.h
  src/khash.h
  src/klist.h
  src/kvec.h
  src/queue.h
  src/string.c
  src/reallocarray.c
  src/hashmap.c
  src/random.c
  src/optparser.c
  src/optparser.h
  src/signal.c
  src/filesystem.c
  src/devurandom.c
  src/tweetnacl.h
  src/tweetnacl.c
  src/options.c
  src/options.h
  src/confparse.c
  src/confparse.h
  src/parse.c
  src/parse.h
  src/util.c
  src/util.h
  src/address.c
  src/address.h
  src/sbmemzero.c
  src/api/sb-api.h
  src/api/register.c
  src/api/result.c
  src/api/run.c
  src/api/broadcast.c
  src/api/subscribe.c
  src/api/unsubscribe.c
  src/api/helpers.c
  src/api/helpers.h
  src/rpc/sb-rpc.h
  src/rpc/connection/event.c
  src/rpc/connection/event.h
  src/rpc/connection/event-defs.h
  src/rpc/connection/streamhandle.c
  src/rpc/connection/streamhandle.h
  src/rpc/connection/inputstream.c
  src/rpc/connection/inputstream.h
  src/rpc/connection/outputstream.c
  src/rpc/connection/outputstream.h
  src/rpc/connection/server.c
  src/rpc/connection/server.h
  src/rpc/connection/connection.c
  src/rpc/connection/connection.h
  src/rpc/connection/dispatch.c
  src/rpc/connection/crypto.c
  src/rpc/connection/crypto.h
  src/rpc/connection/loop.c
  src/rpc/connection/loop.h
  src/rpc/msgpack/helpers.c
  src/rpc/msgpack/helpers.h
  src/rpc/db/sb-db.h
  src/rpc/db/connect.c
  src/rpc/db/plugin.c
  src/rpc/db/function.c
  src/rpc/db/auth.c
)

# sb-pluginkey target sources
set(SB-PLUGINKEY-SOURCES
  src/sb-pluginkey.c
  src/sb-common.h
  src/rpc/sb-rpc.h
  src/util.c
  src/confparse.c
  src/parse.c
  src/reallocarray.c
  src/filesystem.c
)

# sb-makekey target sources
set(SB-MAKEKEY-SOURCES
  src/sb-common.h
  src/reallocarray.c
  src/tweetnacl.c
  src/tweetnacl.h
  src/devurandom.c
  src/sb-makekey.c
  src/filesystem.c
)

# splonebox test(s) sources
set(TEST-SOURCES
  src/main.h
  src/sb-common.h
  src/khash.h
  src/klist.h
  src/queue.h
  src/string.c
  src/reallocarray.c
  src/hashmap.c
  src/random.c
  src/optparser.c
  src/optparser.h
  src/signal.c
  src/filesystem.c
  src/devurandom.c
  src/tweetnacl.h
  src/tweetnacl.c
  src/options.c
  src/options.h
  src/confparse.c
  src/confparse.h
  src/parse.c
  src/parse.h
  src/util.c
  src/util.h
  src/address.c
  src/address.h
  src/sbmemzero.c
  src/api/sb-api.h
  src/api/register.c
  src/api/run.c
  src/api/result.c
  src/api/broadcast.c
  src/api/subscribe.c
  src/api/unsubscribe.c
  src/api/helpers.c
  src/api/helpers.h
  src/rpc/sb-rpc.h
  src/rpc/connection/event.c
  src/rpc/connection/event.h
  src/rpc/connection/event-defs.h
  src/rpc/connection/streamhandle.c
  src/rpc/connection/streamhandle.h
  src/rpc/connection/inputstream.c
  src/rpc/connection/inputstream.h
  src/rpc/connection/outputstream.c
  src/rpc/connection/outputstream.h
  src/rpc/connection/server.c
  src/rpc/connection/server.h
  src/rpc/connection/connection.c
  src/rpc/connection/connection.h
  src/rpc/connection/dispatch.c
  src/rpc/connection/crypto.c
  src/rpc/connection/crypto.h
  src/rpc/connection/loop.c
  src/rpc/connection/loop.h
  src/rpc/msgpack/helpers.c
  src/rpc/msgpack/helpers.h
  src/rpc/db/sb-db.h
  src/rpc/db/connect.c
  src/rpc/db/plugin.c
  src/rpc/db/function.c
  src/rpc/db/auth.c
  test/main.c
  test/test-list.h
  test/helper-unix.h
  test/wrapper-functions.c
  test/helper-all.c
  test/helper-all.h
  test/helper-validate.c
  test/helper-validate.h
  test/unit/server-start.c
  test/unit/server-stop.c
  test/unit/dispatch-table-get.c
  test/functional/db-connect.c
  test/functional/db-plugin-add.c
  test/functional/db-pluginkey-verify.c
  test/functional/db-function-register.c
  test/functional/db-function-verify.c
  test/functional/db-function-flush-args.c
  test/functional/filesystem-load.c
  test/functional/filesystem-save-sync.c
  test/functional/dispatch-handle-register.c
  test/functional/dispatch-handle-run.c
  test/functional/dispatch-handle-result.c
  test/functional/dispatch-handle-subscribe.c
  test/functional/dispatch-handle-broadcast.c
  test/functional/crypto.c
  test/functional/confparse.c
  test/functional/db-whitelist.c
  test/functional/msgpack-rpc-helper.c
)

if(CLANG_ADDRESS_SANITIZER OR CLANG_MEMORY_SANITIZER OR CLANG_TSAN)
  list(APPEND gen_cflags "-DEXITFREE")
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/test)
include_directories(SYSTEM ${BSD_INCLUDE_DIRS})
include_directories(SYSTEM ${LIBUV_INCLUDE_DIRS})
include_directories(SYSTEM ${MSGPACK_INCLUDE_DIRS})
include_directories(SYSTEM ${HIREDIS_INCLUDE_DIRS})
include_directories(SYSTEM ${CMOCKA_INCLUDE_DIRS})

# sb target
add_executable(sb ${SPLONEBOX-SOURCES})
target_link_libraries(sb
  ${BSD_LIBRARIES}
  ${LIBUV_LIBRARIES}
  ${MSGPACK_LIBRARIES}
  ${HIREDIS_LIBRARIES}
)

# sb-makekey target
add_executable(sb-makekey ${SB-MAKEKEY-SOURCES})
target_link_libraries(sb-makekey ${BSD_LIBRARIES})

# sb-test target
add_executable(sb-test ${TEST-SOURCES})

# sb-pluginkey target
add_executable(sb-pluginkey ${SB-PLUGINKEY-SOURCES})

# wrap some functions for testing
set_property(TARGET sb-test APPEND_STRING PROPERTY LINK_FLAGS "-Wl,--wrap=outputstream_write,--wrap=loop_process_events_until,--wrap=crypto_write ")
set_property(TARGET sb-test APPEND_STRING PROPERTY COMPILE_FLAGS "-DBOX_UNIT_TESTS ")

target_link_libraries(sb-test
  ${BSD_LIBRARIES}
  ${LIBUV_LIBRARIES}
  ${MSGPACK_LIBRARIES}
  ${HIREDIS_LIBRARIES}
  ${CMOCKA_LIBRARIES}
)

if(USE_COVERAGE)
  set_property(TARGET sb-test APPEND_STRING PROPERTY COMPILE_FLAGS "--coverage ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY LINK_FLAGS "--coverage ")
endif()

if(CLANG_ADDRESS_SANITIZER)
  set_property(TARGET sb APPEND_STRING PROPERTY COMPILE_FLAGS "-DEXITFREE ")
  set_property(TARGET sb APPEND_STRING PROPERTY COMPILE_FLAGS "-fno-sanitize-recover -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address -fsanitize=undefined ")
  set_property(TARGET sb APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=address -fsanitize=undefined ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY COMPILE_FLAGS "-DEXITFREE ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY COMPILE_FLAGS "-fno-sanitize-recover -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address -fsanitize=undefined ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=address -fsanitize=undefined ")
  set_property(TARGET sb-pluginkey APPEND_STRING PROPERTY COMPILE_FLAGS "-DEXITFREE ")
  set_property(TARGET sb-pluginkey APPEND_STRING PROPERTY COMPILE_FLAGS "-fno-sanitize-recover -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address -fsanitize=undefined ")
  set_property(TARGET sb-pluginkey APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=address -fsanitize=undefined ")

elseif(CLANG_MEMORY_SANITIZER)
  set_property(TARGET sb APPEND_STRING PROPERTY COMPILE_FLAGS "-DEXITFREE ")
  set_property(TARGET sb APPEND_STRING PROPERTY COMPILE_FLAGS "-fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
  set_property(TARGET sb APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=memory -fsanitize-memory-track-origins ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY COMPILE_FLAGS "-DEXITFREE ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY COMPILE_FLAGS "-fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=memory -fsanitize-memory-track-origins ")
  set_property(TARGET sb-pluginkey APPEND_STRING PROPERTY COMPILE_FLAGS "-DEXITFREE ")
  set_property(TARGET sb-pluginkey APPEND_STRING PROPERTY COMPILE_FLAGS "-fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
  set_property(TARGET sb-pluginkey APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=memory -fsanitize-memory-track-origins ")
elseif(CLANG_TSAN)
  set_property(TARGET sb APPEND_STRING PROPERTY COMPILE_FLAGS "-DEXITFREE ")
  set_property(TARGET sb APPEND_STRING PROPERTY COMPILE_FLAGS "-fsanitize=thread ")
  set_property(TARGET sb APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=thread ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY COMPILE_FLAGS "-DEXITFREE ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY COMPILE_FLAGS "-fsanitize=thread ")
  set_property(TARGET sb-test APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=thread ")
  set_property(TARGET sb-pluginkey APPEND_STRING PROPERTY COMPILE_FLAGS "-DEXITFREE ")
  set_property(TARGET sb-pluginkey APPEND_STRING PROPERTY COMPILE_FLAGS "-fsanitize=thread ")
  set_property(TARGET sb-pluginkey APPEND_STRING PROPERTY LINK_FLAGS "-fsanitize=thread ")
endif()
