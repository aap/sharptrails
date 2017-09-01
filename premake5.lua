workspace "sharptrails"
	configurations { "Release", "DebugIII", "DebugVC" }
	location "build"

	files { "src/*.*" }

	includedirs { "src" }
	includedirs { os.getenv("RWSDK34") }

	includedirs { "../rwd3d9/source" }
	libdirs { "../rwd3d9/libs" }
	links { "rwd3d9.lib" }

	prebuildcommands {
		"for /R \"../shaders/\" %%f in (*.hlsl) do \"%DXSDK_DIR%/Utilities/bin/x86/fxc.exe\" /T ps_2_0 /nologo /E main /Fo ../res/%%~nf.cso %%f",
	}

project "sharptrails"
	kind "SharedLib"
	language "C++"
	targetname "sharptrails"
	targetdir "bin/%{cfg.buildcfg}"
	targetextension ".dll"
	characterset ("MBCS")

	filter "configurations:DebugIII"
		defines { "DEBUG" }
		symbols "On"
		debugdir "C:/Users/aap/games/gta3"
		debugcommand "C:/Users/aap/games/gta3/gta3.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gta3\\dlls\\sharptrails.dll\""

	filter "configurations:DebugVC"
		defines { "DEBUG" }
		symbols "On"
		debugdir "C:/Users/aap/games/gtavc"
		debugcommand "C:/Users/aap/games/gtavc/gta_vc.exe"
		postbuildcommands "copy /y \"$(TargetPath)\" \"C:\\Users\\aap\\games\\gtavc\\dlls\\sharptrails.dll\""

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		flags { "StaticRuntime" }
