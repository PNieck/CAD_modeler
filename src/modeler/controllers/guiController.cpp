#include <CAD_modeler/controllers/guiController.hpp>

#include <stdexcept>


GuiController::GuiController(Model & model, MainController & controller):
    SubController(model, controller)
{
}


Entity GuiController::AddCurve(const std::vector<Entity>& entities, CurveType curveType) const
{
    switch (curveType)
    {
    case CurveType::C0:
        return model.AddC0Curve(entities);
    
    case CurveType::C2:
        return model.AddC2Curve(entities);

    case CurveType::Interpolation:
        return model.AddInterpolationCurve(entities);

    default:
        throw std::runtime_error("Unknown curve type");
    }
}


Entity GuiController::AddSurface(SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width) const
{
    switch (surfaceType)
    {
    case SurfaceType::C0:
        return model.AddC0Surface(dir, length, width);

    case SurfaceType::C2:
        return model.AddC2Surface(dir, length, width);
    
    default:
        throw std::runtime_error("Unknown surface type");
    }
}


Entity GuiController::AddCylinder(CylinderType cylinderType) const
{
    switch (cylinderType)
    {
    case CylinderType::C0:
        return model.AddC0Cylinder();

    case CylinderType::C2:
        return model.AddC2Cylinder();
    
    default:
        throw std::runtime_error("Unknown cylinder type");
    }
}


void GuiController::AddRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
{
    switch (surfaceType)
    {
    case SurfaceType::C0:
        model.AddRowOfC0SurfacePatches(surface, dir, length, width);
        break;

    case SurfaceType::C2:
        model.AddRowOfC2SurfacePatches(surface, dir, length, width);
        break;

    default:
        throw std::runtime_error("Unknown surface type");
    }
}


void GuiController::AddColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
{
    switch (surfaceType)
    {
    case SurfaceType::C0:
        model.AddColOfC0SurfacePatches(surface, dir, length, width);
        break;

    case SurfaceType::C2:
        model.AddColOfC2SurfacePatches(surface, dir, length, width);
        break;

    default:
        throw std::runtime_error("Unknown surface type");
    }
}


void GuiController::DeleteRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
{
    switch (surfaceType)
    {
    case SurfaceType::C0:
        model.DeleteRowOfC0SurfacePatches(surface, dir, length, width);
        break;

    case SurfaceType::C2:
        model.DeleteRowOfC2SurfacePatches(surface, dir, length, width);
        break;

    default:
        throw std::runtime_error("Unknown surface type");
    }
}


void GuiController::DeleteColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
{
    switch (surfaceType)
    {
    case SurfaceType::C0:
        model.DeleteColOfC0SurfacePatches(surface, dir, length, width);
        break;

    case SurfaceType::C2:
        model.DeleteColOfC2SurfacePatches(surface, dir, length, width);
        break;

    default:
        throw std::runtime_error("Unknown surface type");
    }
}


void GuiController::RecalculateSurface(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
{
    switch (surfaceType)
    {
    case SurfaceType::C0:
        model.RecalculateC0Surface(surface, dir, length, width);
        break;

    case SurfaceType::C2:
        model.RecalculateC2Surface(surface, dir, length, width);
        break;

    default:
        throw std::runtime_error("Unknown surface type");
    }
}


void GuiController::AddRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
{
    switch (cylinderType)
    {
    case CylinderType::C0:
        model.AddRowOfC0CylinderPatches(cylinder, radius, dir);
        break;

    case CylinderType::C2:
        model.AddRowOfC2CylinderPatches(cylinder, radius, dir);
        break;

    default:
        throw std::runtime_error("Unknown cylinder type");
    }
}


void GuiController::AddColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
{
    switch (cylinderType)
    {
    case CylinderType::C0:
        model.AddColOfC0CylinderPatches(cylinder, radius, dir);
        break;

    case CylinderType::C2:
        model.AddColOfC2CylinderPatches(cylinder, radius, dir);
        break;

    default:
        throw std::runtime_error("Unknown cylinder type");
    }
}


void GuiController::DeleteRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
{
    switch (cylinderType)
    {
    case CylinderType::C0:
        model.DeleteRowOfC0CylinderPatches(cylinder, radius, dir);
        break;

    case CylinderType::C2:
        model.DeleteRowOfC2CylinderPatches(cylinder, radius, dir);
        break;

    default:
        throw std::runtime_error("Unknown cylinder type");
    }
}


void GuiController::DeleteColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
{
    switch (cylinderType)
    {
    case CylinderType::C0:
        model.DeleteColOfC0CylinderPatches(cylinder, radius, dir);
        break;

    case CylinderType::C2:
        model.DeleteColOfC2CylinderPatches(cylinder, radius, dir);
        break;

    default:
        throw std::runtime_error("Unknown cylinder type");
    }
}


void GuiController::RecalculateCylinder(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
{
    switch (cylinderType)
    {
    case CylinderType::C0:
        model.RecalculateC0Cylinder(cylinder, radius, dir);
        break;

    case CylinderType::C2:
        model.RecalculateC2Cylinder(cylinder, radius, dir);
        break;

    default:
        throw std::runtime_error("Unknown cylinder type");
    }
}
