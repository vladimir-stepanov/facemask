// "One API"
//
// Include all basic classes to here needed for simple apps

// grapevine manager classes
#include <Grapevine/Resource/ResourceManager.hpp>
#include <Grapevine/Canvas/Canvas.hpp>
#include <Grapevine/Input/InputSource.hpp>

// grapevine basic extensions
#include <Grapevine/SceneExtensions/TransformationComponent.hpp>
#include <Grapevine/SceneExtensions/TransformationComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/CameraComponent.hpp>
#include <Grapevine/SceneExtensions/CameraComponentPrivate.hpp>
#include <Grapevine/SceneExtensions/ModelComponent.hpp>
#include <Grapevine/SceneExtensions/TextComponent.hpp>
#include <Grapevine/SceneExtensions/AnimationComponent.hpp>
#include <Grapevine/SceneExtensions/AnimationProgressTriggerComponent.hpp>
#include <Grapevine/SceneExtensions/SimpleScene.hpp>

// grapevine scene structure
#include <Grapevine/Scene/Graph.hpp>
#include <Grapevine/Scene/Node.hpp>

// animation
#include <Grapevine/Animation/Animation.hpp>
#include <Grapevine/Animation/LinearPath.hpp>
#include <Grapevine/Animation/BezierQuadraticPath.hpp>

// model loading & dynamic mesh
#include <Grapevine/Model/ModelAsset.hpp>
#include <Grapevine/Model/DynamicMesh.hpp>

// materials
#include <Grapevine/Material/MaterialAsset.hpp>
#include <Grapevine/Material/TextureAsset.hpp>

// core basic functionality
#include <Grapevine/Core/Log.hpp>
#include <Grapevine/Core/Exception.hpp>
#include <Grapevine/Core/Timer.hpp>
#include <Grapevine/Core/ReferencedPointer.hpp>

// math
#include <Grapevine/Math/Vector.hpp>
#include <Grapevine/Math/Matrix.hpp>

// other
#include <Grapevine/Arrange/Grid.hpp>
#include <Grapevine/Math/Ray.hpp>
#include <Grapevine/Benchmark/FpsCounter.hpp>
