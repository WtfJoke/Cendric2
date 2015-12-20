#pragma once

#include <map>

#include "global.h"
#include "Logger.h"
#include "DatabaseManager.h"

#include "Enums/ResourceID.h"
#include "Enums/ErrorID.h"

#include "Structs/ConfigurationData.h"
#include "FileIO/ConfigurationReader.h"
#include "FileIO/ItemReader.h"

#include "GUI/BitmapFont.h"

class ResourceManager {
public:
	ResourceManager();
	~ResourceManager();

	// initializes its maps & data
	void init();

	void deleteResource(ResourceID id);
	// deletes all spell + dynamic tile + enemy resources
	void deleteLevelResources();
	// loads all spell + dynamic tile + enemy resources
	// called before loading a level so that the game won't lag when using a new spell.
	void loadLevelResources();
	void deleteResource(const std::string& filename);

	void setError(ErrorID id, const std::string& description);

	// loads a sound and applies the current configuration to it (sound on/off, volume) and starts it.
	void playSound(sf::Sound& sound, ResourceID id);
	// streams a music and applies the current configuration to it (sound on/off, volume), starts and loops it.
	void playMusic(sf::Music& music, const std::string& filename);

	sf::Texture* getTexture(ResourceID id);
	sf::Texture* getTexture(const std::string& filename);
	sf::SoundBuffer* getSoundBuffer(ResourceID id);
	sf::SoundBuffer* getSoundBuffer(const std::string& filename);
	sf::Font* getFont(ResourceID id);
	sf::Font* getFont(const std::string& filename);
	BitmapFont* getBitmapFont(ResourceID id);
	BitmapFont* getBitmapFont(const std::string& filename);

	const ItemBean* getItemBean(const std::string& id);
	char* getFilename(ResourceID id);
	const std::pair<ErrorID, std::string>* pollError() const;
	ConfigurationData& getConfiguration();

	ResultSet queryDB(std::string query) const;

private:
	std::map<std::string, sf::Texture> m_textures;
	std::map<std::string, sf::SoundBuffer> m_soundBuffers;
	std::map<std::string, BitmapFont> m_bitmapFonts;
	std::map<std::string, sf::Font> m_fonts;
	// a map that is filled with all ResourceTags and the corresponding filenames
	std::map <ResourceID, std::string> m_fileNames;
	// this pair stores resource errors and gets checked in every game loop iteration. mostly and hopefully void.
	std::pair<ErrorID, std::string> m_currentError;
	// configuration can be manipulated by the user
	ConfigurationData m_configuration;
	// all item beans are saved here
	std::map<std::string, ItemBean> m_itemMap;
	DatabaseManager m_databaseManager;
};