#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

namespace PM3D {
	class SoundClass
	{
	private:

		struct WaveHeaderType
		{
			char chunkId[4];
			unsigned long chunkSize;
			char format[4];
			char subChunkId[4];
			unsigned long subChunkSize;
			unsigned short audioFormat;
			unsigned short numChannels;
			unsigned long sampleRate;
			unsigned long bytesPerSecond;
			unsigned short blockAlign;
			unsigned short bitsPerSample;
			char dataChunkId[4];
			unsigned long dataSize;
		};

	public:
		SoundClass();
		SoundClass(const SoundClass&);
		~SoundClass();

		bool Initialize(HWND);
		void Shutdown();
		bool PlayWaveFile(IDirectSoundBuffer8* secondaryBuffer);

		bool fadeOut1();
		bool fadeOut2();

		bool fadeIn1();
		bool fadeIn2();

		bool DiminuerSon1(IDirectSoundBuffer8* secondaryBuffer, int* volume, bool* fadeout);
		bool DiminuerSon2(IDirectSoundBuffer8* secondaryBuffer, int* volume, bool* fadeout);
		bool AugmenterSon1(IDirectSoundBuffer8* secondaryBuffer, int* volume, bool* fadein);
		bool AugmenterSon2(IDirectSoundBuffer8* secondaryBuffer, int* volume, bool* fadein);


		


		bool stopWaveFile(IDirectSoundBuffer8* secondaryBuffer);
		IDirectSoundBuffer8* MusiqueMenuPrincipal;
		bool musiqueFadeOut1 = false;
		bool musiqueFadeIn1 = false;
		int volume1 = 0;

		IDirectSoundBuffer8* GameMusic;
		bool musiqueFadeIn2 = false;
		bool musiqueFadeOut2 = false;
		int volume2 = DSBVOLUME_MIN;


		int volumeMax = -1000;
		IDirectSoundBuffer8* Click;
		IDirectSoundBuffer8* Thunder;
		IDirectSoundBuffer8* Up;
		IDirectSoundBuffer8* Down;
		IDirectSoundBuffer8* Respawn;
		IDirectSoundBuffer8* Crash;

		bool growthPlayed=false;
		IDirectSoundBuffer8* Growth;



		IDirectSoundBuffer8* Victory;
	private:
		bool InitializeDirectSound(HWND);
		void ShutdownDirectSound();

		bool LoadWaveFile(char*, IDirectSoundBuffer8**);
		void ShutdownWaveFile(IDirectSoundBuffer8**);

	
		
	private:
		IDirectSound8* m_DirectSound;
		IDirectSoundBuffer* m_primaryBuffer;
		
	};
}
