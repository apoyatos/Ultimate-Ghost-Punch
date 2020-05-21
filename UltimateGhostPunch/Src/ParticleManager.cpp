#include "ParticleManager.h"
#include <ComponentRegister.h>
#include <GameObject.h>
#include <ParticleEmitter.h>

#include "PlayerState.h"

REGISTER_FACTORY(ParticleManager);

ParticleManager::ParticleManager(GameObject* gameObject) : UserComponent(gameObject), floorDust(nullptr), jumpDust(nullptr), landDust(nullptr),
bloodSplash(nullptr), blockSparks(nullptr), stunSparks(nullptr),
spectre(nullptr), spectreSplash(nullptr), playerState(nullptr),
stunDelay(0.0f), stunTimer(0.0f)
{

}

ParticleManager::~ParticleManager()
{

}

void ParticleManager::start()
{
	// PlayerState for info
	playerState = gameObject->getComponent<PlayerState>();
	checkNull(playerState);

	/* FLOOR DUST */
	createParticle(&floorDust, "FloorDust", Vector3::NEGATIVE_UP * 2.5f);

	/* JUMP DUST */
	createParticle(&jumpDust, "JumpDust", Vector3::NEGATIVE_UP * 2.5f);

	/* LAND DUST */
	createParticle(&landDust, "LandDust", Vector3::NEGATIVE_UP * 2.5f);

	/* BLOOD SPLASH */
	createParticle(&bloodSplash, "BloodSplash");

	/* BLOCK SPARKS */
	createParticle(&blockSparks, "BlockSparks");

	/* STUN SPARKS */
	createParticle(&stunSparks, "StunSparks", Vector3::UP * 2.5f);
	stunDelay = 0.5f;

	/* SPECTRE */
	createParticle(&spectre, "Spectre");

	/* SPECTRE SPLASH */
	createParticle(&spectreSplash, "SpectreSplash");

}

void ParticleManager::preUpdate(float deltaTime)
{
	/* FLOOR DUST */
	manageFloorDust();

	/* JUMP DUST */
	manageJumpDust();

	/* LAND DUST */
	manageLandDust();

	/* BLOOD SPLASH */
	manageBloodSplash();

	/* BLOCK SPARKS */
	manageBlockSparks();

	/* STUN SPARKS */
	manageStunSparks(deltaTime);

	/* SPECTRE */
	manageSpectre();

	/* SPECTRE SPLASH */
	manageSpectreSplash();
}

void ParticleManager::createParticle(ParticleEmitter** emitter, const std::string& particleName, const Vector3& position)
{
	if (*emitter != nullptr) return;

	GameObject* particlesObject = instantiate("ParticleEmitter");
	if (particlesObject == nullptr)
		return;

	// Add child
	gameObject->addChild(particlesObject);

	*emitter = particlesObject->getComponent<ParticleEmitter>();
	if (*emitter == nullptr)
		return;

	if (particlesObject->transform != nullptr)
		particlesObject->transform->setPosition(position);
	(*emitter)->newEmitter(particleName);
}

void ParticleManager::manageFloorDust()
{
	if (floorDust == nullptr || playerState == nullptr) return;

	if (playerState->isGrounded() && playerState->isMoving() && playerState->canMove())
		floorDust->start();
	else
		floorDust->stop();
}

void ParticleManager::manageJumpDust()
{
	if (jumpDust == nullptr || playerState == nullptr) return;

	if ((playerState->isGrounded() && playerState->isJumping()) || playerState->hasJumped())
		jumpDust->start();
	else
		jumpDust->stop();
}

void ParticleManager::manageLandDust()
{
	if (landDust == nullptr || playerState == nullptr) return;

	if (playerState->hasLanded() && playerState->canMove())
		landDust->start();
	else
		landDust->stop();
}

void ParticleManager::manageBloodSplash()
{
	if (bloodSplash == nullptr || playerState == nullptr) return;

	if (playerState->isHurt())
		bloodSplash->start();
	else
		bloodSplash->stop();
}

void ParticleManager::manageBlockSparks()
{
	if (blockSparks == nullptr || playerState == nullptr) return;

	if (playerState->isBlocking() && playerState->hasBlocked())
		blockSparks->start();
	else
		blockSparks->stop();
}

void ParticleManager::manageStunSparks(float deltaTime)
{
	if (stunSparks == nullptr || playerState == nullptr) return;

	if (playerState->isStunned()) {
		stunTimer -= deltaTime;
		if (stunTimer <= 0.0f)
			stunSparks->start();
	}
	else {
		stunSparks->stop();
		stunTimer = stunDelay;
	}
}

void ParticleManager::manageSpectre()
{
	if (spectre == nullptr || playerState == nullptr) return;

	if (playerState->canGhostMove())
		spectre->start();
	else
		spectre->stop();
}

void ParticleManager::manageSpectreSplash()
{
	if (spectreSplash == nullptr || playerState == nullptr) return;

	if (playerState->hasPunchSucceeded())
		spectreSplash->start();
	else
		spectreSplash->stop();
}
