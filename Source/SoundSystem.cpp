#include "SoundSystem.h"


SoundSystem::SoundSystem()
{
	//Initialise the FMOD sound system with 16 channels.
	if (FMOD::System_Create(&m_pSystem) != FMOD_OK)
	{
		return;
	}

	int driver_count = 0;
	m_pSystem->getNumDrivers(&driver_count);

	if (driver_count == 0)
	{
		return;
	}

	m_pSystem->init(16, FMOD_INIT_NORMAL, 0);
}


void SoundSystem::createSound(SoundClass *pSound, const char* pFile)
{
	//Create a new sound
	m_pSystem->createSound(pFile, FMOD_CREATESAMPLE, 0, pSound);
}


void SoundSystem::playSound(SoundClass pSound, bool bLoop, FMOD::Channel** pChannel)
{
	//Play a sound either with looping or without.
	if (bLoop == false)
	{
		pSound->setMode(FMOD_LOOP_OFF);
	}
	else
	{
		pSound->setMode(FMOD_LOOP_BIDI);
		pSound->setLoopCount(-1);
	}

	//Also pass in an FMOD channel pointer to keep track of the sound if it is to loop.
	m_pSystem->playSound(pSound, 0, false, pChannel);
}


void SoundSystem::releaseSound(SoundClass pSound)
{
	//Release a sound to free up channels.
	pSound->release();
}