#include <CAD_modeler/model/components/millingWarningsRepo.hpp>


void MillingWarningsRepo::AddWarning(int commandId, WarningType warningType)
{
    const auto idAndWarnings = warnings.find(commandId);

    if (idAndWarnings == warnings.end()) {
        warnings.insert(std::pair(commandId, warningType));
        return;
    }

    if (idAndWarnings->second & warningType)
        return;

    idAndWarnings->second |= warningType;
}
