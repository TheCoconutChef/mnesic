add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "build" })
set_languages("c++17")

add_requires("boost_ut", "nanobench", "fmt")

target("mnesic")
set_kind("headeronly")
add_headerfiles("src/mnesic/*.hpp")
add_includedirs("src/", { public = true })
add_packages("boost_callable_traits")

target("mnesic-tests")
set_languages("c++20") -- for boost_ut
set_kind("binary")
add_files("tests/*.cpp")
add_deps("mnesic")
add_packages("boost_ut", "fmt")

target("mnesic-bench")
set_kind("binary")
add_files("benchmarks/*.cpp")
add_deps("mnesic")
add_packages("nanobench")

includes("examples")
