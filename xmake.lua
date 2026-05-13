set_project("argon")
add_rules("mode.debug", "mode.release")
set_languages("c11")
set_warnings("all")
set_encodings("utf-8")
add_includedirs(".")

if is_plat("windows") then
    add_cxflags("/analyze")
end

target("argon")
    set_kind("static")
    add_defines("ARGON_IMPLS")
    add_files("argon_build.c")
    add_headerfiles("argon.h")
    set_default(true)

target("argon_shared")
    set_kind("shared")
    add_defines("ARGON_IMPLS", "ARGON_BUILD_SHARED")
    add_files("argon_build.c")
    add_headerfiles("argon.h")

target("example")
    set_kind("binary")
    add_files("example.c")

for _, test_file in ipairs(os.files("tests/test_*.c")) do
    local name = path.basename(test_file)
    target(name)
        set_kind("binary")
        add_files(test_file)
        add_includedirs(".")
        add_includedirs("tests")

        if xmake.version():ge("3.0.5") then
            add_tests(name, {realtime_output = true})
        else
            add_tests(name)
        end
end
