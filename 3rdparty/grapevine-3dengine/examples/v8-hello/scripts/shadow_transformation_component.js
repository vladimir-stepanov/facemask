var utility = require("scripts/utility.js");
var registerComponent = require("scripts/declarative.js").registerComponent;

var rotation_toJavascript = function()
    {
        var jsFragments = [];
        jsFragments.push("{ axis: ");
        jsFragments = jsFragments.concat(this.axis.toJavascript());
        jsFragments.push(", angle: ");
        jsFragments.push(this.angle);
        jsFragments.push("}");
        return jsFragments.join("");
    }

registerComponent("TransformationComponent",
    function(shadowNode, descriptor)
    {
        return new grapevine.TransformationComponent();
    },
    {
        location:
        {
            onUpdate: function(value)
            {
                this.__shadow.component.setLocation(value);
            },
            defaultValue: function()
            {
                return this.__shadow.component.location();
            }
        },
        rotation:
        {
            onUpdate: function(value)
            {
                value.toJavascript = rotation_toJavascript;
                value.axis = (typeof value.axis == "undefined")  ? new grapevine.Vector3(0,0,1) : value.axis;
                value.angle = (typeof value.angle == "undefined")  ? 0.0 : value.angle;

                this.__shadow.rotation = value;
                this.__shadow.component.setRotation(value.axis, value.angle);
            },
            defaultValue: function()
            {
                return this.__shadow.rotation || {axis: new grapevine.Vector3(0,0,1), angle: 0.0, toJavascript: rotation_toJavascript};
            }
        },
        scale:
        {
            onUpdate: function(value)
            {
                this.__shadow.component.setScale(value);
            },
            defaultValue: function()
            {
                return this.__shadow.component.scale();
            }
        }

    });
