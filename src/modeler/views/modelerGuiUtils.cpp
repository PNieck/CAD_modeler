#include <CAD_modeler/views/modelerGuiView.hpp>


Entity ModelerGuiView::AddCurve(const std::vector<Entity>& entities, CurveType curveType) const
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


Entity ModelerGuiView::AddSurface(SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width) const
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


Entity ModelerGuiView::AddCylinder(CylinderType cylinderType) const
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


void ModelerGuiView::AddRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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


void ModelerGuiView::AddColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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


void ModelerGuiView::DeleteRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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


void ModelerGuiView::DeleteColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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


int ModelerGuiView::GetSurfaceRowsCnt(const Entity surface, const SurfaceType surfaceType) const
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


int ModelerGuiView::GetSurfaceColsCnt(Entity surface, SurfaceType surfaceType) const
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


void ModelerGuiView::RecalculateSurface(Entity surface, SurfaceType surfaceType, const alg::Vec3 &dir, float length, float width)
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

void ModelerGuiView::AddControlPointToCurve(Entity curve, Entity entity, CurveType curveType) const
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

void ModelerGuiView::AddRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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


void ModelerGuiView::AddColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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


void ModelerGuiView::DeleteRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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


void ModelerGuiView::DeleteColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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


int ModelerGuiView::GetCylinderRowsCnt(Entity cylinder, CylinderType CylinderType) const
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


int ModelerGuiView::GetCylinderColsCnt(Entity cylinder, CylinderType CylinderType) const
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


void ModelerGuiView::RecalculateCylinder(const Entity cylinder, CylinderType cylinderType, const alg::Vec3 &dir, float radius)
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
