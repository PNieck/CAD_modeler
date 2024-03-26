#include <gtest/gtest.h>

#include <CAD_modeler/model/components/componentsManager.hpp>
#include <CAD_modeler/model/entitiesManager.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/scale.hpp>


TEST(ComponentsManagerTests, RegisteringComponents) {
    ComponentsManager manager;

    manager.RegisterComponent<Position>();
    manager.RegisterComponent<Scale>();
}


TEST(ComponentsManagerTests, RegisteringSameComponentsTwoTimes) {
    ComponentsManager manager;

    manager.RegisterComponent<Position>();
    manager.RegisterComponent<Scale>();
    manager.RegisterComponent<Position>();
}


TEST(ComponentsManagerTests, AddEntity) {
    ComponentsManager componentsMgr;
    EntitiesManager entitiesMgr;

    componentsMgr.RegisterComponent<Position>();
    componentsMgr.RegisterComponent<Scale>();

    Entity e1 = entitiesMgr.CreateEntity();
    Position p1(1.0f);

    Entity e2 = entitiesMgr.CreateEntity();
    Position p2(2.0f);

    componentsMgr.AddComponent(e1, p1);
    componentsMgr.AddComponent(e2, p2);

    Position& r1 = componentsMgr.GetComponent<Position>(e1);
    Position& r2 = componentsMgr.GetComponent<Position>(e2);

    EXPECT_EQ(r1.vec, p1.vec);
    EXPECT_EQ(r2.vec, p2.vec);
}
