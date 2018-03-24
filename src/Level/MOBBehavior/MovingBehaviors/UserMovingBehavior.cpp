#include "Level/MOBBehavior/MovingBehaviors/UserMovingBehavior.h"
#include "Level/Level.h"
#include "Level/LevelMainCharacter.h"
#include "Level/DynamicTiles/LadderTile.h"

const sf::Time UserMovingBehavior::JUMP_GRACE_TIME = sf::milliseconds(100);
const sf::Time UserMovingBehavior::CLIMB_STEP_TIME = sf::milliseconds(150);
const float UserMovingBehavior::MAX_JUMP_VELOCITY = 600.f;
const float UserMovingBehavior::MIN_JUMP_VELOCITY = 200.f;

UserMovingBehavior::UserMovingBehavior(LevelMainCharacter* mainChar) : MovingBehavior(mainChar) {
	m_mainChar = mainChar;
}

void UserMovingBehavior::update(const sf::Time& frameTime) {
	updateTime(m_jumpGraceTime, frameTime);
	bool wasGrounded = m_isGrounded;
	MovingBehavior::update(frameTime);
	if (wasGrounded && !m_isGrounded) {
		m_jumpGraceTime = JUMP_GRACE_TIME;
	}
	if (m_isJumpLocked && m_isGrounded) {
		m_isJumpLocked = false;
	}
	handleClimbing(frameTime);
}

void UserMovingBehavior::checkCollisions(const sf::Vector2f& nextPosition) {
	checkXYDirection(nextPosition, m_isCollidingX, m_isCollidingY);
}

void UserMovingBehavior::stopClimbing() {
	m_climbStepTime = sf::Time::Zero;
	m_isClimbing = false;
	m_currentLadder = nullptr;
}

void UserMovingBehavior::startClimbing(GameObject* ladder, float yPos) {
	m_isClimbing = true;
	m_isGrounded = false;
	m_currentLadder = ladder;
	m_mob->setPositionY(yPos);
	m_mob->setAccelerationY(0.f);
	m_mob->setVelocityY(0.f);
	g_inputController->lockAction();
}

void UserMovingBehavior::handleClimbing(const sf::Time& frameTime) {
	if (!isReady()) return;
	if (m_isClimbing) {
		if (g_inputController->isKeyJustPressed(Key::Jump)) {
			stopClimbing();
			g_inputController->lockAction();
			return;
		}

		bool up = g_inputController->isUp();
		bool down = g_inputController->isDown();
		// handle climb step timing
		if (up || down) {
			m_climbStepTime += frameTime;
			if (m_climbStepTime >= CLIMB_STEP_TIME) {
				m_climbStepTime = sf::Time::Zero;
				float diffY = static_cast<float>(up ? -LadderTile::LADDER_STEP : LadderTile::LADDER_STEP);
				
				WorldCollisionQueryRecord rec;
				rec.collisionDirection = up ? CollisionDirection::Up : CollisionDirection::Down;
				rec.boundingBox = *(m_mob->getBoundingBox());
				rec.boundingBox.top += diffY;
				rec.ignoreDynamicTiles = m_mob->isIgnoreDynamicTiles();
				rec.ignoreOnewayTiles = true;

				if (m_mob->getLevel()->collides(rec)) {
					if (!up != m_mob->isUpsideDown()) {
						stopClimbing();
					}
					return;
				}

				m_mob->setPositionY(rec.boundingBox.top);
				m_isClimbingStep1 = !m_isClimbingStep1;
			}
		}
		else {
			m_climbStepTime = sf::Time::Zero;
		}

		// check if we're still on our ladder
		if (!fastIntersect(*m_mob->getBoundingBox(), *m_currentLadder->getBoundingBox())) {
			stopClimbing();
			checkLadders();
		}
	}
	else {
		// check if a climbing just started
		if (!(g_inputController->isJustUp() || g_inputController->isJustDown())) return;

		checkLadders();
	}
}

void UserMovingBehavior::checkLadders() {
	for (auto& go : *(m_mob->getScreen()->getObjects(_DynamicTile))) {
		LadderTile* tile = dynamic_cast<LadderTile*>(go);
		if (tile && tile->isViewable() &&
			fastIntersect(*tile->getBoundingBox(), *m_mob->getBoundingBox())) {

			float climbingY = tile->getClimbingPositionY(m_mob);

			WorldCollisionQueryRecord rec;
			rec.boundingBox = *(m_mob->getBoundingBox());
			rec.boundingBox.top = climbingY;
			rec.ignoreDynamicTiles = m_mob->isIgnoreDynamicTiles();

			if (m_mob->getLevel()->collides(rec)) {
				g_logger->logWarning("UserMovingBehavior", "Cannot start climbing on this ladder, it would stuck the mob.");
				return;
			}

			startClimbing(tile, climbingY);
		}
	}
}

void UserMovingBehavior::handleDefaultAcceleration() {
	if (m_isClimbing) {
		float newAccelerationX = m_mob->getAcceleration().x;
		m_mob->setAcceleration(sf::Vector2f(newAccelerationX, 0.f));
	}
	else {
		MovingBehavior::handleDefaultAcceleration();
	}
}

void UserMovingBehavior::handleMovementInput() {
	float newAccelerationX = m_mainChar->getAcceleration().x;
	if (!m_mainChar->isFeared()) {

		if (g_inputController->isLeft()) {
			if (!m_isClimbing) m_nextIsFacingRight = false;
			newAccelerationX -= m_isClimbing ? m_walkAcceleration * 0.2f : m_walkAcceleration;
		}
		if (g_inputController->isRight()) {
			if (!m_isClimbing) m_nextIsFacingRight = true;
			newAccelerationX += m_isClimbing ? m_walkAcceleration * 0.2f : m_walkAcceleration;
		}
		if (!m_isClimbing) {
			if (g_inputController->isKeyJustPressed(Key::Jump) && (m_isGrounded || m_jumpGraceTime > sf::Time::Zero)) {
				m_jumpGraceTime = sf::Time::Zero;
				m_mainChar->setVelocityY(m_isFlippedGravity ? MAX_JUMP_VELOCITY : -MAX_JUMP_VELOCITY);
			}
			else if (!m_isJumpLocked && ((m_mainChar->getVelocity().y > MIN_JUMP_VELOCITY && m_isFlippedGravity) ||
				(m_mainChar->getVelocity().y < -MIN_JUMP_VELOCITY && !m_isFlippedGravity)) && !g_inputController->isKeyActive(Key::Jump)) {
				m_mainChar->setVelocityY(m_isFlippedGravity ? MIN_JUMP_VELOCITY : -MIN_JUMP_VELOCITY);
			}
		}
	}

	m_mainChar->setAcceleration(sf::Vector2f(newAccelerationX, m_isClimbing ? 0.f : (m_isFlippedGravity ? -m_gravity : m_gravity)));
}

void UserMovingBehavior::setEnabled(bool enabled) {
	MovingBehavior::setEnabled(enabled);
	if (!enabled) {
		m_mainChar->setAcceleration(sf::Vector2f(0.f, 0.f));
		m_mainChar->setVelocity(sf::Vector2f(0.f, 0.f));
	}
}

void UserMovingBehavior::flipGravity() {
	MovingBehavior::flipGravity();
	// reset the gravity speed
	if (std::abs(m_mob->getVelocity().y) > MIN_JUMP_VELOCITY) {
		m_mainChar->setVelocityY(m_isFlippedGravity ? MIN_JUMP_VELOCITY : -MIN_JUMP_VELOCITY);
	}
}

void UserMovingBehavior::stopAll() {
	m_mob->setVelocity(sf::Vector2f(0.f, 0.f));
	m_mob->setAcceleration(sf::Vector2f(0.f, 0.f));
}

void UserMovingBehavior::setJumpLock() {
	m_isJumpLocked = true;
}

void UserMovingBehavior::updateAnimation(const sf::Time& frameTime) {
	// calculate new game state and set animation.

	GameObjectState newState = GameObjectState::Idle;
	if (m_mainChar->isDead()) {
		newState = GameObjectState::Dead;
	}
	else if (m_isClimbing) {
		newState = m_isClimbingStep1 ? GameObjectState::Climbing_1 : GameObjectState::Climbing_2;
	}
	else if (m_fightAnimationTime > sf::Time::Zero) {
		newState = m_fightAnimationState;
	}
	else if (!m_isGrounded) {
		newState = GameObjectState::Jumping;
	}
	else if (std::abs(m_mainChar->getVelocity().x) > 20.0f) {
		newState = GameObjectState::Walking;
	}

	// only update animation if we need to
	if (m_mainChar->getState() != newState || (m_nextIsFacingRight != m_isFacingRight)) {
		m_isFacingRight = m_nextIsFacingRight;
		m_mainChar->setState(newState, false);
		if (newState != GameObjectState::Dead)
			m_mainChar->setCurrentAnimation(m_mainChar->getAnimation(m_mainChar->getState()), !m_isFacingRight);
		else
			m_mainChar->loopCurrentAnimation(false);
	}
}

