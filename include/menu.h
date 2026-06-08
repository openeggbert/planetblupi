// Menu.h

/**
 * @file menu.h
 * @brief CMenu — context-action popup menu for the Planet Blupi play screen.
 *
 * When the player right-clicks on a Blupi or the terrain the engine queries
 * CDecor for the available actions and passes them to CMenu::Create().
 * The menu then draws a grid of action-button icons and translates mouse
 * events into WM_ACTION_* messages posted back to the main window.
 */

/////////////////////////////////////////////////////////////////////////////

/**
 * @brief Popup action-selection menu.
 *
 * Displays a grid of action buttons built from the CHBUTTON sprite sheet.
 * At most MAXBUTTON entries can be shown.  Each button corresponds to one
 * WM_ACTION_* Windows message that is dispatched when clicked.
 */
class CMenu
{
public:
	CMenu();
	~CMenu();

	/**
	 * @brief Build and show the popup menu.
	 * @param pos      Top-left position of the menu grid in screen pixels
	 * @param nb       Number of action buttons to display
	 * @param pButtons Array of BUTTON_* identifiers
	 * @param pErrors  Parallel array of ERROR_* codes (0 = action available)
	 * @param perso    Character type (0 = Blupi, 1..8 = enemies/disciples)
	 */
	BOOL	Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int nb, int *pButtons, int *pErrors,
				   int perso);
	/** @brief Refresh button states without recreating the menu. */
	void	Update(int nb, int *pButtons, int *pErrors);
	/** @brief Destroy the menu; subsequent IsExist() returns FALSE. */
	void	Delete();
	/** @brief Draw the menu grid onto the back buffer. */
	void	Draw();
	/** @brief Return the BUTTON_* identifier of the currently selected entry. */
	int		GetSel();
	/** @brief Return the slot index of the selected entry. */
	int		GetRank();
	/** @brief Return TRUE when the selected entry has a non-zero error code. */
	BOOL	IsError();
	/** @brief Return TRUE while the menu is visible. */
	BOOL	IsExist();
	/** @brief Post the WM_ACTION_* message for the selected button. */
	void	Message();

	BOOL	TreatEvent(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	int		Detect(POINT pos);
	BOOL	MouseDown(POINT pos);
	BOOL	MouseMove(POINT pos);
	BOOL	MouseUp(POINT pos);

protected:
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	POINT		m_pos;			// coin sup/gauche
	POINT		m_dim;			// dimensions
	int			m_nbButtons;
	POINT		m_nbCel;
	int			m_perso;
	int			m_buttons[MAXBUTTON];
	int			m_errors[MAXBUTTON];
	UINT		m_messages[MAXBUTTON];
	int			m_selRank;
	POINT		m_mousePos;
};

/////////////////////////////////////////////////////////////////////////////
