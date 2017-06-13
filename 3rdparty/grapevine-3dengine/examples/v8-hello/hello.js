var utility = require("scripts/utility.js");
var uiRoot = require("ui.js").root

var print = utility.print;
var Vector3 = grapevine.Vector3;

var canvas = new grapevine.Canvas(480, 800);
var inputSource = new grapevine.InputSource(canvas);

var cameraComponent = new grapevine.CameraComponent(canvas);
var cameraTransformation = new grapevine.TransformationComponent(new Vector3(0, 0, 400));
var camera = new grapevine.Node(cameraComponent, cameraTransformation);

var scene = new grapevine.SimpleScene(camera);

scene.addNode(camera);
scene.addNode(uiRoot.node);

print(utility.indentJavascript(4, uiRoot.toJavascript()))

var running = true;
var time = 0;

var inputFlagMap = {}
inputFlagMap[grapevine.TouchFlags.Pressed]  = grapevine.InputComponentStateFlag.Pressed
inputFlagMap[grapevine.TouchFlags.Moved]    = grapevine.InputComponentStateFlag.Moving
inputFlagMap[grapevine.TouchFlags.Released] = grapevine.InputComponentStateFlag.Released

while(running)
{
    var inputEvent = new grapevine.InputEvent();

    while(inputSource.pollEvent(inputEvent))
    {
        if(inputEvent.type == grapevine.InputEventType.Touch)
        {
            var touchEvent = inputEvent.touch;

            var flags = inputFlagMap[touchEvent.flags]; // TODO fix me to handle multiple simultaneous flags
            if(flags)
            {
                var ray = cameraComponent.mouseCoordinatesToRay(touchEvent.x, touchEvent.y, cameraTransformation.location(), cameraTransformation.rotation());

                var inputComponentEvent = new grapevine.InputComponentEvent()
                inputComponentEvent.ray = ray
                inputComponentEvent.flags = flags;

                // print("Touch: " + touchEvent.x + ", " + touchEvent.y + ": [" +
                //                   ray.origin().x + ", " + ray.origin().y + ", " + ray.origin().z + "], [" +
                //                   ray.unit().x   + ", " + ray.unit().y   + ", " + ray.unit().z   + "]");
                scene.add(inputComponentEvent)
            }



        }
        else if(inputEvent.type == grapevine.InputEventType.System)
        {
            print("System event");
            running = false;
        }
    }


    //rootNode.rotateMe.TransformationComponent.rotation = { angle: 0.15 * time * 100 };

    //<var scale = 1.5 + Math.sin(time);
    //rootNode.rotateMe.scaleMe.TransformationComponent.scale = new Vector3(scale, scale, scale);

   // print(rootNode.rotateMe.TransformationComponent.rotation.angle);

    scene.draw(canvas);

    time += 0.1;
}