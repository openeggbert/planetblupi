// Button.h

/**
 * @file button.h
 * @brief CButton — single clickable toolbar / HUD button widget.
 *
 * Each button can hold an optional flyout sub-menu (up to 20 icons)
 * and tooltip strings.  It draws itself using the CHBUTTON sprite sheet
 * and posts a configurable UINT message when activated.
 *
 * Up to MAXBUTTON (40) buttons are owned by CEvent and constitute the
 * main action toolbar shown during play and build phases.
 */

/////////////////////////////////////////////////////////////////////////////

/**
 * @brief A clickable button widget with optional sub-menu and tooltips.
 *
 * Supports three visual states: released (0), pressed (1) and hovered (+2).
 * Buttons can be enabled/disabled and hidden/shown at runtime.
 */
class CButton
{
public:
	CButton();
	~CButton();

	/**
	 * @brief Create the button widget.
	 * @param pos             Top-left screen position
	 * @param type            Visual type index (selects frame from sprite sheet)
	 * @param bMinimizeRedraw TRUE to skip redundant redraws
	 * @param pMenu           Array of icon indices for the sub-menu flyout
	 * @param nbMenu          Number of sub-menu entries (0 = no flyout)
	 * @param pTooltips       Array of string resource IDs for tooltips
	 * @param nbToolTips      Number of tooltip entries
	 * @param region          World region index (selects sprite variant)
	 * @param message         Windows message to post when the button fires
	 */
	BOOL	Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int type, BOOL bMinimizeRedraw,
				   int *pMenu, int nbMenu,
				   int *pTooltips, int nbToolTips,
				   int region, UINT message);
	/** @brief Draw the button onto the back buffer (if dirty). */
	void	Draw();
	/** @brief Force a full redraw on the next Draw() call. */
	void	Redraw();

	/** @brief Return the current button state (0=released, 1=pressed, +2=hovered). */
	int		GetState();
	/** @brief Set the button state programmatically. */
	void	SetState(int state);

	/** @brief Return the currently selected sub-menu entry index. */
	int		GetMenu();
	/** @brief Programmatically select a sub-menu entry. */
	void	SetMenu(int menu);

	/** @brief Return TRUE when the button is interactive. */
	BOOL	GetEnable();
	/** @brief Enable or disable the button. */
	void	SetEnable(BOOL bEnable);

	/** @brief Return TRUE when the button is invisible. */
	BOOL	GetHide();
	/** @brief Show or hide the button. */
	void	SetHide(BOOL bHide);

	/**
	 * @brief Forward a Windows message to the button for hit-testing.
	 * @return TRUE if the message was consumed by this button
	 */
	BOOL	TreatEvent(UINT message, WPARAM wParam, LPARAM lParam);
	/** @brief Return TRUE when @p pos falls within the button rectangle. */
	BOOL	MouseOnButton(POINT pos);
	/**
	 * @brief Return the tooltip string resource ID at @p pos, or -1.
	 * @param pos Screen position to test (for sub-menu hit-testing)
	 */
	int		GetToolTips(POINT pos);

	
protected:
	BOOL	Detect(POINT pos);
	BOOL	MouseDown(POINT pos);
	BOOL	MouseMove(POINT pos);
	BOOL	MouseUp(POINT pos);

protected:
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	int			m_type;			// type de bouton
	BOOL		m_bEnable;		// TRUE si bouton actif
	BOOL		m_bHide;		// TRUE si bouton caché
	UINT		m_message;		// message envoyé si bouton actionné
	POINT		m_pos;			// coin sup/gauche
	POINT		m_dim;			// dimensions
	int			m_state;		// 0=relâché, 1=pressé, +2=survollé
	int			m_mouseState;	// 0=relâché, 1=pressé, +2=survollé
	int			m_iconMenu[20];	// icônes du sous-menu
	int			m_toolTips[20];	// info-bulles
	int			m_nbMenu;		// nb de case du sous-menu
	int			m_nbToolTips;	// nb d'info-bulles
	int			m_selMenu;		// sous-menu sélectionné
	BOOL		m_bMouseDown;	// TRUE -> bouton souris pressé
	BOOL		m_bMinimizeRedraw;
	BOOL		m_bRedraw;		// TRUE -> doit ętre redessiné
};

/////////////////////////////////////////////////////////////////////////////
