#include "UI/TowerDefence/Widgets/DefenderInfoEntryWidget.h"

#include "Components/RichTextBlock.h"
#include "TowerDefencePawns/Defenders/Defender.h"

bool UDefenderInfoEntryWidget::Initialize() { return Super::Initialize() && DefenderInfoText; }

void UDefenderInfoEntryWidget::Setup(TSubclassOf<ADefender> InDefenderClass) const
{
    if (!IsValid(InDefenderClass)) return;
    TOptional<FName> BaseDefenderNameOpt = ADefender::GetDefenderName(InDefenderClass, 0);
    TOptional<FString> BaseDefenderInfoOpt = ADefender::GetDefenderInfo(InDefenderClass, 0);
    TOptional<FName> FirstUpgradeDefenderNameOpt = ADefender::GetDefenderName(InDefenderClass, 1);
    TOptional<FString> FirstUpgradeDefenderInfoOpt = ADefender::GetDefenderInfo(InDefenderClass, 1);
    TOptional<FName> SecondUpgradeDefenderNameOpt = ADefender::GetDefenderName(InDefenderClass, 2);
    TOptional<FString> SecondUpgradeDefenderInfoOpt = ADefender::GetDefenderInfo(InDefenderClass, 2);
    if (!(BaseDefenderNameOpt && BaseDefenderInfoOpt && FirstUpgradeDefenderNameOpt && FirstUpgradeDefenderInfoOpt &&
          SecondUpgradeDefenderNameOpt && SecondUpgradeDefenderInfoOpt))
        return;
    FName BaseDefenderName = BaseDefenderNameOpt.GetValue(),
          FirstUpgradeDefenderName = FirstUpgradeDefenderNameOpt.GetValue(),
          SecondUpgradeDefenderName = SecondUpgradeDefenderNameOpt.GetValue();
    FString BaseDefenderInfo = BaseDefenderInfoOpt.GetValue(),
            FirstUpgradeDefenderInfo = FirstUpgradeDefenderInfoOpt.GetValue(),
            SecondUpgradeDefenderInfo = SecondUpgradeDefenderInfoOpt.GetValue();
    // clang-format off
    DefenderInfoText->SetText(FText::FromString(
        BaseDefenderName.ToString() + TEXT(":\n") +
        TEXT("Base (\"") + BaseDefenderName.ToString() + TEXT("\"): ") + BaseDefenderInfo + TEXT("\n\n") +
        TEXT("First Upgrade (\"") + FirstUpgradeDefenderName.ToString() + TEXT("\"): ") + FirstUpgradeDefenderInfo + TEXT("\n\n") +
        TEXT("Second Upgrade (\"") + SecondUpgradeDefenderName.ToString() + TEXT("\"): ") + SecondUpgradeDefenderInfo +
        TEXT("\n\n\n\n")
    ));
    // clang-format on
}
