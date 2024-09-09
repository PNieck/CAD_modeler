#include <CAD_modeler/model/components/gregoryPatchParameters.hpp>

#include <stdexcept>
#include <cassert>


Position GregoryPatchParameters::GetOuterPoint(int row, int col) const
{
    switch (row)
    {
    case 0:
        assert(col >=0 && col <=3);
        return outerPoints[col];
        
    case 1:
    case 2:
        assert(col == 0 || col == 3);
        return outerPoints[4 + col/3];
    
    case 3:
        assert(col >=0 && col <=3);
        return outerPoints[8 + col];
    
    default:
        throw std::invalid_argument("Invalid row");
    }
}


void GregoryPatchParameters::SetOuterPoint(const Position& pos, int row, int col)
{
    switch (row)
    {
    case 0:
        assert(col >=0 && col <=3);
        outerPoints[col] = pos;
        break;
        
    case 1:
    case 2:
        assert(col == 0 || col == 3);
        outerPoints[4 + col/3] = pos;
    
    case 3:
        assert(col >=0 && col <=3);
        outerPoints[8 + col] = pos;
    
    default:
        throw std::invalid_argument("Invalid row");
    }
}


Position GregoryPatchParameters::GetInnerPoint(int neighbourRow, int neighbourCol) const
{
    switch (neighbourRow)
    {
    case 0:
        assert(neighbourCol == 1 || neighbourCol == 2);
        return innerPoints[neighbourCol-1];
    
    case 1:
        assert(neighbourCol == 0 || neighbourCol == 3);
        return innerPoints[neighbourCol == 0 ? 7 : 2];

    case 2:
        assert(neighbourCol == 0 || neighbourCol == 3);
        return innerPoints[neighbourCol == 0 ? 6 : 3];

    case 3:
        assert(neighbourCol == 1 || neighbourCol == 2);
        return innerPoints[neighbourCol == 1 ? 5 : 4];

    default:
        throw std::invalid_argument("Invalid row");
    }
}


void GregoryPatchParameters::SetInnerPoint(const Position& pos, int neighbourRow, int neighbourCol)
{
    switch (neighbourRow)
    {
    case 0:
        assert(neighbourCol == 1 || neighbourCol == 2);
        innerPoints[neighbourCol-1] = pos;
        break;
    
    case 1:
        assert(neighbourCol == 0 || neighbourCol == 3);
        innerPoints[neighbourCol == 0 ? 7 : 2] = pos;
        break;

    case 2:
        assert(neighbourCol == 0 || neighbourCol == 3);
        innerPoints[neighbourCol == 0 ? 6 : 3] = pos;
        break;

    case 3:
        assert(neighbourCol == 1 || neighbourCol == 2);
        innerPoints[neighbourCol == 1 ? 5 : 4] = pos;
        break;

    default:
        throw std::invalid_argument("Invalid row");
    }
}
