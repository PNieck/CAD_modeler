#include <ecs/idManager.hpp>


Id IdManager::CreateNewId()
{
    if (!priority_queue.empty()) {
        Id result =  priority_queue.top();
        priority_queue.pop();

        return result;
    }

    return biggestId++;
}


void IdManager::DestroyId(Id id)
{
    if (id == biggestId - 1) {
        biggestId--;
        return;
    }

    priority_queue.push(id);
}
