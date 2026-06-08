// Action.h
//

/**
 * @file action.h
 * @brief Animation state-machine interface for Blupi and enemy characters.
 *
 * Provides the core per-frame animation step function Action() together
 * with helpers for directional icon lookup and movement amplitude.
 * The animation data tables are compiled into action.cpp.
 */

/**
 * @brief Advance one character's animation by one frame.
 *
 * Given the current @p action and @p direct, updates @p phase / @p step,
 * selects the correct sprite (@p channel / @p icon) and screen offset
 * (@p pos / @p posZ), and triggers any scheduled sound effect (@p sound).
 *
 * @param action  Current ACTION_* code
 * @param direct  Current DIRECT_* code
 * @param phase   [in/out] Current animation phase counter
 * @param step    [in/out] Global step counter
 * @param channel [out] Image channel of the selected sprite
 * @param icon    [out] Icon index of the selected sprite
 * @param pos     [out] Pixel offset relative to the character's grid cell
 * @param posZ    [out] Vertical (Z) pixel offset for jump arcs
 * @param sound   [out] SOUND_* index to play this frame, or -1
 * @return TRUE when the animation sequence has completed (loop/transition point)
 */
extern
BOOL Action(short action, short direct,
			short &phase, short &step,
			short &channel, short &icon, POINT &pos, short &posZ,
			short &sound);

/**
 * @brief Rotate an icon to face a new direction.
 *
 * Adjusts @p icon to the correct directional variant for @p direct.
 * @return TRUE if the icon changed
 */
extern
BOOL Rotate(short &icon, short direct);

/**
 * @brief Return the DIRECT_* code encoded in an icon index.
 * @return DIRECT_* constant, or -1 if the icon is not directional
 */
extern
int GetIconDirect(short icon);

/**
 * @brief Return the movement amplitude (pixel displacement per step) for @p action.
 * @return Pixel count per animation step
 */
extern
int GetAmplitude(short action);

