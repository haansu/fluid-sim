--
-- Solution configuration file | ONLY FOR Windows
--

workspace "fluid-sim"
	architecture "x64"

	startproject "App"

	configurations {
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

IncludeDir = {}
LibDir = {}

IncludeDir["GLFW"] = "vendor/lib/glfw/include"
IncludeDir["Vulkan"] = "C:/VulkanSDK/1.3.216.0/Include"
IncludeDir["stb"] = "vendor/lib/stb/include"

LibDir["Vulkan"] = "C:/VulkanSDK/1.3.216.0/Lib"
LibDir["GLFW"] = "vendor/lib/glfw/bin/Debug-x86_64/GLFW"

include "vendor/lib/glfw"

--
-- App
--

project "App"
	location "App"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")
	debugdir("bin/" .. outputdir .. "/App")

	files {
		"App/src/**.h",
		"App/src/**.cpp"
	}

	includedirs { 
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Vulkan}",
		"Engine/src",
		"Render-Engine/src"
	}

	libdirs {
		"%{LibDir.Vulkan}",
		"%{LibDir.GLFW}",
		"bin/" .. outputdir .. "/Engine",
		"bin/" .. outputdir .. "/Render-Engine"
	}

	links {
		"GLFW.lib",
		"vulkan-1.lib",
		"Render-Engine"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines { "FS_APP" }
	
	filter "configurations:Debug"
		defines "FS_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "FS_RELEASE"
		buildoptions "/MD"
		optimize "On"

--
-- Engine
--

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")
	debugdir("bin/" .. outputdir .. "/App")

	pchheader "pch.h"
	pchsource "Engine/src/pch.cpp"

	files {
		"Engine/src/**.h",
		"Engine/src/**.cpp"
	}

	includedirs { 
		"Engine/src"
	}

	links {	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/App")
		}

		defines {
			"FS_ENGINE",
			"ENGINE_API_EXPORT"
		}
	
	filter "configurations:Debug"
		defines "FS_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "FS_RELEASE"
		buildoptions "/MD"
		optimize "On"

--
-- Render-Engine
--

project "Render-Engine"
	location "Render-Engine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")
	debugdir("bin/" .. outputdir .. "/App")

	pchheader "pch.h"
	pchsource "Render-Engine/src/pch.cpp"

	files {
		"Render-Engine/src/**.h",
		"Render-Engine/src/**.cpp"
	}

	includedirs {
		"%{IncludeDir.stb}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Vulkan}",
		"Engine/src"
	}

	libdirs {
		"%{LibDir.Vulkan}",
		"%{LibDir.GLFW}"
	}

	links {
		"GLFW.lib",
		"vulkan-1.lib"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		prebuildcommands {
			("call $(SolutionDir)scripts/shader_compile.bat"),
			("{COPY} $(SolutionDir)shaders/bin $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/shaders"),
			("{COPY} $(SolutionDir)bin/" .. outputdir .. "/%{prj.name}/shaders ../bin/" .. outputdir .. "/App/shaders")
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/App")
		}

		defines {
			"FS_RENDER_ENGINE",
			"RENDER_API_EXPORT"
		}
	
	filter "configurations:Debug"
		defines "FS_DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "FS_RELEASE"
		buildoptions "/MD"
		optimize "On"
