var forEach = require("scripts/utility.js").forEach;
var print = require("scripts/utility.js").print;
var Observable = require("scripts/reactive.js").Observable;
var editorHooks = {}

if(typeof grapevine_vanda != 'undefined')
{
    var NewNode = function() { return new grapevine_vanda.Node(); };
    var nodeAddComponent = function(node, component) { node.add(component); };
    var nodeAddChild = function(node, child) { node.add(child); };
}
else if(typeof grapevine != 'undefined')
{
    var NewNode = function() { return new grapevine.Node(); };
    var nodeAddComponent = function(node, component) { node.addComponent(component); };
    var nodeAddChild = function(node, child) { node.addChild(child); };
}
else
{
    throw new Error("Grapevine and Grapevine-Vanda bingings missing.\nMinimum requirement is to have Grapevine bindings.");
}

function ShadowNode(name, node)
{
    this.__shadow =
    {
        name: name,
        node: node,
        parent: null,
        children: []
    };
}

ShadowNode.prototype.toJavascript = function()
{
    var jsString = "";

    jsString += "{\n";
    this.forEachProperty(function(name, property)
    {
        if(property.value || property.__binding)
        {
            jsString += name + ": ";
            if(property.__binding)
            {
                jsString += property.__binding.toJavascript();
            }
            else
            {
                var value = property.value;
                if(value.toJavascript)
                {
                    jsString += value.toJavascript();
                }
                else if(typeof value == "string")
                {
                    jsString += "\"" + value + "\"";
                }
                else
                {
                    jsString += "" + value;
                }
            }
            jsString += ",\n";
        }
    })

    this.forEachComponent(function(name, component)
    {
        jsString += name + ":\n";
        jsString += component.toJavascript() + ",\n";
    })

    var typeSet = {}
    this.forEachChild(
        function(node)
        {
            typeSet[node.__shadow.name] = (typeSet[node.__shadow.name] || 0) + 1;
        });

    var self = this
    forEach(typeSet, function(name, count)
    {

        var children = 0;
        var childrenJs = "";
        self.forEachChild(
            function(node)
            {
                if(node.__shadow.name == name)
                {
                    var childJs = node.toJavascript();
                    if(childJs && childJs.length > 0)
                    {
                        children++;
                        childrenJs += childJs + ",\n";
                    }
                }
            });

        if(children > 1)
        {
            jsString += name + ":\n[\n";
        }
        else if(children == 1)
        {
            jsString += name + ":\n";
        }

        jsString += childrenJs;

        if(children > 1)
        {
            jsString += "]\n";
        }
    })

    jsString += "}";

    return jsString;
}


ShadowNode.prototype.forEachProperty = function(callback)
{
    forEach(this.__shadow.observables, function(propertyName, property)
    {
        callback(propertyName, property);
    });
};

ShadowNode.prototype.forEachChild = function(callback)
{
    this.__shadow.children.forEach(function(child)
    {
        callback(child)
    });
};

ShadowNode.prototype.forEachNode = function(entry, exit)
{
    entry(this);
    this.__shadow.children.forEach(function(child)
    {
        child.forEachNode(entry, exit);
    });
    if(exit)
    {
        exit(this);
    }
};

ShadowNode.prototype.forEachComponent = function(callback)
{
    forEach(this.__shadow, function(key, value)
    {
        if(value instanceof ShadowComponent)
        {
            var component = value;
            callback(key, component);
        }
    });
};

function ShadowComponent(name, component)
{
    this.__shadow =
    {
        name: name,
        component: component
    };
}

ShadowComponent.prototype.forEachProperty = function(callback)
{
    forEach(this.__shadow.observables, function(propertyName, property)
    {
        callback(propertyName, property);
    });
};

ShadowComponent.prototype.toJavascript = function()
{
    var jsString = "";
    jsString += "{\n";
    this.forEachProperty(function(name, property)
    {
        jsString += name + ": ";
        if(property.__binding)
        {
            jsString += property.__binding.toJavascript();
        }
        else
        {
            var value = property.value;
            if(value.toJavascript)
            {
                jsString += value.toJavascript();
            }
            else if(typeof value == "string")
            {
                jsString += "\"" + value + "\"";
            }
            else
            {
                jsString += "" + value;
            }
        }
        jsString += ",\n";
    })
    jsString += "}";
    return jsString;
}


function Binding(bindings, func)
{
    this.bindings = bindings;
    this.func = func;
}

Binding.prototype.bind = function(obj, property)
{
    var observables = [];
    var failed = false;
    var node = obj instanceof ShadowNode && obj || obj.__shadow.node;

    this.bindings.forEach(function(accessorString)
    {
        if(failed) return;

        var matches = /^((\w+\.)+)(\w+)$/.exec(accessorString);
        var objectAccessor = matches[1].substring(0, matches[1].length - 1);
        var propertyName = matches[3];

        try
        {
            var object = (new Function("return " + objectAccessor)).call(node);
            observables.push(object.__shadow.observables[propertyName]);
        }
        catch(e)
        {
            failed = true;
        }
    });

    if(!failed)
    {
        var observable = obj.__shadow.observables[property]
        observable.bind(observables, this.func);

        return true
    }

    return false;
}

Binding.prototype.toJavascript = function()
{
    var jsString = "";
    jsString += "new declarative.Binding(\n";
    jsString += "[\n\"" + this.bindings.join("\",\n\"") + "\"\n],\n";
    jsString += this.func.toString();
    jsString += ")";

    return jsString;
}

Object.defineProperties(ShadowNode.prototype,
{
    parent:
    {
        set: function(parentShadowNode)
        {
            var currentParent = this.__shadow.parent;
            if(currentParent == parentShadowNode) return;

            if(parentShadowNode !== null)
            {
                nodeAddChild(parentShadowNode.__shadow.node, this.__shadow.node);
                parentShadowNode.__shadow.children.push(this);
                if(this.__shadow.id)
                {
                    if(parentShadowNode[this.__shadow.id])
                    {
                        grapevine.LOGW("Not overwriting node accessor with id " + this.__shadow.id);
                    }
                    else
                    {
                        parentShadowNode[this.__shadow.id] = this;
                    }
                }
            }

            this.__shadow.parent = parentShadowNode;

            if(currentParent !== null)
            {
                currentParent.__shadow.node.removeChild(this.__shadow.node);
                var childIndex = currentParent.__shadow.children.indexOf(this);
                if(childIndex < 0)
                {
                    throw new Error("Corrupted shadow scene graph! Parent is missing a child!");
                }

                currentParent.__shadow.children = currentParent.__shadow.children.splice(childIndex, 1);
                if(currentParent[this.__shadow.id] == this)
                {
                    delete currentParent[this.__shadow.id];
                }
            }
        },

        get: function()
        {
            return this.__shadow.parent;
        },
    },

    children:
    {
        get: function()
        {
            return this.__shadow.children;
        }
    },

    node:
    {
        get: function()
        {
            return this.__shadow.node;
        }
    },

    root:
    {
        get: function()
        {
            var self = this
            var parent = self.parent;
            while(parent)
            {
                self = parent;
                parent = self.parent;
            }

            return self
        }
    }

});

var deferredBindings = []

var nodeClasses =
{
};

var componentClasses =
{
};

function UI(uiDescriptor)
{
    var shadowNode = nodeClasses["Node"].constructor(null, uiDescriptor)

    deferredBindings.forEach(function(entry)
    {
        if(!entry.binding.bind(entry.object, entry.property))
        {
            grapevine.LOGW("Failed to resolve deferred binding");
        }
    })

    return shadowNode;
}

function construct(className, descriptor)
{
    return nodeClasses[className].constructor(null, descriptor);
}

function constructPrivate(className, descriptor)
{
    var shadowNode = construct(className, descriptor);
    shadowNode.toJavascript = function() {return "";};

    return shadowNode;
}

function createObjectProperties(properties)
{
    // creates object properties from onUpdate and defaultValue
    var objectProperties = {}
    forEach(properties, function(propertyName, descriptor)
    {
        var onUpdate = descriptor.onUpdate;
        var defaultValue = descriptor.defaultValue;

        objectProperties[propertyName] = {}
        objectProperties[propertyName].set =
            function(value)
            {
                var observable = this.__shadow.observables[propertyName]
                if(!observable.onUpdate)
                {
                    var self = this
                    observable.onUpdate = function()
                    {
                        if(onUpdate)
                        {
                            onUpdate.call(self, this.value);
                        }
                    }
                }

                if(value instanceof Binding)
                {
                    var binding = value;
                    var result = binding.bind(this, propertyName);
                    if(!result)
                    {
                        deferredBindings.push({ property: propertyName, binding: value, object : this });
                    }
                    observable.__binding = value;
                }
                else
                {
                    observable.value = value;
                    return value;
                }
            }
        objectProperties[propertyName].get =
            function()
            {
                return this.__shadow.observables[propertyName].value;
            };
    });

    return objectProperties;
}

function inherit(className, classProperties, parentClassProperties)
{
    var properties = {}
    var parentClassProperties =
    forEach(parentClassProperties, function(name, value)
    {
        properties[name] = value;
    });
    forEach(classProperties, function(name, value)
    {
        properties[name] = value;
    });
    return properties;
}

function callConstructorChain(className, shadowNode, descriptor)
{
    var parentClass = nodeClasses[className].parentClass;
    if(parentClass)
    {
        callConstructorChain(parentClass, shadowNode, descriptor);
    }
    nodeClasses[className].classConstructor(shadowNode, descriptor);
}

function registerClass(className, classConstructor, classProperties)
{
    // check inheritance
    var matches = /(\w+):(\w+)/.exec(className);
    var parentClass;
    if(matches)
    {
        className = matches[1];
        parentClass = matches[2];
    }

    if(parentClass)
    {
        classProperties = inherit(className, classProperties, nodeClasses[parentClass].properties);
    }

    var objectProperties = createObjectProperties(classProperties);

    nodeClasses[className] =
    {
        constructor: function(parentShadowNode, descriptor)
        {
            var root = NewNode();
            var shadowNode = new ShadowNode(className, root);
            shadowNode.__shadow.observables = {}

            Object.defineProperties(shadowNode, objectProperties);
            forEach(classProperties, function(property, _)
            {
                var defaultValue = classProperties[property].defaultValue;
                shadowNode.__shadow.observables[property] = new Observable(typeof(defaultValue) == "function" && defaultValue.call(this) || defaultValue);
            });

            forEach(descriptor, function(name, descriptor)
            {
                if(name in nodeClasses)
                {
                    var innerConstructor = nodeClasses[name].constructor;
                    if(descriptor instanceof Array)
                    {
                        descriptor.forEach(function(descriptor)
                        {
                            innerConstructor(shadowNode, descriptor);
                        });
                    }
                    else
                    {
                        innerConstructor(shadowNode, descriptor);
                    }
                }
                else
                {
                    shadowNode[name] = descriptor;
                }
            });

            shadowNode.parent = parentShadowNode;

            if(parentClass)
            {
                callConstructorChain(parentClass, shadowNode, descriptor);
            }
            classConstructor(shadowNode, descriptor);

            return shadowNode;
        },

        properties: classProperties,
        classConstructor: classConstructor,
        parentClass: parentClass
    };

    if(editorHooks.registerClass)
    {
        editorHooks.registerClass(className, nodeClasses[className]);
    }
}

function registerComponent(componentName, constructor, properties)
{
    componentClasses[componentName] = properties;

    var objectProperties = createObjectProperties(properties);

    Object.defineProperty(
        ShadowNode.prototype,
        componentName,
        {
            set: function(componentDescriptor)
            {
                var shadowNode = this;
                var component = constructor(shadowNode, componentDescriptor);
                var shadowComponent = new ShadowComponent(componentName, component);
                Object.defineProperties(shadowComponent, objectProperties);

                shadowNode.__shadow[componentName] = shadowComponent;
                shadowComponent.__shadow.node = shadowNode
                if(component)
                {
                    nodeAddComponent(shadowNode.__shadow.node, component);
                }

                shadowComponent.__shadow.observables = {}
                forEach(properties, function(property, _)
                {
                    var defaultValue = properties[property].defaultValue;
                    shadowComponent.__shadow.observables[property] = new Observable(typeof(defaultValue) == "function" && defaultValue.call(shadowComponent) || defaultValue);
                });

                forEach(componentDescriptor, function(property, descriptor)
                {
                    shadowComponent[property] = descriptor;
                });
            },
            get: function()
            {
                return this.__shadow[componentName];
            }
        });


    if(editorHooks.registerComponent)
    {
        editorHooks.registerComponent(componentName, componentClasses[componentName]);
    }

}


registerClass("Node",
    function(shadowNode, descriptor)
    {
    },
    {
        id:
        {
            onUpdate: function(value)
            {
                this.__shadow.id = value
            },
        }
    });


exports.Binding = Binding;
exports.construct = construct;
exports.constructPrivate = constructPrivate;
exports.registerClass = registerClass;
exports.registerComponent = registerComponent;
exports.UI = UI;
exports.editorHooks = editorHooks;
