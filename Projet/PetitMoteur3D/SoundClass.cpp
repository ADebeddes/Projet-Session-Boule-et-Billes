#include "stdafx.h"
#include "SoundClass.h"
#include "soundclass.h"
namespace PM3D {

	SoundClass::SoundClass()
	{
		m_DirectSound = 0;
		m_primaryBuffer = 0;
		MusiqueMenuPrincipal = 0;
		Click = 0;
	}


	SoundClass::SoundClass(const SoundClass& other)
	{
	}


	SoundClass::~SoundClass()
	{
	}


	bool SoundClass::Initialize(HWND hwnd)
	{
		bool result;
		// Initialize direct sound and the primary sound buffer.
		result = InitializeDirectSound(hwnd);
		if (!result)
		{
			return false;
		}

		// Load a wave audio file onto a secondary buffer.
		result = LoadWaveFile("comprestSound.wav", &MusiqueMenuPrincipal);
		if (!result)
		{
			return false;
		}

		result = LoadWaveFile("mouseClick.wav", &Click);
		if (!result)
		{
			return false;
		}

		result = LoadWaveFile("thunder.wav", &Thunder);
		if (!result)
		{
			return false;
		}

		Thunder->SetVolume(-2000);

		result = LoadWaveFile("up.wav", &Up);
		if (!result)
		{
			return false;
		}
		Up->SetVolume(-2000);
		result = LoadWaveFile("down.wav", &Down);
		if (!result)
		{
			return false;
		}
	
		result = LoadWaveFile("respawn.wav", &Respawn);
		if (!result)
		{
			return false;
		}

		result = LoadWaveFile("Victory.wav", &Victory);
		if (!result)
		{
			return false;
		}


		result = LoadWaveFile("GameMusic.wav", &GameMusic);
		if (!result)
		{
			return false;
		}

		result = LoadWaveFile("Impact.wav", &Crash);
		if (!result)
		{
			return false;
		}

		result = LoadWaveFile("growth.wav", &Growth);
		if (!result)
		{
			return false;
		}

		Growth->SetVolume(-4000);

		// Play the wave file now that it has been loaded.
		

		return true;
	}

	void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
	{
		// Release the secondary sound buffer.
		if (*secondaryBuffer)
		{
			(*secondaryBuffer)->Release();
			*secondaryBuffer = 0;
		}

		return;
	}

	void SoundClass::Shutdown()
	{
		// Release the secondary buffer.
		ShutdownWaveFile(&MusiqueMenuPrincipal);

		// Shutdown the Direct Sound API.
		ShutdownDirectSound();

		return;
	}

	

	bool SoundClass::InitializeDirectSound(HWND hwnd)
	{
		HRESULT result;
		DSBUFFERDESC bufferDesc;
		WAVEFORMATEX waveFormat;


		// Initialize the direct sound interface pointer for the default sound device.
		result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
		if (FAILED(result))
		{
			return false;
		}

		// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
		result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
		if (FAILED(result))
		{
			return false;
		}

		// Setup the primary buffer description.
		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
		bufferDesc.dwBufferBytes = 0;
		bufferDesc.dwReserved = 0;
		bufferDesc.lpwfxFormat = NULL;
		bufferDesc.guid3DAlgorithm = GUID_NULL;

		// Get control of the primary sound buffer on the default sound device.
		result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
		if (FAILED(result))
		{
			return false;
		}

		// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nChannels = 2;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		// Set the primary buffer to be the wave format specified.
		result = m_primaryBuffer->SetFormat(&waveFormat);
		if (FAILED(result))
		{
			return false;
		}

		return true;
	}


	void SoundClass::ShutdownDirectSound()
	{
		// Release the primary sound buffer pointer.
		if (m_primaryBuffer)
		{
			m_primaryBuffer->Release();
			m_primaryBuffer = 0;
		}

		// Release the direct sound interface pointer.
		if (m_DirectSound)
		{
			m_DirectSound->Release();
			m_DirectSound = 0;
		}

		return;
	}


	bool SoundClass::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer)
	{
		int error;
		FILE* filePtr;
		unsigned int count;
		WaveHeaderType waveFileHeader;
		WAVEFORMATEX waveFormat;
		DSBUFFERDESC bufferDesc;
		HRESULT result;
		IDirectSoundBuffer* tempBuffer;
		unsigned char* waveData;
		unsigned char* bufferPtr;
		unsigned long bufferSize;

		// Open the wave file in binary.
		error = fopen_s(&filePtr, filename, "rb");
		if (error != 0)
		{
			return false;
		}

		// Read in the wave file header.
		count = static_cast<unsigned int>(fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr));
		if (count != 1)
		{
			return false;
		}

		// Check that the chunk ID is the RIFF format.
		if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
			(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
		{
			return false;
		}

		// Check that the file format is the WAVE format.
		if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
			(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
		{
			return false;
		}

		// Check that the sub chunk ID is the fmt format.
		if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
			(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
		{
			return false;
		}

		// Check that the audio format is WAVE_FORMAT_PCM.
		if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
		{
			return false;
		}

		// Check that the wave file was recorded in stereo format.
		if (waveFileHeader.numChannels != 2)
		{
			return false;
		}

		// Check that the wave file was recorded at a sample rate of 44.1 KHz.
		if (waveFileHeader.sampleRate != 44100)
		{
			return false;
		}

		// Ensure that the wave file was recorded in 16 bit format.
		if (waveFileHeader.bitsPerSample != 16)
		{
			return false;
		}

		// Check for the data chunk header.
		if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
			(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
		{
			return false;
		}

		// Set the wave format of secondary buffer that this wave file will be loaded onto.
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nChannels = 2;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
		bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
		bufferDesc.dwReserved = 0;
		bufferDesc.lpwfxFormat = &waveFormat;
		bufferDesc.guid3DAlgorithm = GUID_NULL;


		// Create a temporary sound buffer with the specific buffer settings.
		result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
		if (FAILED(result))
		{
			return false;
		}

		// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
		result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
		if (FAILED(result))
		{
			return false;
		}

		// Release the temporary buffer.
		tempBuffer->Release();
		tempBuffer = 0;

		// Move to the beginning of the wave data which starts at the end of the data chunk header.
		fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

		// Create a temporary buffer to hold the wave file data.
		waveData = new unsigned char[waveFileHeader.dataSize];
		if (!waveData)
		{
			return false;
		}

		// Read in the wave file data into the newly created buffer.
		count = static_cast<unsigned int>(fread(waveData, 1, waveFileHeader.dataSize, filePtr));
		if (count != waveFileHeader.dataSize)
		{
			return false;
		}

		// Close the file once done reading.
		error = fclose(filePtr);
		if (error != 0)
		{
			return false;
		}

		// Lock the secondary buffer to write wave data into it.
		result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
		if (FAILED(result))
		{
			return false;
		}

		// Copy the wave data into the buffer.
		memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

		// Unlock the secondary buffer after the data has been written to it.
		result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
		if (FAILED(result))
		{
			return false;
		}

		// Release the wave data since it was copied into the secondary buffer.
		delete[] waveData;
		waveData = 0;

		return true;	
	}

	bool SoundClass::PlayWaveFile(IDirectSoundBuffer8* secondaryBuffer)
	{
		HRESULT result;


		// Set position at the beginning of the sound buffer.
		result = secondaryBuffer->SetCurrentPosition(0);
		if (FAILED(result))
		{
			return false;
		}


		// Play the contents of the secondary sound buffer.
		result = secondaryBuffer->Play(0, 0, 0);
		if (FAILED(result))
		{
			return false;
		}

		return true;
	}
	bool SoundClass::fadeOut1()
	{
		musiqueFadeOut1 = true;
		return true;
	}

	bool SoundClass::fadeOut2()
	{
		musiqueFadeOut2 = true;
		return false;
	}

	bool SoundClass::fadeIn1()
	{
		musiqueFadeIn1 = true;
		return true;
	}

	bool SoundClass::fadeIn2()
	{
		musiqueFadeIn2 = true;
		return true;
	}

	bool SoundClass::DiminuerSon1(IDirectSoundBuffer8* secondaryBuffer, int* volume, bool* fadeout)
	{
		if (volume1 > DSBVOLUME_MIN) {
			volume1 -= 60;
			secondaryBuffer->SetVolume(volume1);
		}
		else {
			musiqueFadeOut1= false;
		}
		return  true;
	}
	
	bool SoundClass::DiminuerSon2(IDirectSoundBuffer8* secondaryBuffer, int* volume, bool* fadeout)
	{
		if (volume2 > DSBVOLUME_MIN) {
			volume2 -= 60;
			secondaryBuffer->SetVolume(volume2);
		}
		else {
			musiqueFadeOut2 = false;
		}
		return  true;
	}
	
	
	



	bool SoundClass::AugmenterSon1(IDirectSoundBuffer8* secondaryBuffer, int* volume, bool* fadein)
	{
		if (volume1< volumeMax) {
			volume1 += 160;
			secondaryBuffer->SetVolume(volume1);
		}
		else {
			musiqueFadeIn1 = false;
		}
		return  true;
	}

	bool SoundClass::AugmenterSon2(IDirectSoundBuffer8* secondaryBuffer, int* volume, bool* fadein)
	{
		if (volume2 < volumeMax) {
			volume2 += 160;
			secondaryBuffer->SetVolume(volume2);
		}
		else {
			musiqueFadeIn2 = false;
		}
		return  true;
	}
	
	
	bool SoundClass::stopWaveFile(IDirectSoundBuffer8* secondaryBuffer)
	{
		// Set position at the beginning of the sound buffer.
		

		// Play the contents of the secondary sound buffer.
		auto result = secondaryBuffer->Stop(); 
		if (FAILED(result))
		{
			return false;
		}
		return true;

	}
}
