#pragma once

#include <vector>
#include <map>

#include "global.h"
#include "Screens/Screen.h"
#include "Level/LevelMainCharacter.h"
#include "Level/Level.h"
#include "Level/LevelEquipment.h"

#include "Structs/LevelEquipmentData.h"

// helper class used by the game screan to load the main character including his weapon / equipment.
class LevelMainCharacterLoader {
public:
	// loads the main character and adds it directly to the screen
	LevelMainCharacter* loadMainCharacter(Screen* screen, Level* level) const;
	// loads level equipment adds it directly to the screen
	void loadEquipment(Screen* screen) const;
};