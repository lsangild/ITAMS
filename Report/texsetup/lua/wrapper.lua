-- Import all the lua code. Since this is run from the base LaTeX directory, it is necessary to load relative from that directory, hence the need for "lua/" before all the files.

-- Get git info on the current repository, and expose it in global lua variables.
require("texsetup/lua/git-version.lua")
