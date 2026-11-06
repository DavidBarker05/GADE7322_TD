#include "UI/MainMenu/Widgets/CreditsWidget.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"

bool UCreditsWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!ScrollBox || !VerticalBox || !BackButton) return false;
    BackButton->OnClicked.AddDynamic(this, &UCreditsWidget::ReturnToPreviousScreen);
    return true;
}

void UCreditsWidget::ReturnToPreviousScreen() const { OnBackClicked.Broadcast(); }
