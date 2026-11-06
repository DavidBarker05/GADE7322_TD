#include "UI/Widgets/GuideWidget.h"

#include "Components/Button.h"
#include "Components/RichTextBlock.h"

bool UGuideWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!TextStyleDataTable || !GuideText || !BackButton) return false;
    // clang-format off
    const FString Str = TEXT(
        "Defend the tower in the middle of the map at all costs\n"
        "The colour of the crystal indicates the main tower's health (Green = Full Health, Red = Low Health)\n"
        "\n"
        "Enemies will move towards your main tower from the ends of any of the three paths\n"
        "Stronger enemies appear with each round\n"
        "Enemy count and strength is based on how well you play\n"
        "Enemies can be defeated to earn money\n"
        "\n"
        "Money can be spent on:\n"
        "- Regaining health for the main tower\n"
        "- Buying defenders to protect your tower\n"
        "- Upgrading those defenders to make them stronger\n"
        "\n"
        "Defenders can only be placed in certain positions (the blue squares)\n"
        "Only one defender can be placed at each spot"
    );
    // clang-format on
    GuideText->SetTextStyleSet(TextStyleDataTable);
    GuideText->SetText(FText::FromString(Str));
    BackButton->OnClicked.AddDynamic(this, &UGuideWidget::ReturnToPreviousScreen);
    return true;
}

void UGuideWidget::ReturnToPreviousScreen() const { OnBackClicked.Broadcast(); }
