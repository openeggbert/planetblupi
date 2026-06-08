// misc.h
//

/**
 * @file misc.h
 * @brief Miscellaneous utility functions for Planet Blupi.
 *
 * Covers application instance management, debug output, localised string
 * loading, random number generation, path helpers for game assets, and
 * DirectX error-reporting macros.
 */

/** @brief Store the application HINSTANCE for resource loading. */
extern void InitHInstance(HINSTANCE hInstance);
/** @brief Write a debug message to the debug output stream. */
extern void OutputDebug(char *pMessage);
/**
 * @brief Load a localised string from the application resources.
 * @param nID      String resource identifier (TX_* constant)
 * @param pBuffer  Destination buffer
 * @param lgBuffer Buffer capacity in bytes
 */
extern void LoadString(UINT nID, char *pBuffer, int lgBuffer);
/**
 * @brief Change the current mouse cursor to a SPRITE_* sprite.
 * @param sprite SPRITE_* constant from def.h
 */
extern void ChangeSprite(int sprite);

/**
 * @brief Unpack a Win32 LPARAM mouse-position into a POINT.
 * @param lParam LPARAM from a WM_MOUSEMOVE / WM_xBUTTONxx message
 */
extern POINT ConvLongToPos(LPARAM lParam);

/** @brief Seed the pseudo-random number generator. */
extern void	InitRandom();
/**
 * @brief Return a uniformly distributed random integer in [@p min, @p max].
 * @param min Lower bound (inclusive)
 * @param max Upper bound (inclusive)
 */
extern int	Random(int min, int max);

/**
 * @brief Get the directory containing the game executable.
 * @param pName Output buffer for the path string
 * @param lg    Buffer capacity
 */
extern void GetCurrentDir(char *pName, int lg);
/**
 * @brief Prepend the CD-ROM data path to a filename.
 * @param pFilename [in/out] filename to which the CD path is prepended in place
 */
extern void AddCDPath(char *pFilename);
/**
 * @brief Prepend the user save-data path to a filename.
 * @param pFilename [in/out] filename to which the user path is prepended in place
 */
extern void AddUserPath(char *pFilename);

/**
 * @brief Log a DirectDraw HRESULT error with source location.
 * @param hErr  The failing HRESULT
 * @param sFile Source file name (pass __FILE__)
 * @param nLine Source line number (pass __LINE__)
 */
extern void TraceErrorDD(HRESULT hErr, char *sFile, int nLine);
/**
 * @brief Log a DirectSound HRESULT error with source location.
 * @param hErr  The failing HRESULT
 * @param sFile Source file name (pass __FILE__)
 * @param nLine Source line number (pass __LINE__)
 */
extern void TraceErrorDS(HRESULT hErr, char *sFile, int nLine);

