// Event.h

/**
 * @file event.h
 * @brief CEvent — top-level input and game-phase manager for Planet Blupi.
 *
 * CEvent owns the button strip, the two HUD gauges, the popup menu, phase
 * transitions, demo recording/playback, and the main WM_* message router.
 *
 * Supporting data structures:
 *  - @ref Button     — static descriptor for one toolbar button slot
 *  - @ref Phase      — static descriptor for one game phase (screen)
 *  - @ref DemoHeader — binary header of a recorded demo file
 *  - @ref DemoEvent  — one timestamped input event in a demo file
 */

/////////////////////////////////////////////////////////////////////////////

/**
 * @brief Static descriptor for one toolbar button slot.
 *
 * These descriptors are compiled into event.cpp's phase table and are
 * read-only after startup.
 */
typedef struct
{
	UINT	message;       ///< WM_* message posted when the button fires
	int		type;          ///< Visual type index within the CHBUTTON sprite sheet
	int		iconMenu[20];  ///< Sub-menu icon indices (up to 20)
	int		x, y;          ///< Top-left pixel position on screen
	int		toolTips[20];  ///< Tooltip string resource IDs (TX_*)
}
Button;

/**
 * @brief Static descriptor for one game phase (screen / mode).
 *
 * Phases correspond to the WM_PHASE_* messages.  Each phase defines the
 * background image name, CD-ROM requirement flag, and the set of toolbar
 * buttons that are active during that phase.
 */
typedef struct
{
	UINT	phase;              ///< WM_PHASE_* identifier
	char	backName[20];       ///< Filename stem of the background BLP image
	BOOL	bCDrom;             ///< TRUE if the phase requires the CD-ROM
	Button	buttons[MAXBUTTON]; ///< Toolbar button layout for this phase
}
Phase;


/**
 * @brief Binary header at the start of a .rec demo recording file.
 */
typedef struct
{
	short	majRev;    ///< Major version of the engine that recorded the demo
	short	minRev;    ///< Minor version
	short	bSchool;   ///< TRUE if recorded in school (tutorial) mode
	short	bPrivate;  ///< TRUE if recorded in private (map-editor) mode
	short	world;     ///< World number that was played
	short	skill;     ///< Skill level (0 = easy, 1 = normal, 2 = hard)
	short	reserve1[99];
}
DemoHeader;

/**
 * @brief One timestamped input event stored in a demo recording.
 */
typedef struct
{
	int		time;     ///< Relative time in game ticks when the event occurred
	UINT	message;  ///< Windows message type (WM_LBUTTONDOWN, WM_MOUSEMOVE, …)
	WPARAM	wParam;   ///< Message wParam
	LPARAM	lParam;   ///< Message lParam (encodes mouse position for mouse events)
}
DemoEvent;


/**
 * @brief Top-level input router and game-phase controller.
 *
 * One global CEvent instance drives the entire game loop:
 *  - Maintains the current WM_PHASE_* phase and switches between screens
 *  - Routes WM_* messages to the appropriate subsystem (CDecor, CButton, CMenu …)
 *  - Manages demo recording / playback
 *  - Owns the toolbar buttons (CButton[MAXBUTTON]), gauges (CJauge[2])
 *    and the popup action menu (CMenu)
 */
class CEvent
{
public:
	CEvent();
	~CEvent();

	/** @brief Return the last known mouse position in screen pixels. */
	POINT	GetMousePos();
	/** @brief Bind the event manager to all game subsystems. */
	void	Create(HWND hWnd, CPixmap *pPixmap, CDecor *pDecor, CSound *pSound, CMovie *pMovie);
	/** @brief Notify the event manager of a full-screen mode change. */
	void	SetFullScreen(BOOL bFullScreen);
	/** @brief Select the mouse cursor behaviour (MOUSETYPE* constant). */
	void	SetMouseType(int mouseType);
	/** @brief Return the current game world index (may differ from physical/image). */
	int		GetWorld();
	/** @brief Return the world number as stored on disk. */
	int		GetPhysicalWorld();
	/** @brief Return the world number used to select background images. */
	int		GetImageWorld();
	/** @brief Return TRUE when the in-game help overlay is hidden. */
	BOOL	IsHelpHide();
	/**
	 * @brief Switch to a new game phase (screen).
	 * @param phase WM_PHASE_* identifier
	 * @return TRUE on success
	 */
	BOOL	ChangePhase(UINT phase);
	/** @brief Queue a movie to start once the current phase finishes. */
	void	MovieToStart();
	/** @brief Return the current WM_PHASE_* identifier. */
	UINT	GetPhase();
	/** @brief Try to show the CD-ROM insert prompt. */
	void	TryInsert();
	/** @brief Restore the last saved game from disk. */
	void	RestoreGame();

	/** @brief Return the slot index of toolbar button @p button (BUTTON_*). */
	int		GetButtonIndex(int button);
	/** @brief Return the state of toolbar button @p button. */
	int		GetState(int button);
	/** @brief Set the state of toolbar button @p button. */
	void	SetState(int button, int state);
	/** @brief Return TRUE when toolbar button @p button is interactive. */
	BOOL	GetEnable(int button);
	/** @brief Enable/disable toolbar button @p button. */
	void	SetEnable(int button, BOOL bEnable);
	/** @brief Return TRUE when toolbar button @p button is hidden. */
	BOOL	GetHide(int button);
	/** @brief Show/hide toolbar button @p button. */
	void	SetHide(int button, BOOL bHide);
	/** @brief Return the selected sub-menu index of toolbar button @p button. */
	int		GetMenu(int button);
	/** @brief Set the selected sub-menu index of toolbar button @p button. */
	void	SetMenu(int button, int menu);

	/** @brief Draw all dirty toolbar buttons; return TRUE if anything changed. */
	BOOL	DrawButtons();
	/**
	 * @brief Return the SPRITE_* cursor for the given screen position.
	 * Used to show directional scroll arrows near the viewport edges.
	 */
	int		MousePosToSprite(POINT pos);
	/** @brief Update the cursor sprite for the current @p pos. */
	void	MouseSprite(POINT pos);
	/** @brief Show the "busy" (wait) cursor overlay. */
	void	WaitMouse(BOOL bWait);
	/** @brief Temporarily hide the cursor (e.g. during full-screen transitions). */
	void	HideMouse(BOOL bHide);
	/** @brief Return the mouse position from the previous frame. */
	POINT	GetLastMousePos();
	/**
	 * @brief Primary message handler — dispatches to the active phase.
	 * @return TRUE if the message was fully consumed
	 */
	BOOL	TreatEvent(UINT message, WPARAM wParam, LPARAM lParam);
	/** @brief Handle messages common to all phases (quit, resize, etc.). */
	BOOL	TreatEventBase(UINT message, WPARAM wParam, LPARAM lParam);

	/** @brief Auto-scroll the world view when @p pos is near the viewport edge. */
	void	DecorAutoShift(POINT pos);

	/** @brief Start AVI playback via CMovie. */
	BOOL	StartMovie(char *pFilename);
	/** @brief Stop the current AVI movie. */
	void	StopMovie();
	/** @brief Return TRUE while an AVI movie is playing. */
	BOOL	IsMovie();

	/** @brief Advance animated objects on the current frame (called by WM_TIMER). */
	BOOL	FlipObject();

	/** @brief Load a saved game slot identified by @p message (WM_READ*). */
	void	Read(int message);
	/** @brief Save the current game to slot identified by @p message (WM_WRITE*). */
	void	Write(int message);

	/** @brief Set the game speed multiplier (1 = normal). */
	void	SetSpeed(int speed);
	/** @brief Return the current speed multiplier. */
	int		GetSpeed();
	/** @brief Return TRUE when the game is paused. */
	BOOL	GetPause();
	/** @brief Return TRUE when the Shift key is held. */
	BOOL	IsShift();

	/** @brief Advance the demo playback by one tick. */
	void	DemoStep();
	/** @brief Display a single debug character on-screen. */
	void	DebugDisplay(char m);

	/** @brief Advance the intro animation sequence by one frame. */
	void	IntroStep();

protected:
	void	DrawTextCenter(int res, int x, int y, int font=0);
	BOOL	CreateButtons();
	BOOL	EventButtons(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL	MouseOnButton(POINT pos);
	int		SearchPhase(UINT phase);
	void	DecorShift(int dx, int dy);

	BOOL	PlayDown(POINT pos, int fwKeys);
	BOOL	PlayMove(POINT pos, int fwKeys);
	BOOL	PlayUp(POINT pos, int fwKeys);
	void	ChangeButtons(int message);

	void	BuildFloor(POINT cel, int insIcon);
	void	BuildWater(POINT cel, int insIcon);
	BOOL	BuildDown(POINT pos, int fwKeys, BOOL bMix=TRUE);
	BOOL	BuildMove(POINT pos, int fwKeys);
	BOOL	BuildUp(POINT pos, int fwKeys);

	void	PrivateLibelle();
	BOOL	ReadLibelle(int world, BOOL bSchool, BOOL bHelp);
	BOOL	WriteInfo();
	BOOL	ReadInfo();

	void	DemoRecStart();
	void	DemoRecStop();
	BOOL	DemoPlayStart();
	void	DemoPlayStop();
	void	DemoRecEvent(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	int			m_speed;
	int			m_exercice;
	int			m_mission;
	int			m_private;
	int			m_maxMission;
	int			m_phase;
	int			m_index;
	BOOL		m_bSchool;
	BOOL		m_bPrivate;
	BOOL		m_bAccessBuild;
	BOOL		m_bFullScreen;
	int			m_mouseType;
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	CMovie*		m_pMovie;
	char		m_movieToStart[MAX_PATH];
	int			m_phaseAfterMovie;
	CButton		m_buttons[MAXBUTTON];
	int			m_lastFloor[MAXBUTTON];
	int			m_lastObject[MAXBUTTON];
	int			m_lastHome[MAXBUTTON];
	BOOL		m_bRunMovie;
	BOOL		m_bBuildModify;
	CJauge		m_jauges[2];
	CMenu		m_menu;
	BOOL		m_bMenu;
	POINT		m_menuPos;
	int			m_menuNb;
	int			m_menuButtons[MAXBUTTON];
	int			m_menuErrors[MAXBUTTON];
	int			m_menuPerso;
	POINT		m_menuCel;
	POINT		m_oldMousePos;
	BOOL		m_bMousePress;
	BOOL		m_bMouseDown;
	BOOL		m_bHili;
	int			m_fileWorld[10];
	int			m_fileTime[10];
	POINT		m_posToolTips;
	char		m_textToolTips[50];
	int			m_mouseSprite;
	BOOL		m_bFillMouse;
	BOOL		m_bWaitMouse;
	BOOL		m_bHideMouse;
	BOOL		m_bShowMouse;
	int			m_rankCheat;
	int			m_posCheat;
	BOOL		m_bMovie;
	BOOL		m_bSpeed;
	BOOL		m_bHelp;
	BOOL		m_bAllMissions;
	BOOL		m_bChangeCheat;
	int			m_scrollSpeed;
	BOOL		m_bPause;
	BOOL		m_bShift;
	int			m_shiftPhase;
	POINT		m_shiftVector;
	POINT		m_shiftOffset;
	char		m_libelle[1000];
	int			m_tryPhase;
	int			m_tryInsertCount;
	POINT		m_posInfoButton;
	POINT		m_posHelpButton;
	BOOL		m_bHiliInfoButton;
	BOOL		m_bHiliHelpButton;
	BOOL		m_bInfoHelp;
	BOOL		m_bDemoRec;
	BOOL		m_bDemoPlay;
	DemoEvent*	m_pDemoBuffer;
	int			m_demoTime;
	int			m_demoIndex;
	int			m_demoEnd;
	int			m_demoNumber;
	BOOL		m_bCtrlDown;
	POINT		m_debugPos;
	int			m_introTime;
};

/////////////////////////////////////////////////////////////////////////////
