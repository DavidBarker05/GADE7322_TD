#include "UI/TowerDefence/Widgets/HealthBarWidget.h"
#include "Components/ProgressBar.h"

bool UHealthBarWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    return static_cast<bool>(HealthBar);
}
