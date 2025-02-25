#pragma once


#include "../../model/modeler.hpp"
#include "../objectTypes/curvesTypes.hpp"


void AddControlPointToCurve(Modeler& model, Entity curve, Entity entity, CurveType curveType);


CurveType GetCurveType(Modeler& model, Entity entity);
