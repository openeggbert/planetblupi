/**
 * @file decmove.h
 * @brief Access functions for the animated-decoration movement and icon tables.
 *
 * decmove.cpp contains two static tables:
 *  - A movement table (pixel offsets per step)
 *  - An icon table (sprite frame sequences)
 *
 * Each table is encoded as a series of records separated by a count prefix.
 * CDecor uses GetListMoves() / GetListIcons() to locate a specific record
 * when setting up a Move element.
 */

#pragma once

/**
 * @brief Return a pointer to movement record @p rank in the global table.
 *
 * The record format is: [nb, dx0, dy0, dx1, dy1, …] where nb is the count
 * of (dx, dy) displacement pairs.
 *
 * @param rank Zero-based record index
 * @return Pointer into the static table; do not free
 */
short * GetListMoves (int rank);

/**
 * @brief Return a pointer to icon-sequence record @p rank in the global table.
 *
 * The record format is: [nb, icon0, icon1, …] where nb is the frame count.
 *
 * @param rank Zero-based record index
 * @return Pointer into the static table; do not free
 */
short * GetListIcons (int rank);
