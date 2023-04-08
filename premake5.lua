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
IncludeDir["Vulkan"] = "C:/VulkanSDK/1.3.243.0/Include"
IncludeDir["stb"] = "vendor/lib/stb/include"
IncludeDir["tol"] = "vendor/lib/tol/include"
IncludeDir["imgui"] = "vendor/lib/imgui"
IncludeDir["imguibackends"] = "vendor/lib/imgui/backends"

LibDir["Vulkan"] = "C:/VulkanSDK/1.3.243.0/Lib"
LibDir["GLFW"] = "vendor/lib/glfw/bin/Debug-x86_64/GLFW"
LibDir["ImGUI"] = "vendor/lib/imgui/bin/Debug-x86_64/ImGUI"

include "vendor/lib/glfw"
include "vendor/lib/imgui"

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
		"%{IncludeDir.imgui}",
		"%{IncludeDir.imguibackends}",
		"Engine/src",
		"Render-Engine/src"
	}
	
	libdirs {
		"%{LibDir.Vulkan}",
		"%{LibDir.GLFW}",
		"%{LibDir.ImGUI}",
		"bin/" .. outputdir .. "/Engine",
		"bin/" .. outputdir .. "/Render-Engine"
	}
	
	links {
		"GLFW.lib",
		"ImGUI.lib",
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
		"%{IncludeDir.tol}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.imguibackends}",
		"Engine/src",
		"Render-Engine/src"
	}
	
	libdirs {
		"%{LibDir.Vulkan}",
		"%{LibDir.GLFW}",
		"%{LibDir.ImGUI}"
	}
	
	links {
		"GLFW.lib",
		"vulkan-1.lib",
		"ImGUI.lib"
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
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/App"),
			("{COPY} $(SolutionDir)/textures ../bin/" .. outputdir .. "/App/textures"),
			("{COPY} $(SolutionDir)/models ../bin/" .. outputdir .. "/App/models")
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
