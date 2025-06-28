#pragma once

#include "entitiesManager.hpp"


enum class EventType;


class IEventHandler {
public:
    virtual ~IEventHandler() = default;
};


template<typename Comp>
class EventHandler: IEventHandler {
public:
    virtual void HandleEvent(Entity entity, const Comp& component, EventType eventType) = 0;
};
