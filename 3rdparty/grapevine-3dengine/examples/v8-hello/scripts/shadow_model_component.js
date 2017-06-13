var registerComponent = require("scripts/declarative.js").registerComponent;

registerComponent("ModelComponent",
    function(shadowNode, descriptor)
    {
    },
    {
        model:
        {
            onUpdate: function(model)
            {
                if(model)
                {
                    this.__shadow.component = new grapevine.ModelComponent(new grapevine.ModelAsset(model));
                    var node = this.__shadow.node.node;
                    node.removeComponent("ModelComponent");
                    node.addComponent(this.__shadow.component);
                }
            },
        }
    });