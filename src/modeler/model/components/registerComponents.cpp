#include <CAD_modeler/model/components/registerComponents.hpp>

#include <CAD_modeler/model/components/cameraParameters.hpp>
#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/rotation.hpp>
#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/selected.hpp>
#include <CAD_modeler/model/components/torusParameters.hpp>
#include <CAD_modeler/model/components/toUpdate.hpp>
#include <CAD_modeler/model/components/controlPoints.hpp>
#include <CAD_modeler/model/components/c0CurveParameters.hpp>
#include <CAD_modeler/model/components/c2CurveParameters.hpp>
#include <CAD_modeler/model/components/bSplinePolygonMesh.hpp>
#include <CAD_modeler/model/components/bezierControlPoints.hpp>
#include <CAD_modeler/model/components/unremovable.hpp>
#include <CAD_modeler/model/components/c0SurfacePatches.hpp>
#include <CAD_modeler/model/components/c0SurfaceDensity.hpp>


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
    coordinator.RegisterComponent<ToUpdate>();
    coordinator.RegisterComponent<ControlPoints>();
    coordinator.RegisterComponent<C0CurveParameters>();
    coordinator.RegisterComponent<C2CurveParameters>();
    coordinator.RegisterComponent<BSplinePolygonMesh>();
    coordinator.RegisterComponent<BezierControlPoints>();
    coordinator.RegisterComponent<Unremovable>();
    coordinator.RegisterComponent<C0SurfacePatches>();
    coordinator.RegisterComponent<C0SurfaceDensity>();
}
