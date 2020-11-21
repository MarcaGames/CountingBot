local currentPath, verbose = ...

local globalApp = require("../premake/app")

local app = globalApp.app("HyperDiscord", currentPath, verbose)
app.kind = "StaticLib"
app.includeDir = "src/HyperDiscord"
app.sourceDir = "src/"

globalApp.addFile(app, "src/**.h", verbose)
globalApp.addFile(app, "src/**.hpp", verbose)
globalApp.addFile(app, "src/**.c", verbose)
globalApp.addFile(app, "src/**.cpp", verbose)
globalApp.addFile(app, "vendor/**.h", verbose)
globalApp.addFile(app, "vendor/**.hpp", verbose)
globalApp.addFile(app, "vendor/**.c", verbose)
globalApp.addFile(app, "vendor/**.cpp", verbose)

globalApp.addState(app, { filter = {}, premakeState = function()
	sysincludedirs{
		app.currentPath .. "vendor/json/"
	}
end}, verbose)

return app