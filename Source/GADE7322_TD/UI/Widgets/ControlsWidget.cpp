#include "UI/Widgets/ControlsWidget.h"

#include "Components/Button.h"
#include "Components/RichTextBlock.h"
#include "UI/TowerDefence/Widgets/PauseScreenWidget.h"

bool UControlsWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!TextStyleDataTable || !ControlsText || !BackButton) return false;
    // clang-format off
    FString Str = TEXT(
        "<LargeText>CONTROLS</>\n"
        "\n"
        "WASD = Move\n"
        "QE = Rotate\n"
        "Left Click = Select\n"
        "Right Click = Deselect\n"
        "Mouse Scroll = Zoom\n"
        "F = Focus\n"
        "Escape = Pause"
    );
    // clang-format on
    // ^ Toggled clang-format so that the strings looked nicer
#if WITH_EDITOR
    Str.Append(TEXT("\nP = Editor Pause"));
#endif
    ControlsText->SetTextStyleSet(TextStyleDataTable);
    ControlsText->SetText(FText::FromString(Str));
    BackButton->OnClicked.AddDynamic(this, &UControlsWidget::ReturnToPreviousScreen);
    return true;
}

void UControlsWidget::ReturnToPreviousScreen() const { OnBackClicked.Broadcast(); }
