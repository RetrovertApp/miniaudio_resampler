require "tundra.syntax.glob"
require "tundra.path"
require "tundra.util"

-----------------------------------------------------------------------------------------------------------------------

SharedLibrary {
	Name = "miniaudio_resampler",

    Env = {
        CCOPTS = { "-Wno-unused-function"; Config = { "macos-*-*", "linux-*-*" } },
	},

	Includes = { "../retrovert_api/c" },
	Sources = { "miniaudio_resampler.c" },
}

-----------------------------------------------------------------------------------------------------------------------

Default "miniaudio_resampler"
