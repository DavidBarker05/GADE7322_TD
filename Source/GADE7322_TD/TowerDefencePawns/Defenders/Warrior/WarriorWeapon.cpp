#include "TowerDefencePawns/Defenders/Warrior/WarriorWeapon.h"

#include "TowerDefencePawns/Components/OutlineComponent.h"

AWarriorWeapon::AWarriorWeapon()
{
    OutlineComponent = CreateDefaultSubobject<UOutlineComponent>(TEXT("Outline Component"));
}
