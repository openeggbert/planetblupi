// Jauge.h

/**
 * @file jauge.h
 * @brief CJauge — energy / progress gauge widget for the Planet Blupi HUD.
 *
 * Renders one of the two on-screen gauges (Blupi energy and mission
 * progress) using the CHJAUGE sprite sheet (image/jauge.blp).
 */

/////////////////////////////////////////////////////////////////////////////

/**
 * @brief On-screen gauge (energy bar or mission-progress bar).
 *
 * Owned by CEvent; two gauges are created for each play session.
 * The level (0 … 100) is set via SetLevel() and the gauge redraws
 * automatically on the next Draw() call.
 */
class CJauge
{
public:
	CJauge();
	~CJauge();

	/**
	 * @brief Initialise the gauge widget.
	 * @param pos              Top-left pixel position in the HUD
	 * @param type             Gauge type index (selects the sprite variant)
	 * @param bMinimizeRedraw  TRUE to skip redundant redraws
	 */
	BOOL	Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int type, BOOL bMinimizeRedraw);
	/** @brief Render the gauge onto the back buffer (if dirty). */
	void	Draw();
	/** @brief Force a full redraw on the next Draw() call. */
	void	Redraw();

	/** @brief Set the fill level (0 = empty, 100 = full). */
	void	SetLevel(int level);
	/** @brief Switch the visual variant. */
	void	SetType(int type);

	/** @brief Return TRUE when the gauge is hidden. */
	BOOL	GetHide();
	/** @brief Show or hide the gauge. */
	void	SetHide(BOOL bHide);

	/** @brief Return the top-left pixel position of the gauge. */
	POINT	GetPos();
	/** @brief Mark the gauge as dirty so it redraws next frame. */
	void	SetRedraw();
	
protected:
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	BOOL		m_bHide;		// TRUE si bouton caché
	POINT		m_pos;			// coin sup/gauche
	POINT		m_dim;			// dimensions
	int			m_type;
	int			m_level;
	BOOL		m_bMinimizeRedraw;
	BOOL		m_bRedraw;		// TRUE -> doit ętre redessiné
};

/////////////////////////////////////////////////////////////////////////////
