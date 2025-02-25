#include <CAD_modeler/views/modeler/modelerMainMenuView.hpp>
#include <CAD_modeler/views/modeler/modelerGuiUtils.hpp>


void AddControlPointToCurve(Modeler& model, Entity curve, Entity entity, CurveType curveType)
{
    switch (curveType)
    {
    case CurveType::C0:
        model.AddControlPointToC0Curve(curve, entity);
        break;

    case CurveType::C2:
        model.AddControlPointToC2Curve(curve, entity);
        break;

    case CurveType::Interpolation:
        model.AddControlPointToInterpolationCurve(curve, entity);
        break;
    
    default:
        throw std::runtime_error("Unknown curve type");
    }
}


CurveType GetCurveType(Modeler& model, const Entity entity)
{
    if (model.GetAllC0Curves().contains(entity))
        return CurveType::C0;
    
    if (model.GetAllC2Curves().contains(entity))
        return CurveType::C2;

    if (model.GetAllInterpolationCurves().contains(entity))
        return CurveType::Interpolation;
    
    throw std::runtime_error("Unknown curve type");
}


Entity ModelerMainMenuView::AddCurve(const std::vector<Entity>& entities, CurveType curveType) const
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


Entity ModelerMainMenuView::AddSurface(SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width) const
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


Entity ModelerMainMenuView::AddCylinder(CylinderType cylinderType) const
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


void ModelerMainMenuView::AddRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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


void ModelerMainMenuView::AddColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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


void ModelerMainMenuView::DeleteRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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


void ModelerMainMenuView::DeleteColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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


int ModelerMainMenuView::GetSurfaceRowsCnt(const Entity surface, const SurfaceType surfaceType) const
{
    switch (surfaceType)
    {
        case SurfaceType::C0:
            return model.GetRowsCntOfC0Patches(surface);

        case SurfaceType::C2:
            return model.GetRowsCntOfC2Patches(surface);

        default:
            throw std::runtime_error("Unknown surface type");
    }
}


int ModelerMainMenuView::GetSurfaceColsCnt(Entity surface, SurfaceType surfaceType) const
{
    switch (surfaceType)
    {
        case SurfaceType::C0:
            return model.GetColsOfC0Patches(surface);

        case SurfaceType::C2:
            return model.GetColsCntOfC2Patches(surface);

        default:
            throw std::runtime_error("Unknown surface type");
    }
}


void ModelerMainMenuView::RecalculateSurface(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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


void ModelerMainMenuView::AddRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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


void ModelerMainMenuView::AddColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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


void ModelerMainMenuView::DeleteRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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


void ModelerMainMenuView::DeleteColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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


int ModelerMainMenuView::GetCylinderRowsCnt(Entity cylinder, CylinderType CylinderType) const
{
    switch (CylinderType)
    {
        case CylinderType::C0:
            return model.GetRowsCntOfC0Patches(cylinder);

        case CylinderType::C2:
            return model.GetRowsCntOfC2Cylinder(cylinder);

        default:
            throw std::runtime_error("Unknown surface type");
    }
}


int ModelerMainMenuView::GetCylinderColsCnt(Entity cylinder, CylinderType CylinderType) const
{
    switch (CylinderType)
    {
        case CylinderType::C0:
            return model.GetColsOfC0Patches(cylinder);

        case CylinderType::C2:
            return model.GetColsCntOfC2Cylinder(cylinder);

        default:
            throw std::runtime_error("Unknown surface type");
    }
}


void ModelerMainMenuView::RecalculateCylinder(const Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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
