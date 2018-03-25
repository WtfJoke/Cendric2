#pragma once

#include "global.h"
#include "Controller/MouseController.h"
#include "Controller/KeyboardController.h"
#include "Controller/GamepadController.h"

class InputController final : public MouseController, public KeyboardController, public GamepadController {
public:
	InputController();
	~InputController();

	void update(const sf::Time& frameTime) override;

	bool isKeyActive(Key key) const;
	bool isKeyJustPressed(Key key) const;

	// helper methods as their combination is often used
	bool isSelected() const;
	bool isLeft() const;
	bool isRight() const;
	bool isUp() const;
	bool isDown() const;
	bool isAttacking() const;
	bool isJustDown() const;
	bool isJustUp() const;

private:
	void init();

	std::map<Key, bool> m_keyActiveMap;
	std::map<Key, bool> m_keyJustPressedMap;

	bool isKeyPressed(Key key) const;
};