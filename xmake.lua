add_rules("mode.debug", "mode.release")
set_languages("c++23")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "build" })

add_requires("boost_ut", "boost", "nanobench", "fmt")

target("mnesic")
set_kind("headeronly")
add_headerfiles("src/mnesic/*.hpp")
add_includedirs("src/", { public = true })
add_packages("boost")

target("mnesic-tests")
set_kind("binary")
add_files("tests/*.cpp")
add_deps("mnesic")
add_packages("boost_ut", "fmt")

target("mnesic-bench")
set_kind("binary")
add_files("benchmarks/*.cpp")
add_deps("mnesic")
add_packages("nanobench")

add_subdirs("examples")
