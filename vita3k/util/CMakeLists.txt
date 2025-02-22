add_library(
	util
	STATIC
	src/arm.cpp
	src/byte.cpp
	src/float_to_half.cpp
	src/fs_utils.cpp
	src/hash.cpp
	src/instrset_detect.cpp
	src/logging.cpp
	src/net_utils.cpp
	src/string_utils.cpp
	src/tracy.cpp
)

target_include_directories(util PUBLIC include)
target_link_libraries(util PUBLIC ${Boost_LIBRARIES} fmt spdlog http mem)
target_link_libraries(util PRIVATE libcurl crypto)
target_compile_definitions(util PRIVATE $<$<CONFIG:Debug,RelWithDebInfo>:TRACY_ENABLE>)
target_link_libraries(util PRIVATE sdl2)

if(ANDROID)
	target_link_libraries(util PUBLIC emuenv sdl2 android xxHash::xxhash)
endif()
