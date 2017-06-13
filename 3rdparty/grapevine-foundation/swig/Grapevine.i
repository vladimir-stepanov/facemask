%module grapevine

%{
    #include<Grapevine/Core/Class.hpp>
    #include<Grapevine/Canvas/Canvas.hpp>
    #include<Grapevine/Input/InputSource.hpp>
    #include<Grapevine/Input/InputEvents.hpp>

    #include<Grapevine/Material/MaterialAsset.hpp>
    #include<Grapevine/Material/TextureAsset.hpp>
    #include<Grapevine/Model/MeshAsset.hpp>
    #include<Grapevine/Scene/Component.hpp>
    #include<Grapevine/Scene/Node.hpp>
    #include<Grapevine/SceneExtensions/ModelComponent.hpp>
    #include<Grapevine/SceneExtensions/CameraComponent.hpp>
    #include<Grapevine/SceneExtensions/SimpleScene.hpp>
    #include<Grapevine/SceneExtensions/TransformationComponent.hpp>
    #include<Grapevine/Math/Vector.hpp>

//    using namespace Grapevine;
//    using namespace Scene;
%}

namespace Grapevine
{
    namespace Scene
    {
        class AbstractComponentPrivate;
        class AbstractTreeVisitor;
        class AbstractListVisitor;
    }

    class ReferencedPointer;
    //force the use of value wrapper for ModelAsset
    %feature("valuewrapper") ModelAsset;
    class ModelAsset;
}
%include<Grapevine/Core/Class.hpp>

%include<Grapevine/Math/Vector2Template.hpp>
%include<Grapevine/Math/Vector3Template.hpp>
%include<Grapevine/Math/Vector4Template.hpp>

%template(Vector2) Grapevine::Vector2Template<float>;
%template(Vector3) Grapevine::Vector3Template<float>;
%template(Vector4) Grapevine::Vector4Template<float>;

%include<Grapevine/Math/Vector.hpp>

%include<Grapevine/Input/InputEvents.hpp>
%include<Grapevine/Canvas/Canvas.hpp>
%include<Grapevine/Input/InputSource.hpp>

%include<Grapevine/Core/Buffer.hpp>
%include<Grapevine/Image/Bitmap.hpp>
%include<Grapevine/Material/TextureAsset.hpp>

%include<Grapevine/Animation/ControlPoint.hpp>
%include<Grapevine/Model/DynamicMesh.hpp>
%include<Grapevine/Model/MeshAsset.hpp>
%include<Grapevine/Material/MaterialAsset.hpp>



%include<Grapevine/Scene/Component.hpp>

%include<Grapevine/Scene/NodePrivate.hpp>
%include<Grapevine/Scene/Node.hpp>
%include<Grapevine/SceneExtensions/ModelComponent.hpp>
%include<Grapevine/SceneExtensions/SimpleScene.hpp>
%include<Grapevine/SceneExtensions/TransformationComponent.hpp>

