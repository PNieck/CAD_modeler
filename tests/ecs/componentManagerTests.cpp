#include <gtest/gtest.h>

#include <ecs/componentsManager.hpp>
#include <ecs/entitiesManager.hpp>


class Component1 {
public:
    int val;
};


class Component2 {
public:
    float val;
};


TEST(ComponentsManagerTests, RegisteringComponents) {
    ComponentsManager manager;

    manager.RegisterComponent<Component1>();
    manager.RegisterComponent<Component2>();
}


TEST(ComponentsManagerTests, RegisteringSameComponentsTwoTimes) {
    ComponentsManager manager;

    manager.RegisterComponent<Component1>();
    manager.RegisterComponent<Component2>();
    manager.RegisterComponent<Component1>();
}


TEST(ComponentsManagerTests, AddEntity) {
    ComponentsManager componentsMgr;
    EntitiesManager entitiesMgr;

    componentsMgr.RegisterComponent<Component1>();
    componentsMgr.RegisterComponent<Component2>();

    Entity e1 = entitiesMgr.CreateEntity();
    Component1 c1 { .val = 1 };

    Entity e2 = entitiesMgr.CreateEntity();
    Component2 c2 { .val = 2.0f };

    componentsMgr.AddComponent(e1, c1);
    componentsMgr.AddComponent(e2, c2);

    Component1& r1 = componentsMgr.GetComponent<Component1>(e1);
    Component2& r2 = componentsMgr.GetComponent<Component2>(e2);

    EXPECT_EQ(r1.val, c1.val);
    EXPECT_EQ(r2.val, c2.val);
}


TEST(ComponentsManagerTests, DeleteNonExistingComponent) {
    ComponentsManager manager;
    EntitiesManager entitiesMgr;

    manager.RegisterComponent<Component1>();
    Entity e = entitiesMgr.CreateEntity();

    manager.DeleteComponent<Component1>(e);
}
