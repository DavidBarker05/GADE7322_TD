#include "Any/Any.h"



void Foo()
{
    FAny Bar = 1;
    Bar = 2;
    Bar = "Hello World!";
    Bar = 3.14f;

    TArray<FAny> Params = {1, "Hello", 3.14f};
}
