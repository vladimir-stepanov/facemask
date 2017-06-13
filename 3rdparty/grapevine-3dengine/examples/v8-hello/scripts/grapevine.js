grapevine.LOGD = function(string) { grapevine.log("D", string) }
grapevine.LOGI = function(string) { grapevine.log("I", string) }
grapevine.LOGW = function(string) { grapevine.log("W", string) }
grapevine.LOGE = function(string) { grapevine.log("E", string) }
grapevine.Vector3.xAxis = new grapevine.Vector3(1, 0, 0)
grapevine.Vector3.yAxis = new grapevine.Vector3(0, 1, 0)
grapevine.Vector3.zAxis = new grapevine.Vector3(0, 0, 1)

grapevine.Vector2.prototype.toJavascript = function()
{
    return "new Vector2(" + this.x + ", " + this.y + ")"
}

grapevine.Vector3.prototype.toJavascript = function()
{
    return "new Vector3(" + this.x + ", " + this.y + ", " + this.z + ")"
}

grapevine.Vector4.prototype.toJavascript = function()
{
    return "new Vector4(" + this.x + ", " + this.y + ", " + this.z + ", " + this.w + ")"
}

var require
(function()
{
    var loadedModules = {}
    require = function(moduleName)
    {
        if(!loadedModules[moduleName])
        {
            var exports = {}
            var scriptSource = grapevine.LoadScript(moduleName)

            var thisNamespace = {}
            scriptSource.call(thisNamespace, exports)

            loadedModules[moduleName] = exports
        }
        return loadedModules[moduleName]
    }
})()
