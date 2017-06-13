var registerComponent = require("scripts/declarative.js").registerComponent;

registerComponent("InputComponent",
    function(shadowNode, descriptor)
    {
        return new grapevine.InputComponent();
    },
    {
        onEvent:
        {
            onUpdate: function(value)
            {
                var node = this.__shadow.node;
                this.__shadow.onEvent = value;
                this.__shadow.component.setCallback(value.bind(node));
            },
        }
    });