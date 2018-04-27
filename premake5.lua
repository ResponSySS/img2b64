workspace "img2b64"
    configurations { "Debug", "Release" }

project "img2b64"
    kind "ConsoleApp"
    language "C"
    targetdir "./bin"
-- links { "" }
    buildoptions { "-Wall -Iinc/" }
    links { "b64" }
    
    files { "**.h", "**.c" }
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
