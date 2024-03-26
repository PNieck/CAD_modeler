#ifndef ID_MANGER_H
#define ID_MANGER_H

#include <cstdint>
#include <queue>


using Id = uint32_t;

class IdManager {
public:
    IdManager() = default;

    Id CreateNewId();

    void DestroyId(Id id);

private:
    std::priority_queue<Id, std::vector<Id>, std::greater<Id>> priority_queue;
    Id biggestId = 0;
};


#endif
