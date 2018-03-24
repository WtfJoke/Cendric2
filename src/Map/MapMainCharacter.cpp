#include "Map/MapMainCharacter.h"
#include "Map/DynamicTiles/DoorMapTile.h"
#include "Screens/MapScreen.h"

MapMainCharacter::MapMainCharacter(Map* map, Screen* screen) : MapMovableGameObject(map) {
	m_screen = screen;
	load();

	m_isAlwaysUpdate = true;
}

MapMainCharacter::~MapMainCharacter() {
}

void MapMainCharacter::setCharacterCore(CharacterCore* core) {
	m_core = core;
	setPosition(m_core->getData().currentMapPosition);
}

void MapMainCharacter::update(const sf::Time& frameTime) {
	m_previousFramePosition = getPosition();
	handleInput();
	sf::Vector2f nextPosition;
	calculateNextPosition(frameTime, nextPosition);
	checkCollisions(nextPosition);
	MovableGameObject::update(frameTime);
	updateAnimation(frameTime);
	handleInteraction();
}

void MapMainCharacter::checkCollisions(const sf::Vector2f& nextPosition) {
	const sf::FloatRect& bb = *getBoundingBox();

	sf::FloatRect nextBoundingBoxX(nextPosition.x, bb.top, bb.width, bb.height);
	sf::FloatRect nextBoundingBoxY(bb.left, nextPosition.y, bb.width, bb.height);
	WorldCollisionQueryRecord rec;

	auto const isMovingY = nextPosition.y != bb.top;
	auto const isMovingX = nextPosition.x != bb.left;
	auto const isMovingRight = nextPosition.x > bb.left;
	auto const isMovingDown = nextPosition.y > bb.top;

	if (!isMovingX && !isMovingY) return;

	// should we use strategy 2: try y direction first, then x direction?
	auto tryYfirst = false;
	rec.boundingBox = nextBoundingBoxX;
	rec.collisionDirection = isMovingRight ? CollisionDirection::Right : CollisionDirection::Left;
	if (m_map->collides(rec)) {
		if (std::abs(nextPosition.x - rec.safeLeft) > 5.f) {
			tryYfirst = true;
		}
	}

	if (!tryYfirst) {
		// check for collision on x axis
		rec.boundingBox = nextBoundingBoxX;
		rec.collisionDirection = isMovingRight ? CollisionDirection::Right : CollisionDirection::Left;
		if (isMovingX && m_map->collides(rec)) {
			setAccelerationX(0.f);
			setVelocityX(0.f);
			if (!rec.noSafePos) {
				setPositionX(rec.safeLeft);
				nextBoundingBoxY.left = rec.safeLeft;
			}
		}
		else {
			nextBoundingBoxY.left = nextPosition.x;
		}

		// check for collision on y axis
		rec.boundingBox = nextBoundingBoxY;
		rec.collisionDirection = isMovingDown ? CollisionDirection::Down : CollisionDirection::Up;

		if (isMovingY && m_map->collides(rec)) {
			setAccelerationY(0.f);
			setVelocityY(0.f);
			if (!rec.noSafePos) {
				setPositionY(rec.safeTop);
			}
		}
	}
	else {
		// check for collision on y axis
		rec.boundingBox = nextBoundingBoxY;
		rec.collisionDirection = isMovingDown ? CollisionDirection::Down : CollisionDirection::Up;

		if (isMovingY && m_map->collides(rec)) {
			setAccelerationY(0.f);
			setVelocityY(0.f);
			if (!rec.noSafePos) {
				setPositionY(rec.safeTop);
				nextBoundingBoxX.top = rec.safeTop;
			}
		}
		else {
			nextBoundingBoxX.top = nextPosition.y;
		}

		// check for collision on x axis
		rec.boundingBox = nextBoundingBoxX;
		rec.collisionDirection = isMovingRight ? CollisionDirection::Right : CollisionDirection::Left;

		if (isMovingX & m_map->collides(rec)) {
			setAccelerationX(0.f);
			setVelocityX(0.f);
			if (!rec.noSafePos) {
				setPositionX(rec.safeLeft);
			}
		}
	}
}

void MapMainCharacter::render(sf::RenderTarget& target) {
	MapMovableGameObject::render(target);
	dynamic_cast<MapScreen*>(m_screen)->renderEquipment(target);
}

void MapMainCharacter::handleInput() {
	sf::Vector2f currentAcceleration;

	if (g_inputController->isDown()) {
		currentAcceleration += sf::Vector2f(0.f, 1.f);
	}
	if (g_inputController->isUp()) {
		currentAcceleration += sf::Vector2f(0.f, -1.f);
	}
	if (g_inputController->isLeft()) {
		currentAcceleration += sf::Vector2f(-1.f, 0.f);
	}
	if (g_inputController->isRight()) {
		currentAcceleration += sf::Vector2f(1.f, 0.f);
	}

	currentAcceleration *= WALK_ACCELERATION;
	setAcceleration(currentAcceleration);
}

void MapMainCharacter::load() {
	g_resourceManager->loadTexture(getSpritePath(), ResourceType::Map);
	const sf::Texture* tex = g_resourceManager->getTexture(getSpritePath());

	setBoundingBox(sf::FloatRect(0.f, 0.f, 18.f, 15.f));
	setSpriteOffset(sf::Vector2f(-16.f, -35.f));

	Animation* walkingAnimationDown = new Animation(sf::seconds(0.15f));
	walkingAnimationDown->setSpriteSheet(tex);
	walkingAnimationDown->addFrame(sf::IntRect(0, 0, 50, 50));
	walkingAnimationDown->addFrame(sf::IntRect(50, 0, 50, 50));
	walkingAnimationDown->addFrame(sf::IntRect(100, 0, 50, 50));
	walkingAnimationDown->addFrame(sf::IntRect(50, 0, 50, 50));

	addAnimation(GameObjectState::Walking_down, walkingAnimationDown);

	Animation* walkingAnimationLeft = new Animation(sf::seconds(0.15f));
	walkingAnimationLeft->setSpriteSheet(tex);
	walkingAnimationLeft->addFrame(sf::IntRect(0, 50, 50, 50));
	walkingAnimationLeft->addFrame(sf::IntRect(50, 50, 50, 50));
	walkingAnimationLeft->addFrame(sf::IntRect(100, 50, 50, 50));
	walkingAnimationLeft->addFrame(sf::IntRect(50, 50, 50, 50));

	addAnimation(GameObjectState::Walking_left, walkingAnimationLeft);

	Animation* walkingAnimationRight = new Animation(sf::seconds(0.15f));
	walkingAnimationRight->setSpriteSheet(tex);
	walkingAnimationRight->addFrame(sf::IntRect(0, 100, 50, 50));
	walkingAnimationRight->addFrame(sf::IntRect(50, 100, 50, 50));
	walkingAnimationRight->addFrame(sf::IntRect(100, 100, 50, 50));
	walkingAnimationRight->addFrame(sf::IntRect(50, 100, 50, 50));

	addAnimation(GameObjectState::Walking_right, walkingAnimationRight);

	Animation* walkingAnimationUp = new Animation(sf::seconds(0.15f));
	walkingAnimationUp->setSpriteSheet(tex);
	walkingAnimationUp->addFrame(sf::IntRect(0, 150, 50, 50));
	walkingAnimationUp->addFrame(sf::IntRect(50, 150, 50, 50));
	walkingAnimationUp->addFrame(sf::IntRect(100, 150, 50, 50));
	walkingAnimationUp->addFrame(sf::IntRect(50, 150, 50, 50));

	addAnimation(GameObjectState::Walking_up, walkingAnimationUp);

	Animation* idleAnimationDown = new Animation();
	idleAnimationDown->setSpriteSheet(tex);
	idleAnimationDown->addFrame(sf::IntRect(50, 0, 50, 50));

	addAnimation(GameObjectState::Idle_down, idleAnimationDown);

	Animation* idleAnimationLeft = new Animation();
	idleAnimationLeft->setSpriteSheet(tex);
	idleAnimationLeft->addFrame(sf::IntRect(50, 50, 50, 50));

	addAnimation(GameObjectState::Idle_left, idleAnimationLeft);

	Animation* idleAnimationRight = new Animation();
	idleAnimationRight->setSpriteSheet(tex);
	idleAnimationRight->addFrame(sf::IntRect(50, 100, 50, 50));

	addAnimation(GameObjectState::Idle_right, idleAnimationRight);

	Animation* idleAnimationUp = new Animation();
	idleAnimationUp->setSpriteSheet(tex);
	idleAnimationUp->addFrame(sf::IntRect(50, 150, 50, 50));

	addAnimation(GameObjectState::Idle_up, idleAnimationUp);

	setDebugBoundingBox(COLOR_WHITE);

	// initial values
	m_state = GameObjectState::Idle_down;
	setCurrentAnimation(getAnimation(m_state), false);
	playCurrentAnimation(true);
}

void MapMainCharacter::calculateUnboundedVelocity(const sf::Time& frameTime, sf::Vector2f& nextVel) const {
	if (getAcceleration().x == 0.f) {
		nextVel.x = 0.f;
	}
	else {
		nextVel.x = (getVelocity().x + getAcceleration().x * frameTime.asSeconds());
	}

	if (getAcceleration().y == 0.f) {
		nextVel.y = 0.f;
	}
	else {
		nextVel.y = (getVelocity().y + getAcceleration().y * frameTime.asSeconds());
	}
}

void MapMainCharacter::boundVelocity(sf::Vector2f& vel) const {
	// check bounds
	if (std::abs(vel.x) > getConfiguredMaxVelocityX() || std::abs(vel.y) > getConfiguredMaxVelocityX()) {
		normalize(vel);
		vel *= getConfiguredMaxVelocityX();
	}
}

const sf::Vector2f& MapMainCharacter::getPreviousPosition() const {
	return m_previousFramePosition;
}

float MapMainCharacter::getConfiguredMaxVelocityX() const {
	return 200.f;
}

GameObjectType MapMainCharacter::getConfiguredType() const {
	return _MapMovableGameObject;
}

std::string MapMainCharacter::getSpritePath() const {
	if (!m_screen->getCharacterCore()->isConditionFulfilled("boss", "BossVelius")) {
		return "res/texture/cendric/spritesheet_cendric_map.png";
	}
	return "res/texture/cendric/spritesheet_cendric_map_end.png";
}