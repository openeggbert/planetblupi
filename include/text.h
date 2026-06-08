// Text.h

/**
 * @file text.h
 * @brief Sprite-based text rendering helpers for Planet Blupi.
 *
 * All text is rendered using sprite sheets loaded into CPixmap channels
 * (CHTEXT for normal fonts, CHLITTLE for the small font, CHBIGNUM for
 * large score digits).  Three fonts are available for the main text channel:
 *  - FONTWHITE (0) – standard white font
 *  - FONTRED   (1) – red variant (errors / warnings)
 *  - FONTSLIM  (2) – slim/narrow variant
 * The FONTLITTLE (10) value selects the small-character channel instead.
 */

#pragma once

#define FONTWHITE	0   ///< Standard white bitmap font
#define FONTRED		1   ///< Red bitmap font (used for error messages)
#define FONTSLIM	2   ///< Slim/narrow bitmap font
#define FONTLITTLE	10  ///< Small bitmap font (uses CHLITTLE channel)

/**
 * @brief Draw a string at @p pos using @p font.
 * @param pPixmap Renderer to draw into
 * @param pos     Top-left pixel position
 * @param pText   NUL-terminated string
 * @param font    FONT* constant selecting the font variant
 */
extern
void DrawText(CPixmap *pPixmap, POINT pos, char *pText, int font=0);

/**
 * @brief Draw a string along a diagonal slope.
 * @param pente Slope in 1/100 pixels per character (positive = downward)
 */
extern
void DrawTextPente(CPixmap *pPixmap, POINT pos, char *pText,
				   int pente, int font=0);

/**
 * @brief Draw a word-wrapped string inside a virtual column.
 * @param pente Horizontal slope applied to each line
 * @param part  Index of the paragraph to render (-1 = all)
 */
extern
void DrawTextRect(CPixmap *pPixmap, POINT pos, char *pText,
				  int pente, int font=0, int part=-1);

/**
 * @brief Draw a string centred on @p pos.
 */
extern
void DrawTextCenter(CPixmap *pPixmap, POINT pos, char *pText, int font=0);

/**
 * @brief Return the rendered pixel height of @p pText.
 * @param part Paragraph index (-1 = total height of all paragraphs)
 */
extern
int GetTextHeight(char *pText, int font=0, int part=-1);

/** @brief Return the rendered pixel width of @p pText. */
extern
int GetTextWidth(char *pText, int font=0);

/**
 * @brief Draw a large numeric score value using the CHBIGNUM sprite sheet.
 * @param num Non-negative integer to display
 */
extern
void DrawBignum(CPixmap *pPixmap, POINT pos, int num);

/** @brief Return the pixel width of the large-digit representation of @p num. */
extern
int GetBignumWidth(int num);

