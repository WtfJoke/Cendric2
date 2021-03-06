#pragma once

#include "global.h"
#include "Screens/Screen.h"

#include "Structs/MapData.h"
#include "Structs/NPCData.h"
#include "Enums/MapDynamicTileID.h"

#include "Map/DynamicTiles/CookingTile.h"

class MapScreen;

// static helper class used by the map to load npcs
class MapLoader final {
public:
	static void loadAfterMainChar(MapData& data, MapScreen* screen);
	static void loadForRenderTexture(MapData& data, MapScreen* screen);

private:
	// loads dynamic tiles out of map data and adds them directly to the screen.
	static void loadDynamicTiles(MapData& data, MapScreen* screen);
	// loads npcs out of map data and adds them directly to the screen
	static void loadNpcs(MapData& data, MapScreen* screen);
	// loads lights out of map data and adds them directly to the screen
	static void loadLights(MapData& data, MapScreen* screen);
	// loads triggers out of map data and adds them directly to the screen
	static void loadTriggers(MapData& data, MapScreen* screen);
};