--
-- clang.lua
-- Clang toolset adapter for Premake
-- Copyright (c) 2013 Jason Perkins and the Premake project
--

	premake.tools.clang = {}
	local clang = premake.tools.clang
	local gcc = premake.tools.gcc
	local project = premake.project
	local config = premake.config


--
-- Build a list of flags for the C preprocessor corresponding to the
-- settings in a particular project configuration.
--
-- @param cfg
--    The project configuration.
-- @return
--    An array of C preprocessor flags.
--

	clang.cppflags = {
		system = {
			haiku = "-MMD",
			wii = { "-MMD", "-MP", "-I$(LIBOGC_INC)", "$(MACHDEP)" },
			_ = { "-MMD", "-MP" }
		}
	}

	function clang.getcppflags(cfg)
		local flags = config.mapFlags(cfg, clang.cppflags)
		return flags
	end


--
-- Build a list of C compiler flags corresponding to the settings in
-- a particular project configuration. These flags are exclusive
-- of the C++ compiler flags, there is no overlap.
--
-- @param cfg
--    The project configuration.
-- @return
--    An array of C compiler flags.
--

	clang.cflags = {
		architecture = {
			x32 = "-m32",
			x64 = "-m64",
		},
		flags = {
			FatalWarnings = "-Werror",
			NoFramePointer = "-fomit-frame-pointer",
			Symbols = "-g"
		},
		floatingpoint = {
			Fast = "-ffast-math",
			Strict = "-ffloat-store",
		},
		kind = {
			SharedLib = function(cfg)
				if cfg.system ~= premake.WINDOWS then return "-fPIC" end
			end,
		},
		optimize = {
			Off = "-O0",
			On = "-O2",
			Debug = "-Og",
			Full = "-O3",
			Size = "-Os",
			Speed = "-O3",
		},
		vectorextensions = {
			SSE = "-msse",
			SSE2 = "-msse2",
		},
		warnings = {
			Extra = "-Wall -Wextra",
			Off = "-w",
		}
	}

	function clang.getcflags(cfg)
		local flags = config.mapFlags(cfg, clang.cflags)
		return flags
	end



--
-- Build a list of C++ compiler flags corresponding to the settings
-- in a particular project configuration. These flags are exclusive
-- of the C compiler flags, there is no overlap.
--
-- @param cfg
--    The project configuration.
-- @return
--    An array of C++ compiler flags.
--

	clang.cxxflags = {
		flags = {
			NoExceptions = "-fno-exceptions",
			NoRTTI = "-fno-rtti",
			NoBufferSecurityCheck = "-fno-stack-protector"
		}
	}

	function clang.getcxxflags(cfg)
		local flags = config.mapFlags(cfg, clang.cxxflags)
		return flags
	end



--
-- Returns a list of defined preprocessor symbols, decorated for
-- the compiler command line.
--
-- @param defines
--    An array of preprocessor symbols to define; as an array of
--    string values.
-- @return
--    An array of symbols with the appropriate flag decorations.
--

	function clang.getdefines(defines)

		-- Just pass through to GCC for now
		local flags = gcc.getdefines(defines)
		return flags

	end



--
-- Returns a list of forced include files, decorated for the compiler
-- command line.
--
-- @param cfg
--    The project configuration.
-- @return
--    An array of force include files with the appropriate flags.
--

	function clang.getforceincludes(cfg)

		-- Just pass through to GCC for now
		local flags = gcc.getforceincludes(cfg)
		return flags

	end


--
-- Returns a list of include file search directories, decorated for
-- the compiler command line.
--
-- @param cfg
--    The project configuration.
-- @param dirs
--    An array of include file search directories; as an array of
--    string values.
-- @return
--    An array of symbols with the appropriate flag decorations.
--

	function clang.getincludedirs(cfg, dirs)

		-- Just pass through to GCC for now
		local flags = gcc.getincludedirs(cfg, dirs)
		return flags

	end


--
-- Build a list of linker flags corresponding to the settings in
-- a particular project configuration.
--
-- @param cfg
--    The project configuration.
-- @return
--    An array of linker flags.
--

	clang.ldflags = {
		architecture = {
			x32 = { "-m32", "-L/usr/lib32" },
			x64 = { "-m64", "-L/usr/lib64" },
		},
		kind = {
			SharedLib = function(cfg)
				local r = { iif(cfg.system == premake.MACOSX, "-dynamiclib", "-shared") }
				if cfg.system == "windows" and not cfg.flags.NoImportLib then
					table.insert(r, '-Wl,--out-implib="' .. cfg.linktarget.relpath .. '"')
				end
				return r
			end,
			WindowedApp = function(cfg)
				if cfg.system == premake.WINDOWS then return "-mwindows" end
			end,
		},
		system = {
			wii = { "-L$(LIBOGC_LIB)", "$(MACHDEP)" }
		}
	}

	function clang.getldflags(cfg)
		local flags = config.mapFlags(cfg, clang.ldflags)

		-- Scan the list of linked libraries. If any are referenced with
		-- paths, add those to the list of library search paths
		for _, dir in ipairs(config.getlinks(cfg, "system", "directory")) do
			table.insert(flags, '-L' .. project.getrelative(cfg.project, dir))
		end

		return flags
	end


--
-- Build a list of libraries to be linked for a particular project
-- configuration, decorated for the linker command line.
--
-- @param cfg
--    The project configuration.
-- @param systemOnly
--    Boolean flag indicating whether to link only system libraries,
--    or system libraries and sibling projects as well.
-- @return
--    A list of libraries to link, decorated for the linker.
--

	function clang.getlinks(cfg, systemOnly)

		-- Just pass through to GCC for now
		local flags = gcc.getlinks(cfg, systemOnly)
		return flags

	end


--
-- Return a list of makefile-specific configuration rules. This will
-- be going away when I get a chance to overhaul these adapters.
--
-- @param cfg
--    The project configuration.
-- @return
--    A list of additional makefile rules.
--

	clang.makesettings = {
		system = {
		}
	}

	function clang.getmakesettings(cfg)
		local settings = config.mapFlags(cfg, clang.makesettings)
		return table.concat(settings)
	end


--
-- Retrieves the executable command name for a tool, based on the
-- provided configuration and the operating environment. I will
-- be moving these into global configuration blocks when I get
-- the chance.
--
-- @param cfg
--    The configuration to query.
-- @param tool
--    The tool to fetch, one of "cc" for the C compiler, "cxx" for
--    the C++ compiler, or "ar" for the static linker.
-- @return
--    The executable command name for a tool, or nil if the system's
--    default value should be used.
--

	clang.tools = {
		cc = "clang",
		cxx = "clang++",
		ar = "ar"
	}

	function clang.gettoolname(cfg, tool)
		return clang.tools[tool]
	end
