#include "UI/ControlsWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/PauseScreenWidget.h"

bool UControlsWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!ControlsText || !BackButton) return false;
    // clang-format off
    FString Str = TEXT(
        "WASD = Move\n"
        "QE = Rotate\n"
        "Left Click = Select\n"
        "Right Click = Deselect\n"
        "Mouse Scroll = Zoom\n"
        "F = Focus\n"
        "Escape = Pause"
    );
    // clang-format on
#if WITH_EDITOR
    Str.Append(TEXT("\nP = Editor Pause"));
#endif
    ControlsText->SetText(FText::FromString(Str));
    BackButton->OnClicked.AddDynamic(this, &UControlsWidget::ReturnToPauseScreen);
    return true;
}

void UControlsWidget::ReturnToPauseScreen() const
{
    if (UPauseScreenWidget* OwningPauseScreen = GetTypedOuter<UPauseScreenWidget>()) OwningPauseScreen->ShowMainPanel();
}
