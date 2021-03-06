#pragma once

#include "global.h"
#include "Spells/SpellCreator.h"

#include "Spells/GhostFormSpell.h"
#include "Level/LevelEquipment.h"

// a class that creates ghost form spells
class GhostFormSpellCreator final : public SpellCreator {
public:
	GhostFormSpellCreator(const SpellData& spellData, LevelMovableGameObject* owner);
	std::string getStrengthModifierName() const override;
	int getStrengthModifierValue() const override;

	void execExecuteSpell(const sf::Vector2f& target) override;

protected:
	void addStrengthModifier(int level) override;
	void addDurationModifier(int level) override;
};