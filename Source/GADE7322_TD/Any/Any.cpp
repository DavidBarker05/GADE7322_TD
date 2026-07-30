#include "Any/Any.h"

FAny Bar = 1;

void Foo()
{
    Bar = 2;
    Bar = "Hello World!";
    Bar = 3.14f;

    TArray<FAny> Params = {1, "Hello", 3.14f};
}
