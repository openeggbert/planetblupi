/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddutil.cpp
 *  Content:    Routines for loading bitmap and palettes from resources
 *
 ***************************************************************************/

/**
 * @file ddutil.h
 * @brief DirectDraw utility helpers (originally from the DirectX 5 SDK).
 *
 * Provides convenience wrappers around IDirectDraw for:
 *  - Loading BMP files or HBITMAP objects into off-screen surfaces
 *  - Loading 8-bit palettes from BMP resources
 *  - Setting transparent colour keys (one or two colours)
 *  - Searching a surface for a matching colour entry
 *
 * Used by CPixmap to manage its off-screen surface pool.
 */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

/** @brief Enable or disable verbose DirectDraw debug output. */
extern void DDSetDebug(BOOL bDebug);
/**
 * @brief Create an off-screen surface from an existing HBITMAP.
 * @return Newly created IDirectDrawSurface, or NULL on failure
 */
extern IDirectDrawSurface * DDConnectBitmap(IDirectDraw *pdd, HBITMAP hbm);
/**
 * @brief Load a 256-colour palette from a BMP file or resource.
 * @param szBitmap Path or resource name of the BMP
 * @return Newly created IDirectDrawPalette, or NULL on failure
 */
extern IDirectDrawPalette * DDLoadPalette(IDirectDraw *pdd, LPCSTR szBitmap);
/**
 * @brief Load a BMP file into a new off-screen surface, optionally cropped.
 * @param dx Desired width (0 = use bitmap width)
 * @param dy Desired height (0 = use bitmap height)
 */
extern IDirectDrawSurface * DDLoadBitmap(IDirectDraw *pdd, LPCSTR szBitmap, int dx, int dy);
/** @brief Reload a BMP file into an existing surface (restore after mode switch). */
extern HRESULT              DDReLoadBitmap(IDirectDrawSurface *pdds, LPCSTR szBitmap);
/**
 * @brief Copy an HBITMAP (or sub-region) into a DirectDraw surface.
 * @param x, y  Source origin within the HBITMAP
 * @param dx,dy Source dimensions to copy
 */
extern HRESULT              DDCopyBitmap(IDirectDrawSurface *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
/**
 * @brief Find the surface pixel value that best matches @p rgb.
 * @return Packed pixel value suitable for colour-key operations
 */
extern DWORD                DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb);
/** @brief Set a single transparent colour key on @p pdds. */
extern HRESULT              DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb);
/** @brief Set a two-colour transparent range on @p pdds. */
extern HRESULT              DDSetColorKey2(IDirectDrawSurface *pdds, COLORREF rgb1, COLORREF rgb2);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
