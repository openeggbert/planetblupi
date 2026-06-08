// sound.h
//

/**
 * @file sound.h
 * @brief CSound — DirectSound-based audio manager for Planet Blupi.
 *
 * Manages loading, caching and playback of WAV sound effects and MIDI music
 * tracks.  Each sound effect is identified by a channel index (0 … MAXSOUND-1)
 * that corresponds to a SOUND_* constant defined in def.h.  MIDI music is
 * controlled separately via PlayMusic() / StopMusic().
 */

#pragma once

#include <dsound.h>
#include <stdio.h>


/////////////////////////////////////////////////////////////////////////////

#define MAXSOUND	100  ///< Maximum number of concurrently cached sound buffers
#define MAXVOLUME	20   ///< Number of discrete volume steps exposed to the UI
#define MAXBLUPI	100  ///< Maximum number of characters that can each own a sound channel

/**
 * @brief DirectSound-based audio manager.
 *
 * Wraps IDirectSound to provide:
 *  - Caching of WAV files into DirectSound secondary buffers
 *  - Positional (panned) playback relative to the viewport
 *  - MIDI music playback via MCI
 *  - Independent volume controls for SFX and music
 */
class CSound
{
public:
	CSound();
	~CSound();

	/** @brief Initialise DirectSound and attach to the given window. */
	BOOL	Create(HWND hWnd);
	/** @brief Enable or disable all audio output. */
	void	SetState(BOOL bState);
	/** @brief Return TRUE if DirectSound was successfully initialised. */
	BOOL	GetEnable();

	/** @brief Set the SFX volume (0 … MAXVOLUME). */
	void	SetAudioVolume(int volume);
	/** @brief Return the current SFX volume. */
	int		GetAudioVolume();
	/** @brief Set the MIDI music volume (0 … MAXVOLUME). */
	void	SetMidiVolume(int volume);
	/** @brief Return the current MIDI music volume. */
	int		GetMidiVolume();

	/** @brief Pre-load all sound files referenced by the current world. */
	void	CacheAll();
	/**
	 * @brief Load a WAV file into DirectSound buffer @p channel.
	 * @param channel Destination buffer slot (0 … MAXSOUND-1)
	 * @param pFilename Path to the WAV file
	 */
	BOOL	Cache(int channel, char *pFilename);
	/** @brief Release the DirectSound buffer for @p channel. */
	void	Flush(int channel);

	/**
	 * @brief Play sound buffer @p channel with optional volume and pan offsets.
	 * @param channel Buffer index previously loaded with Cache()
	 * @param volume  Additional attenuation in DirectSound millibels (0 = full)
	 * @param pan     Pan offset in millibels (negative = left, positive = right)
	 */
	BOOL	Play(int channel, int volume=0, int pan=0);
	/**
	 * @brief Play a sound at a world position, auto-panning relative to the viewport.
	 * @param channel Buffer index
	 * @param pos     World pixel position of the sound source
	 * @param rank    Blupi rank owning this sound (-1 = global)
	 */
	BOOL	PlayImage(int channel, POINT pos, int rank=-1);
	/**
	 * @brief Start MIDI playback via MCI.
	 * @param hWnd           Window receiving MCI notifications
	 * @param lpszMIDIFilename Path to the .mid file
	 */
	BOOL	PlayMusic(HWND hWnd, LPSTR lpszMIDIFilename);
	/** @brief Restart the current MIDI track from the beginning. */
	BOOL	RestartMusic();
	/** @brief Temporarily pause MIDI playback. */
	void	SuspendMusic();
	/** @brief Stop MIDI playback entirely. */
	void	StopMusic();
	/** @brief Return TRUE while a MIDI track is actively playing. */
	BOOL	IsPlayingMusic();
	/** @brief Synchronise the MIDI volume to the current m_midiVolume setting. */
	void	AdaptVolumeMusic();
	/** @brief Set the number of suspend calls to skip before actually suspending. */
	void	SetSuspendSkip(int nb);

protected:
	/** @brief Create a DirectSound secondary buffer with the given PCM parameters. */
	BOOL	CreateSoundBuffer(int dwBuf, DWORD dwBufSize, DWORD dwFreq, DWORD dwBitsPerSample, DWORD dwBlkAlign, BOOL bStereo);
	/** @brief Fill a DirectSound buffer with raw PCM data from an open file. */
	BOOL	ReadData(LPDIRECTSOUNDBUFFER lpDSB, FILE* pFile, DWORD dwSize, DWORD dwPos);
	/** @brief Open a WAV file and create a matching DirectSound buffer. */
	BOOL	CreateBufferFromWaveFile(int dwBuf, char *pFileName);
	/** @brief Stop all currently playing DirectSound buffers. */
	BOOL	StopAllSounds();
	/** @brief Trigger playback of a specific DirectSound buffer by raw index. */
	BOOL	PlaySoundDS(DWORD dwSound, DWORD dwFlags);

protected:
	HWND				m_hWnd;
	BOOL				m_bEnable;
	BOOL				m_bState;
	LPDIRECTSOUND		m_lpDS;
	LPDIRECTSOUNDBUFFER	m_lpDSB[MAXSOUND];
	short				m_channelBlupi[MAXBLUPI];
	UINT				m_MidiDeviceID;
	char				m_MIDIFilename[50];
	int					m_audioVolume;
	int					m_midiVolume;
	int					m_lastMidiVolume;
	int					m_nbSuspendSkip;
};

/////////////////////////////////////////////////////////////////////////////
