#include <CAD_modeler/window.hpp>
#include <CAD_modeler/controllers/mainController.hpp>


int main()
{
    Window window(600, 400, "TestWindow");

    window.RunMessageLoop();

    return 0;
}
