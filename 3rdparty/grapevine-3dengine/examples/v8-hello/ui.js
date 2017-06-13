var utility = require("scripts/utility.js");
var declarative = require("scripts/declarative.js");
var Observable = require("scripts/reactive.js").Observable;

var print = utility.print;
var Vector3 = grapevine.Vector3;

require("scripts/shadow_components.js")


declarative.registerClass("Rect:Node",
    function(shadowNode, descriptor)
    {
        var privateUI = declarative.constructPrivate("Node",
        {
            Node:
            [
                {
                    TransformationComponent:
                    {
                        location: new declarative.Binding(
                        [
                            "this.parent.parent.width",
                            "this.parent.parent.height",
                        ],
                        function (width, height)
                        {
                            return new Vector3(-width * 0.5, -height * 0.5, 0);
                        })
                    },
                    ModelComponent:
                    {
                        model: "showboat_textured_with_reflection.barrel",
                    },
                },
                {
                    TransformationComponent:
                    {
                        location: new declarative.Binding(
                        [
                            "this.parent.parent.width",
                            "this.parent.parent.height",
                        ],
                        function (width, height)
                        {
                            return new Vector3(width * 0.5, -height * 0.5, 0);
                        })
                    },
                    ModelComponent:
                    {
                        model: "showboat_textured_with_reflection.barrel",
                    },
                },
                {
                    TransformationComponent:
                    {
                        location: new declarative.Binding(
                        [
                            "this.parent.parent.width",
                            "this.parent.parent.height",
                        ],
                        function (width, height)
                        {
                            return new Vector3(-width * 0.5, height * 0.5, 0);
                        })
                    },
                    ModelComponent:
                    {
                        model: "showboat_textured_with_reflection.barrel",
                    },
                },
                {
                    TransformationComponent:
                    {
                        location: new declarative.Binding(
                        [
                            "this.parent.parent.width",
                            "this.parent.parent.height",
                        ],
                        function (width, height)
                        {
                            return new Vector3(width * 0.5, height * 0.5, 0);
                        })
                    },
                    ModelComponent:
                    {
                        model: "showboat_textured_with_reflection.barrel",
                    },
                },
            ]
        })

        privateUI.parent = shadowNode;
    },
    {
        width:
        {
            defaultValue: 300,
        },
        height:
        {
            defaultValue: 300,
        }
    });


exports.root = declarative.UI(
{
    Rect:
    [
        {
            width: new declarative.Binding(
                    ["this.root.controlNode.TransformationComponent.location"],
                    function(location) { return 2 * Math.abs(location.x) + 300 }),
            height: new declarative.Binding(
                    ["this.root.controlNode.TransformationComponent.location"],
                    function(location) { return 2 * Math.abs(location.y) + 300 }),
        },
    ],
    Node:
    {
        id: "controlNode",

        TransformationComponent:
        {
            location: new Vector3(0,0,0),
        },
        ModelComponent:
        {
            model: "showboat_textured_with_reflection.barrel",
        },
        InputComponent:
        {
            onEvent: function (event)
            {
                var t = -event.ray.origin().z / event.ray.unit().z;
                var x = event.ray.origin().x + t * event.ray.unit().x;
                var y = event.ray.origin().y + t * event.ray.unit().y;
                this.TransformationComponent.location = new Vector3(x, y, 0)
            },
        },
    },
});

