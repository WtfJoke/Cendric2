#pragma once

#include "global.h"
#include "Level/Enemy.h"
#include "Level/Level.h"
#include "Spells/SpellManager.h"
#include "Screens/Screen.h"

class ZeffBoss : public Enemy {
public:
	ZeffBoss(const Level* level, Screen* screen);
	~ZeffBoss() {}

	void loadAnimation() override;
	void render(sf::RenderTarget& target) override;
	void update(const sf::Time& frameTime) override;

	int getMentalStrength() const override;
	sf::Time getConfiguredWaitingTime() const override;
	void setDead() override;

	void insertDefaultLoot(std::map<std::string, int>& loot, int& gold) const override;
	void insertRespawnLoot(std::map<std::string, int>& loot, int& gold) const override;

protected:
	std::string getSpritePath() const override;
	std::string getDeathSoundPath() const override;
	MovingBehavior* createMovingBehavior(bool asAlly) override;
	AttackingBehavior* createAttackingBehavior(bool asAlly) override;
	void handleAttackInput();
	// loads attributes and adds immune spells + enemies. all attributes are set to zero before that call. default does nothing.
	void loadAttributes() override;
	// loads spells and adds them to the spell manager. default does nothing.
	void loadSpells() override;
	// particles & death
	void loadParticleSystem();
	void updateParticleSystemPosition();

	sf::Sound m_sound;
	sf::Time m_fadingTime = sf::seconds(2.f);
	sf::Time m_particleTime = sf::seconds(2.f);

	std::unique_ptr<particles::TextureParticleSystem> m_ps = nullptr;
	particles::DiskPositionGenerator* m_posGenerator = nullptr;
};