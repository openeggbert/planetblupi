// CPixmap.h

/**
 * @file pixmap.h
 * @brief CPixmap — DirectDraw-based sprite renderer for Planet Blupi.
 *
 * Manages up to MAXIMAGE off-screen DirectDraw surfaces, each holding a
 * sprite sheet ("channel").  Sprites within a sheet are addressed by a
 * zero-based icon index.  All drawing goes through a back buffer that is
 * flipped to the primary surface by Display().
 *
 * Channel indices are defined by the CH* constants in def.h:
 *  - CHBACK (0)   – background / ground
 *  - CHFLOOR (1)  – floor tiles  (image/floor*.blp)
 *  - CHOBJECT (2) – objects       (image/obj*.blp)
 *  - CHOBJECTo(3) – overlay objects
 *  - CHBLUPI (4)  – Blupi sprites (image/blupi.blp)
 *  - CHHILI  (5)  – highlight overlay (image/hili.blp)
 *  - CHFOG   (6)  – fog of war (image/fog.blp)
 *  - CHMASK1 (7)  – alpha mask (image/mask1.blp)
 *  - CHLITTLE(8)  – small character font (image/little.blp)
 *  - CHMAP   (9)  – minimap (rendered at runtime)
 *  - CHBUTTON(10) – UI buttons (image/button.blp)
 *  - CHGROUND(11) – ground layer (image/floor*.blp region variants)
 *  - CHJAUGE (12) – energy gauge (image/jauge.blp)
 *  - CHTEXT  (13) – text font (image/text.blp)
 *  - CHBIGNUM(14) – large score digits (image/bignum.blp)
 */

#pragma once

#include <ddraw.h>

/////////////////////////////////////////////////////////////////////////////

#define MAXIMAGE	100  ///< Maximum number of simultaneously loaded image channels


/**
 * @brief DirectDraw-based sprite renderer.
 *
 * Wraps IDirectDraw surfaces for hardware-accelerated 2D sprite blitting.
 * Supports full-screen and windowed modes, software palette (256-colour),
 * transparent colour-key blitting, and custom mouse cursor sprites drawn
 * directly onto the back buffer.
 */
class CPixmap
{
public:
	CPixmap();
	~CPixmap();

	void	SetDebug(BOOL bDebug);
	
	/**
	 * @brief Initialise DirectDraw and create primary + back surfaces.
	 * @param hwnd        Owner window handle
	 * @param dim         Total surface dimensions in pixels
	 * @param bFullScreen TRUE for exclusive full-screen mode
	 * @param mouseType   One of MOUSETYPEGRA / MOUSETYPEWIN / MOUSETYPEWINPOS
	 */
	BOOL	Create(HWND hwnd, POINT dim, BOOL bFullScreen, int mouseType);
	/** @brief Free all DirectDraw resources. */
	BOOL	Flush();
	/** @brief Restore lost surfaces after an ALT+TAB event. */
	BOOL	Restore();
	/** @brief Copy the system palette to all surfaces. */
	BOOL	InitSysPalette();
	/** @brief Return TRUE when running in 8-bit palette mode. */
	BOOL	IsPalette();
	/** @brief Fill a rectangle on the back buffer with a solid colour. */
	void	Fill(RECT rect, COLORREF color);

	/** @brief Save the current surface palette entries. */
	BOOL	SavePalette();
	/** @brief Restore previously saved palette entries. */
	BOOL	RestorePalette();
	/**
	 * @brief Find the palette index closest to an RGB triple.
	 * @return Palette index 0 … 255
	 */
	int		SearchColor(int red, int green, int blue);
	/**
	 * @brief Load a sprite sheet from disk into @p channel.
	 * @param channel     Destination channel index (0 … MAXIMAGE-1)
	 * @param pFilename   Path to the BLP/BMP image file
	 * @param totalDim    Total bitmap dimensions
	 * @param iconDim     Dimensions of one sprite frame
	 * @param bUsePalette TRUE to apply the shared 8-bit palette
	 */
	BOOL	Cache(int channel, char *pFilename, POINT totalDim, POINT iconDim, BOOL bUsePalette);
	/** @brief Load a full-image (single-icon) sprite sheet. */
	BOOL	Cache(int channel, char *pFilename, POINT totalDim, BOOL bUsePalette);
	/** @brief Load from an in-memory HBITMAP. */
	BOOL	Cache(int channel, HBITMAP hbm, POINT totalDim);
	/** @brief Release the DirectDraw surface for @p channel. */
	void	Flush(int channel);
	/** @brief Set the transparent colour key for @p channel (1-colour). */
	void	SetTransparent(int channel, COLORREF color);
	/** @brief Set a 2-colour transparent range for @p channel. */
	void	SetTransparent2(int channel, COLORREF color1, COLORREF color2);
	/** @brief Set the clipping rectangle applied to all subsequent draws. */
	void	SetClipping(RECT clip);
	/** @brief Return the current clipping rectangle. */
	RECT	GetClipping();

	/**
	 * @brief Test whether a pixel within icon @p rank is opaque.
	 * @return TRUE if the pixel at @p pos is not transparent
	 */
	BOOL	IsIconPixel(int channel, int rank, POINT pos);

	/**
	 * @brief Blit one sprite frame onto a destination channel surface.
	 * @param chDst   Destination channel (typically 0 = back buffer)
	 * @param channel Source channel
	 * @param rank    Sprite frame index within the sheet
	 * @param pos     Top-left destination position
	 * @param mode    Blit mode flags
	 * @param bMask   TRUE to use the mask surface
	 */
	BOOL	DrawIcon(int chDst, int channel, int rank, POINT pos, int mode=0, BOOL bMask=FALSE);
	/** @brief Draw a sprite at half opacity (used for fog-of-war edges). */
	BOOL	DrawIconDemi(int chDst, int channel, int rank, POINT pos, int mode=0, BOOL bMask=FALSE);
	/**
	 * @brief Draw a horizontal strip of a sprite (for partial-reveal effects).
	 * @param startY First source row to copy
	 * @param endY   Last source row to copy (exclusive)
	 */
	BOOL	DrawIconPart(int chDst, int channel, int rank, POINT pos, int startY, int endY, int mode=0, BOOL bMask=FALSE);
	/** @brief Copy an arbitrary sub-rectangle from one channel to another. */
	BOOL	DrawPart(int chDst, int channel, POINT dest, RECT rect, int mode=0, BOOL bMask=FALSE);
	/** @brief Blit the entire @p rect of channel onto the destination channel. */
	BOOL	DrawImage(int chDst, int channel, RECT rect, int mode=0);

	/**
	 * @brief Combine two sprite frames through a mask to produce a third.
	 * Used to generate the masked Blupi highlight sprites at runtime.
	 */
	BOOL	BuildIconMask(int channelMask, int rankMask,
						  int channel, int rankSrc, int rankDst);

	/** @brief Flip the back buffer to the primary surface (show the frame). */
	BOOL	Display();

	/** @brief Move the software cursor and change its sprite atomically. */
	void	SetMousePosSprite(POINT pos, int sprite, BOOL bDemoPlay);
	/** @brief Update the software cursor position. */
	void	SetMousePos(POINT pos, BOOL bDemoPlay);
	/** @brief Change the cursor sprite without moving it. */
	void	SetMouseSprite(int sprite, BOOL bDemoPlay);
	/** @brief Show or hide the software cursor. */
	void	MouseShow(BOOL bShow);
	/** @brief Force a full redraw of the cursor background save area. */
	void	MouseInvalidate();
	/** @brief Clear the saved background under the cursor. */
	void	MouseBackClear();
	/** @brief Restore the background under the cursor (erase cursor). */
	void	MouseBackDraw();

protected:
	HRESULT	RestoreAll();
	HRESULT	BltFast(int chDst, int channel, POINT dst, RECT rcRect, int mode);
	HRESULT	BltFast(LPDIRECTDRAWSURFACE lpDD,
					int channel, POINT dst, RECT rcRect, int mode);

	void	MouseUpdate();
	BOOL	MouseQuickDraw(RECT rect);
	void	MouseBackSave();
	void	MouseBackRestore();
	void	MouseBackDebug();
	RECT	MouseRectSprite();
	void	MouseHotSpot();

protected:
	BOOL					m_bFullScreen;
	int						m_mouseType;
	BOOL					m_bDebug;
	BOOL					m_bPalette;
	HWND					m_hWnd;
	POINT					m_dim;					// dimensions totales
	RECT					m_clipRect;				// rectangle de clipping

	POINT					m_mousePos;
	int						m_mouseSprite;
	POINT					m_mouseHotSpot;
	POINT					m_mouseBackPos;
	BOOL					m_bMouseBack;
	BOOL					m_bMouseShow;
	BOOL					m_bBackDisplayed;

	LPDIRECTDRAW			m_lpDD;					// DirectDraw object
	LPDIRECTDRAWSURFACE		m_lpDDSPrimary;			// DirectDraw primary surface
	LPDIRECTDRAWSURFACE		m_lpDDSBack;			// DirectDraw back surface
	LPDIRECTDRAWSURFACE		m_lpDDSMouse;			// DirectDraw back surface
	LPDIRECTDRAWPALETTE		m_lpDDPal;				// the primary surface palette
	LPDIRECTDRAWSURFACE		m_lpDDSurface[MAXIMAGE]; // offscreen surfaces
	LPDIRECTDRAWCLIPPER		m_lpClipper;
    PALETTEENTRY			m_pal[256];				// sauvegarde palette
    PALETTEENTRY			m_sysPal[256];			// sauvegarde palette
	COLORREF				m_colorSurface[2*MAXIMAGE];

	char					m_filename[MAXIMAGE][20];
	POINT					m_totalDim[MAXIMAGE];	// dimensions totale image
	POINT					m_iconDim[MAXIMAGE];	// dimensions d'une icône
	DDBLTFX					m_DDbltfx;
};

/////////////////////////////////////////////////////////////////////////////
