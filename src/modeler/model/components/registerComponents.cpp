#include <CAD_modeler/model/components/registerComponents.hpp>

#include <CAD_modeler/model/components/cameraParameters.hpp>
#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/rotation.hpp>
#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/selected.hpp>
#include <CAD_modeler/model/components/torusParameters.hpp>


void RegisterAllComponents(Coordinator& coordinator)
{
    coordinator.RegisterComponent<CameraParameters>();
    coordinator.RegisterComponent<Mesh>();
    coordinator.RegisterComponent<Name>();
    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<Rotation>();
    coordinator.RegisterComponent<Scale>();
    coordinator.RegisterComponent<Selected>();
    coordinator.RegisterComponent<TorusParameters>();
}
