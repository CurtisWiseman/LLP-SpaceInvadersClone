#pragma once
#include <fmod.hpp>


class SoundSystem
{
public:
	typedef FMOD::Sound* SoundClass;

	FMOD::System* m_pSystem;
	SoundSystem();
	void createSound(SoundClass *pSound, const char* pFile);
	void playSound(SoundClass pSound, bool bLoop, FMOD::Channel** pChannel);
	void releaseSound(SoundClass pSound);
};