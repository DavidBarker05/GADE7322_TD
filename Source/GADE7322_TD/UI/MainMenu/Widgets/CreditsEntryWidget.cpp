#include "UI/MainMenu/Widgets/CreditsEntryWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/KismetSystemLibrary.h"

bool UCreditsEntryWidget::Initialize()
{
    if (!Super::Initialize()) return false;
    if (!(VerticalBox && NameText && CreatorText && LinkButton && LinkButtonText)) return false;
    LinkButton->OnClicked.AddDynamic(this, &UCreditsEntryWidget::OpenLink);
    NameText->SetText(FText::FromString(EntryName));
    CreatorText->SetText(FText::FromString(TEXT("By: ") + EntryCreator));
    LinkButtonText->SetText(FText::FromString(URL));
    return true;
}

void UCreditsEntryWidget::OpenLink()
{
    if (URL.IsEmpty()) return;
    UKismetSystemLibrary::LaunchURL(URL);
}
