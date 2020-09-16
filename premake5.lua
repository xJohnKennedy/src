-- For reference, please refer to the premake wiki:
-- https://github.com/premake/premake-core/wiki

-- a base desse script pode ser encontrada em 
-- https://github.com/JohannesMP/Premake-for-Beginners

local ROOT = "../"

---------------------------------
-- [ WORKSPACE CONFIGURATION   --
---------------------------------
workspace "RUNGE_FORCA_BRUTA_BACIA"                   -- Solution Name
	configurations
	{ 
		"Debug", "Release" , "Release_Windows_7" 
	}  		-- Optimization/General config mode in VS

	platforms									-- Dropdown platforms section in VS
	{ 
		"x64"
	}       		 		

	-- _ACTION is the argument you passed into premake5 when you ran it.
	local project_action = "UNDEFINED"
	if _ACTION ~= nill then project_action = _ACTION end


	-- Where the project files (vs project, solution, etc) go
	location( "sln/")

	-------------------------------
	-- [ COMPILER/LINKER CONFIG] --
	-------------------------------
	--flags "FatalWarnings" -- comment if you don't want warnings to count as errors
	warnings "Extra"

	-- see 'filter' in the wiki pages
	filter "configurations:Debug"    
		defines { "DEBUG" }  
		symbols  "On"

	filter "configurations:Release"  
		defines { "NDEBUG" } 
		optimize "Speed"		-- otimiza o projeto para o modo velocidade, no MSVC normalemnte é aplicado a flag /O2

	filter "configurations:Release_Windows_7"  
		defines { "NDEBUG" } 
		optimize "Speed"		-- otimiza o projeto para o modo velocidade, no MSVC normalemnte é aplicado a flag /O2
		staticruntime "On"		-- para rodar em windows 7 que ainda não foi atualizado e não possui o Universal CRT (UCRT)

	filter { "platforms:*32" } 
		architecture "x86"
	filter { "platforms:*64" } 
		architecture "x64"

	-- when building any visual studio project
	filter { "system:windows", "action:vs*"}
		flags         { "MultiProcessorCompile", "NoMinimalRebuild" }
		linkoptions   { "/ignore:4099" }      -- Ignore library pdb warnings when running in debug


	filter {} -- clear filter when you know you no longer need it!


	-------------------------------
	-- [ PROJECT CONFIGURATION ] --
	------------------------------- 
	project "Rkutta"
		kind "ConsoleApp" -- "WindowApp" removes console
		language "C"
		
		-- where the output binary goes. 	
		-- -> exe/Debug/Rkutta
		-- -> exe/Release/Rkutta
		targetdir "exe/%{cfg.buildcfg}/%{prj.name}"
		
		-- where the output binary goes. 	
		-- -> obj/Debug/Rkutta
		-- -> obj/Release/Rkutta
		objdir "obj/%{cfg.buildcfg}/%{prj.name}"
		
		-- the name of the executable saved to targetdir		
		-- -> Rkutta.exe
		targetname "%{prj.name}"


		--------------------------------------
		-- [ PROJECT FILES CONFIGURATIONS ] --
		--------------------------------------
		local SourceDir = ROOT .. "src/%{prj.name}/";	--../src/Rkutta/
		-- what files the visual studio project/makefile/etc should know about
		files
		{ 
		  SourceDir .. "**.h", 
		  SourceDir .. "**.hpp", 
		  SourceDir .. "**.c", 
		  SourceDir .. "**.cpp", 
		  SourceDir .. "**.tpp",
		  "**.h", "**.hpp", "**.c", "**.cpp", "**.tpp",
		}

		-- Exclude template files from project (so they don't accidentally get compiled)
		filter { "files:**.tpp" }
		  flags {"ExcludeFromBuild"}

		filter {} -- clear filter!


		-- setting up visual studio filters (basically virtual folders).
		vpaths 
		{
		  ["Header Files/*"] = { SourceDir .. "**.h", SourceDir .. "**.hxx", SourceDir .. "**.hpp" },
		  ["Source Files/*"] = { SourceDir .. "**.c", SourceDir .. "**.cxx", SourceDir .. "**.cpp" },
		}


		-- where to find header files that you might be including, mainly for library headers.
		includedirs
		{
			"./",				--./
			ROOT .. "src/",		--../src/
			SourceDir			--../src/Rkutta/  -- include root source directory to allow for absolute include paths
		  -- include the headers of any libraries/dlls you need
		}


		-------------------------------------------
		-- [ PROJECT DEPENDENCY CONFIGURATIONS ] --
		-------------------------------------------
		libdirs
		{
		  -- add dependency directories here
		}

		links
		{
		  -- add depedencies (libraries) here
		}

		-------------------------------------------
		--------- [ POST BUILD COMMANDS ] ---------
		-------------------------------------------

		configuration "Release"
			   postbuildcommands { "cd ..\\exe\\Release"}
			   postbuildcommands { "copy Rkutta\\Rkutta.exe .\\"}
		
	-------------------------------
	-- [ PROJECT CONFIGURATION ] --
	------------------------------- 
	project "Forca_br"
		kind "ConsoleApp" -- "WindowApp" removes console
		language "C"
		
		-- where the output binary goes. 	
		-- -> exe/Debug/Rkutta
		-- -> exe/Release/Rkutta
		targetdir "exe/%{cfg.buildcfg}/%{prj.name}"
		
		-- where the output binary goes. 	
		-- -> obj/Debug/Rkutta
		-- -> obj/Release/Rkutta
		objdir "obj/%{cfg.buildcfg}/%{prj.name}"
		
		-- the name of the executable saved to targetdir		
		-- -> Rkutta.exe
		targetname "%{prj.name}"


		--------------------------------------
		-- [ PROJECT FILES CONFIGURATIONS ] --
		--------------------------------------
		local SourceDir = ROOT .. "src/%{prj.name}/";	--../src/Forca_br/
		-- what files the visual studio project/makefile/etc should know about
		files
		{ 
		  SourceDir .. "**.h", 
		  SourceDir .. "**.hpp", 
		  SourceDir .. "**.c", 
		  SourceDir .. "**.cpp", 
		  SourceDir .. "**.tpp",
		  "**.h", "**.hpp", "**.c", "**.cpp", "**.tpp",
		}

		-- Exclude template files from project (so they don't accidentally get compiled)
		filter { "files:**.tpp" }
		  flags {"ExcludeFromBuild"}

		filter {} -- clear filter!


		-- setting up visual studio filters (basically virtual folders).
		vpaths 
		{
		  ["Header Files/*"] = { SourceDir .. "**.h", SourceDir .. "**.hxx", SourceDir .. "**.hpp" },
		  ["Source Files/*"] = { SourceDir .. "**.c", SourceDir .. "**.cxx", SourceDir .. "**.cpp" },
		}


		-- where to find header files that you might be including, mainly for library headers.
		includedirs
		{
			"./",				--./
			ROOT .. "src/",		--../src/
			SourceDir			--../src/Forca_br/  -- include root source directory to allow for absolute include paths
		  -- include the headers of any libraries/dlls you need
		}


		-------------------------------------------
		-- [ PROJECT DEPENDENCY CONFIGURATIONS ] --
		-------------------------------------------
		libdirs
		{
		  -- add dependency directories here
		}

		links
		{
		  -- add depedencies (libraries) here
		}

		-------------------------------------------
		--------- [ POST BUILD COMMANDS ] ---------
		-------------------------------------------

		configuration "Release"
			   postbuildcommands { "cd ..\\exe\\Release"}
			   postbuildcommands { "copy Forca_br\\Forca_br.exe .\\"}
	
	-------------------------------
	-- [ PROJECT CONFIGURATION ] --
	------------------------------- 
	project "Bacia_forca_bruta"
		kind "ConsoleApp" -- "WindowApp" removes console
		language "C"
		
		-- where the output binary goes. 	
		-- -> exe/Debug/Rkutta
		-- -> exe/Release/Rkutta
		targetdir "exe/%{cfg.buildcfg}/%{prj.name}"
		
		-- where the output binary goes. 	
		-- -> obj/Debug/Rkutta
		-- -> obj/Release/Rkutta
		objdir "obj/%{cfg.buildcfg}/%{prj.name}"
		
		-- the name of the executable saved to targetdir		
		-- -> Rkutta.exe
		targetname "%{prj.name}"


		--------------------------------------
		-- [ PROJECT FILES CONFIGURATIONS ] --
		--------------------------------------
		local SourceDir = ROOT .. "src/%{prj.name}/";	--../src/Bacia_forca_bruta/
		-- what files the visual studio project/makefile/etc should know about
		files
		{ 
		  SourceDir .. "**.h", 
		  SourceDir .. "**.hpp", 
		  SourceDir .. "**.c", 
		  SourceDir .. "**.cpp", 
		  SourceDir .. "**.tpp",
		  "**.h", "**.hpp", "**.c", "**.cpp", "**.tpp",
		}

		-- Exclude template files from project (so they don't accidentally get compiled)
		filter { "files:**.tpp" }
		  flags {"ExcludeFromBuild"}

		filter {} -- clear filter!


		-- setting up visual studio filters (basically virtual folders).
		vpaths 
		{
		  ["Header Files/*"] = { SourceDir .. "**.h", SourceDir .. "**.hxx", SourceDir .. "**.hpp" },
		  ["Source Files/*"] = { SourceDir .. "**.c", SourceDir .. "**.cxx", SourceDir .. "**.cpp" },
		}


		-- where to find header files that you might be including, mainly for library headers.
		includedirs
		{
			"./",				--./
			ROOT .. "src/",		--../src/
			SourceDir			--../src/Bacia_forca_bruta/  -- include root source directory to allow for absolute include paths
		  -- include the headers of any libraries/dlls you need
		}


		-------------------------------------------
		-- [ PROJECT DEPENDENCY CONFIGURATIONS ] --
		-------------------------------------------
		libdirs
		{
		  -- add dependency directories here
		}

		links
		{
		  -- add depedencies (libraries) here
		}

		-------------------------------------------
		--------- [ POST BUILD COMMANDS ] ---------
		-------------------------------------------

		configuration "Release"
			   postbuildcommands { "cd ..\\exe\\Release"}
			   postbuildcommands { "copy Bacia_forca_bruta\\Bacia_forca_bruta.exe .\\"}
	
	
	-------------------------------------------
	--   [ COPYING CONFIGURATION FILES ]   --
	-------------------------------------------
	local SourceConfigDir = "..\\" .. "src\\config\\";	--../src/config/

	-- copia os arquivos, mas não sobrescreve se no destino o arquivo já existir visto que estes arquivos são de configuração independentes
	ok, err = os.execute("Echo n|COPY /-y /v " .. SourceConfigDir .. "Kutta_header_config.h" .. " .\\_config_modelo\\")
	ok, err = os.execute("Echo n|COPY /-y /v " .. SourceConfigDir .. "Nequ_config.h" .. " .\\_config_modelo\\")

