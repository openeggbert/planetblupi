/***************************************************************************
 *
 *  Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       wave.h
 *  Content:    DirectSound WAV routines include file
 *
 ***************************************************************************/

/**
 * @file wave.h
 * @brief Helper for loading embedded WAV resources into DirectSound buffers.
 *
 * Originally from the Microsoft DirectX 5 SDK sample utilities.
 * Used to load the built-in "boing" and "blow" WAV resources (IDR_WAVE_BOING
 * and IDR_WAVE_BLOW) at startup.
 */

#include <dsound.h>

/**
 * @brief Load a WAV resource from the application binary into a DirectSound buffer.
 * @param hinst      Application instance handle
 * @param ResourceID Win32 resource identifier (e.g. IDR_WAVE_BOING)
 * @param lpds       IDirectSound instance to create the buffer on
 * @param lpDSB      [out] Newly created IDirectSoundBuffer
 */
void LoadWave(HINSTANCE hinst, int ResourceID,
			  LPDIRECTSOUND lpds,
			  LPDIRECTSOUNDBUFFER &lpDSB);
