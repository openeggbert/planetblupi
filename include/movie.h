// movie.h
//

/**
 * @file movie.h
 * @brief CMovie — MCI-based AVI playback for Planet Blupi cinematics.
 *
 * Wraps the Windows MCI (Media Control Interface) to play AVI video files
 * for intro sequences, mission briefings and the ending movie.
 * On non-Windows or Web builds the class is compiled in but Play() is a
 * no-op (GetEnable() returns FALSE).
 *
 * AVI files live in the @c movie/ directory of the game assets.
 */

/**
 * @brief MCI-based video player for AVI cinematic files.
 *
 * One CMovie instance is shared by the whole application.
 * The play loop is driven by the main Win32 message pump.
 */
class CMovie
{
public:
	CMovie();
	~CMovie();

	/** @brief Initialise the MCI subsystem; returns TRUE on success. */
	BOOL	Create();
	/** @brief Return TRUE when MCI video playback is available. */
	BOOL	GetEnable();
	/** @brief Return TRUE when the AVI file @p pFilename exists on disk. */
	BOOL	IsExist(char *pFilename);
	/**
	 * @brief Open and start playing an AVI file inside @p rect.
	 * @param hWnd      Parent window handle
	 * @param rect      Screen rectangle to play the video in
	 * @param pFilename Path to the AVI file
	 * @return TRUE if playback started successfully
	 */
	BOOL	Play(HWND hWnd, RECT rect, char *pFilename);
	/** @brief Stop and close the currently playing AVI. */
	void	Stop(HWND hWnd);

protected:
	void	playMovie(HWND hWnd, int nDirection);
	BOOL	fileOpenMovie(HWND hWnd, RECT rect, char *pFilename);
	void	fileCloseMovie(HWND hWnd);
	void	positionMovie(HWND hWnd, RECT rect);
	void	termAVI();
	BOOL	initAVI();

protected:
	BOOL		m_bEnable;
	MCIDEVICEID	m_wMCIDeviceID;	// MCI Device ID for the AVI file
	HWND		m_hwndMovie;	// window handle of the movie
	BOOL		m_fPlaying;		// Play flag: TRUE == playing, FALSE == paused
	BOOL		m_fMovieOpen;	// Open flag: TRUE == movie open, FALSE = none
};

