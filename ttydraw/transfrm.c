/*
 *  libcaca     Colour ASCII-Art library
 *  Copyright © 2002—2018 Sam Hocevar <sam@hocevar.net>
 *              All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains horizontal and vertical flipping routines.
 */

#include "config.h"
#include "ttydraw.h"
#include "ttyint.h"

static uint32_t flipchar(uint32_t ch);
static uint32_t flopchar(uint32_t ch);
static uint32_t rotatechar(uint32_t ch);
static uint32_t leftchar(uint32_t ch);
static uint32_t rightchar(uint32_t ch);
static void leftpair(uint32_t pair[2]);
static void rightpair(uint32_t pair[2]);

int caca_invert(caca_canvas_t *cv)
{
    return -1;
}

int caca_flip(caca_canvas_t *cv)
{
    return -1;
}

int caca_flop(caca_canvas_t *cv)
{
    return -1;
}

int caca_rotate_180(caca_canvas_t *cv)
{
    return -1;
}

int caca_rotate_left(caca_canvas_t *cv)
{
    return -1;
}

int caca_rotate_right(caca_canvas_t *cv)
{
    return -1;
}

int caca_stretch_left(caca_canvas_t *cv)
{
    return -1;
}

int caca_stretch_right(caca_canvas_t *cv)
{
    return -1;
}

/* FIXME: as the lookup tables grow bigger, use a log(n) lookup instead
 * of linear lookup. */
static uint32_t flipchar(uint32_t ch)
{
    int i;

    static uint32_t const noflip[] =
    {
         /* ASCII */
         ' ', '"', '#', '\'', '-', '.', '*', '+', ':', '=', '0', '8',
         'A', 'H', 'I', 'M', 'O', 'T', 'U', 'V', 'W', 'X', 'Y', '^',
         '_', 'i', 'o', 'v', 'w', 'x', '|',
         /* CP437 and box drawing */
         0x2591, 0x2592, 0x2593, 0x2588, 0x2584, 0x2580, /* ░ ▒ ▓ █ ▄ ▀ */
         0x2500, 0x2501, 0x2503, 0x2503, 0x253c, 0x254b, /* ─ ━ │ ┃ ┼ ╋ */
         0x252c, 0x2534, 0x2533, 0x253b, 0x2566, 0x2569, /* ┬ ┴ ┳ ┻ ╦ ╩ */
         0x2550, 0x2551, 0x256c, /* ═ ║ ╬ */
         0x2575, 0x2577, 0x2579, 0x257b, /* ╵ ╷ ╹ ╻ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         '(', ')',
         '/', '\\',
         '<', '>',
         '[', ']',
         'b', 'd',
         'p', 'q',
         '{', '}',
         /* ASCII-Unicode */
         ';', 0x204f, /* ; ⁏ */
         '`', 0x00b4, /* ` ´ */
         ',', 0x02ce, /* , ˎ */
         '1', 0x07c1, /* 1 ߁ */
         'B', 0x10412,/* B 𐐒 */
         'C', 0x03fd, /* C Ͻ */
         'D', 0x15e1, /* D ᗡ */
         'E', 0x018e, /* E Ǝ */
         'J', 0x1490, /* J ᒐ */
         'L', 0x2143, /* L ⅃ */
         'N', 0x0418, /* N И */
         'P', 0x1040b,/* P 𐐋 */
         'R', 0x042f, /* R Я */
         'S', 0x01a7, /* S Ƨ */
         'c', 0x0254, /* c ɔ */
         'e', 0x0258, /* e ɘ */
         /* CP437 */
         0x258c, 0x2590, /* ▌ ▐ */
         0x2596, 0x2597, /* ▖ ▗ */
         0x2598, 0x259d, /* ▘ ▝ */
         0x2599, 0x259f, /* ▙ ▟ */
         0x259a, 0x259e, /* ▚ ▞ */
         0x259b, 0x259c, /* ▛ ▜ */
         0x25ba, 0x25c4, /* ► ◄ */
         0x2192, 0x2190, /* → ← */
         0x2310, 0xac,   /* ⌐ ¬ */
         /* Box drawing */
         0x250c, 0x2510, /* ┌ ┐ */
         0x2514, 0x2518, /* └ ┘ */
         0x251c, 0x2524, /* ├ ┤ */
         0x250f, 0x2513, /* ┏ ┓ */
         0x2517, 0x251b, /* ┗ ┛ */
         0x2523, 0x252b, /* ┣ ┫ */
         0x2552, 0x2555, /* ╒ ╕ */
         0x2558, 0x255b, /* ╘ ╛ */
         0x2553, 0x2556, /* ╓ ╖ */
         0x2559, 0x255c, /* ╙ ╜ */
         0x2554, 0x2557, /* ╔ ╗ */
         0x255a, 0x255d, /* ╚ ╝ */
         0x255e, 0x2561, /* ╞ ╡ */
         0x255f, 0x2562, /* ╟ ╢ */
         0x2560, 0x2563, /* ╠ ╣ */
         0x2574, 0x2576, /* ╴ ╶ */
         0x2578, 0x257a, /* ╸ ╺ */
         /* Misc Unicode */
         0x22f2, 0x22fa, /* ⋲ ⋺ */
         0x22f3, 0x22fb, /* ⋳ ⋻ */
         0x2308, 0x2309, /* ⌈ ⌉ */
         0x230a, 0x230b, /* ⌊ ⌋ */
         0x230c, 0x230d, /* ⌌ ⌍ */
         0x230e, 0x230f, /* ⌎ ⌏ */
         0x231c, 0x231d, /* ⌜ ⌝ */
         0x231e, 0x231f, /* ⌞ ⌟ */
         0x2326, 0x232b, /* ⌦ ⌫ */
         0x2329, 0x232a, /* 〈 〉 */
         0x2341, 0x2342, /* ⍁ ⍂ */
         0x2343, 0x2344, /* ⍃ ⍄ */
         0x2345, 0x2346, /* ⍅ ⍆ */
         0x2347, 0x2348, /* ⍇ ⍈ */
         0x233f, 0x2340, /* ⌿ ⍀ */
         0x239b, 0x239e, /* ⎛ ⎞ */
         0x239c, 0x239f, /* ⎜ ⎟ */
         0x239d, 0x23a0, /* ⎝ ⎠ */
         0x23a1, 0x23a4, /* ⎡ ⎤ */
         0x23a2, 0x23a5, /* ⎢ ⎥ */
         0x23a3, 0x23a6, /* ⎣ ⎦ */
         0x23a7, 0x23ab, /* ⎧ ⎫ */
         0x23a8, 0x23ac, /* ⎨ ⎬ */
         0x23a9, 0x23ad, /* ⎩ ⎭ */
         0x23b0, 0x23b1, /* ⎰ ⎱ */
         0x23be, 0x23cb, /* ⎾ ⏋ */
         0x23bf, 0x23cc, /* ⎿ ⏌ */
         0
    };

    for(i = 0; noflip[i]; i++)
        if(ch == noflip[i])
            return ch;

    for(i = 0; pairs[i]; i++)
        if(ch == pairs[i])
            return pairs[i ^ 1];

    return ch;
}

static uint32_t flopchar(uint32_t ch)
{
    int i;

    static uint32_t const noflop[] =
    {
         /* ASCII */
         ' ', '(', ')', '*', '+', '-', '0', '3', '8', ':', '<', '=',
         '>', 'B', 'C', 'D', 'E', 'H', 'I', 'K', 'O', 'X', '[', ']',
         'c', 'o', '{', '|', '}',
         /* CP437 and box drawing */
         0x2591, 0x2592, 0x2593, 0x2588, 0x258c, 0x2590, /* ░ ▒ ▓ █ ▌ ▐ */
         0x2500, 0x2501, 0x2503, 0x2503, 0x253c, 0x254b, /* ─ ━ │ ┃ ┼ ╋ */
         0x251c, 0x2524, 0x2523, 0x252b, 0x2560, 0x2563, /* ├ ┤ ┣ ┫ ╠ ╣ */
         0x2550, 0x2551, 0x256c, /* ═ ║ ╬ */
         0x2574, 0x2576, 0x2578, 0x257a, /* ╴ ╶ ╸ ╺ */
         /* Misc Unicode */
         0x22f2, 0x22fa, 0x22f3, 0x22fb, 0x2326, 0x232b, /* ⋲ ⋺ ⋳ ⋻ ⌦ ⌫ */
         0x2329, 0x232a, 0x2343, 0x2344, 0x2345, 0x2346, /* 〈 〉 ⍃ ⍄ ⍅ ⍆ */
         0x2347, 0x2348, 0x239c, 0x239f, 0x23a2, 0x23a5, /* ⍇ ⍈ ⎜ ⎟ ⎢ ⎥ */
         0x23a8, 0x23ac, /* ⎨ ⎬ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         '/', '\\',
         'M', 'W',
         ',', '`',
         'b', 'p',
         'd', 'q',
         'p', 'q',
         'f', 't',
         '.', '\'',
         /* ASCII-Unicode */
         '_', 0x203e, /* _ ‾ */
         '!', 0x00a1, /* ! ¡ */
         'A', 0x2200, /* A ∀ */
         'J', 0x1489, /* J ᒉ */
         'L', 0x0413, /* L Г */
         'N', 0x0418, /* N И */
         'P', 0x042c, /* P Ь */
         'R', 0x0281, /* R ʁ */
         'S', 0x01a7, /* S Ƨ */
         'U', 0x0548, /* U Ո */
         'V', 0x039b, /* V Λ */
         'Y', 0x2144, /* Y ⅄ */
         'h', 0x03bc, /* h μ */
         'i', 0x1d09, /* i ᴉ */
         'j', 0x1e37, /* j ḷ */
         'l', 0x0237, /* l ȷ */
         'v', 0x028c, /* v ʌ */
         'w', 0x028d, /* w ʍ */
         'y', 0x03bb, /* y λ */
         /* Not perfect, but better than nothing */
         '"', 0x201e, /* " „ */
         'm', 0x026f, /* m ɯ */
         'n', 'u',
         /* CP437 */
         0x2584, 0x2580, /* ▄ ▀ */
         0x2596, 0x2598, /* ▖ ▘ */
         0x2597, 0x259d, /* ▗ ▝ */
         0x2599, 0x259b, /* ▙ ▛ */
         0x259f, 0x259c, /* ▟ ▜ */
         0x259a, 0x259e, /* ▚ ▞ */
         /* Box drawing */
         0x250c, 0x2514, /* ┌ └ */
         0x2510, 0x2518, /* ┐ ┘ */
         0x252c, 0x2534, /* ┬ ┴ */
         0x250f, 0x2517, /* ┏ ┗ */
         0x2513, 0x251b, /* ┓ ┛ */
         0x2533, 0x253b, /* ┳ ┻ */
         0x2554, 0x255a, /* ╔ ╚ */
         0x2557, 0x255d, /* ╗ ╝ */
         0x2566, 0x2569, /* ╦ ╩ */
         0x2552, 0x2558, /* ╒ ╘ */
         0x2555, 0x255b, /* ╕ ╛ */
         0x2564, 0x2567, /* ╤ ╧ */
         0x2553, 0x2559, /* ╓ ╙ */
         0x2556, 0x255c, /* ╖ ╜ */
         0x2565, 0x2568, /* ╥ ╨ */
         0x2575, 0x2577, /* ╵ ╷ */
         0x2579, 0x257b, /* ╹ ╻ */
         /* Misc Unicode */
         0x2308, 0x230a, /* ⌈ ⌊ */
         0x2309, 0x230b, /* ⌉ ⌋ */
         0x230c, 0x230e, /* ⌌ ⌎ */
         0x230d, 0x230f, /* ⌍ ⌏ */
         0x231c, 0x231e, /* ⌜ ⌞ */
         0x231d, 0x231f, /* ⌝ ⌟ */
         0x2341, 0x2342, /* ⍁ ⍂ */
         0x233f, 0x2340, /* ⌿ ⍀ */
         0x239b, 0x239d, /* ⎛ ⎝ */
         0x239e, 0x23a0, /* ⎞ ⎠ */
         0x23a1, 0x23a3, /* ⎡ ⎣ */
         0x23a4, 0x23a6, /* ⎤ ⎦ */
         0x23a7, 0x23a9, /* ⎧ ⎩ */
         0x23ab, 0x23ad, /* ⎫ ⎭ */
         0x23b0, 0x23b1, /* ⎰ ⎱ */
         0x23be, 0x23bf, /* ⎾ ⎿ */
         0x23cb, 0x23cc, /* ⏋ ⏌ */
         0
    };

    for(i = 0; noflop[i]; i++)
        if(ch == noflop[i])
            return ch;

    for(i = 0; pairs[i]; i++)
        if(ch == pairs[i])
            return pairs[i ^ 1];

    return ch;
}

static uint32_t rotatechar(uint32_t ch)
{
    int i;

    static uint32_t const norotate[] =
    {
         /* ASCII */
         ' ', '*', '+', '-', '/', '0', '8', ':', '=', 'H', 'I', 'N',
         'O', 'S', 'X', 'Z', '\\', 'o', 's', 'x', 'z', '|',
         /* Unicode */
         0x2591, 0x2592, 0x2593, 0x2588, 0x259a, 0x259e, /* ░ ▒ ▓ █ ▚ ▞ */
         0x2500, 0x2501, 0x2503, 0x2503, 0x253c, 0x254b, /* ─ ━ │ ┃ ┼ ╋ */
         0x2550, 0x2551, 0x256c, /* ═ ║ ╬ */
         /* Misc Unicode */
         0x233f, 0x2340, 0x23b0, 0x23b1, /* ⌿ ⍀ ⎰ ⎱ */
         0
    };

    static uint32_t const pairs[] =
    {
         /* ASCII */
         '(', ')',
         '<', '>',
         '[', ']',
         '{', '}',
         '.', '\'',
         '6', '9',
         'M', 'W',
         'b', 'q',
         'd', 'p',
         'n', 'u',
         /* ASCII-Unicode */
         '_', 0x203e, /* _ ‾ */
         ',', 0x00b4, /* , ´ */
         ';', 0x061b, /* ; ؛ */
         '`', 0x02ce, /* ` ˎ */
         '&', 0x214b, /* & ⅋ */
         '!', 0x00a1, /* ! ¡ */
         '?', 0x00bf, /* ? ¿ */
         '3', 0x0190, /* 3 Ɛ */
         '4', 0x152d, /* 4 ᔭ */
         'A', 0x2200, /* A ∀ */
         'B', 0x10412,/* B 𐐒 */
         'C', 0x03fd, /* C Ͻ */
         'D', 0x15e1, /* D ᗡ */
         'E', 0x018e, /* E Ǝ */
         'F', 0x2132, /* F Ⅎ -- 0x07c3 looks better, but is RTL */
         'G', 0x2141, /* G ⅁ */
         'J', 0x148b, /* J ᒋ */
         'L', 0x2142, /* L ⅂ */
         'P', 0x0500, /* P Ԁ */
         'Q', 0x038c, /* Q Ό */
         'R', 0x1d1a, /* R ᴚ */
         'T', 0x22a5, /* T ⊥ */
         'U', 0x0548, /* U Ո */
         'V', 0x039b, /* V Λ */
         'Y', 0x2144, /* Y ⅄ */
         'a', 0x0250, /* a ɐ */
         'c', 0x0254, /* c ɔ */
         'e', 0x01dd, /* e ǝ */
         'f', 0x025f, /* f ɟ */
         'g', 0x1d77, /* g ᵷ */
         'h', 0x0265, /* h ɥ */
         'i', 0x1d09, /* i ᴉ */
         'j', 0x1e37, /* j ḷ */
         'k', 0x029e, /* k ʞ */
         'l', 0x0237, /* l ȷ */
         'm', 0x026f, /* m ɯ */
         'r', 0x0279, /* r ɹ */
         't', 0x0287, /* t ʇ */
         'v', 0x028c, /* v ʌ */
         'w', 0x028d, /* w ʍ */
         'y', 0x028e, /* y ʎ */
         /* Unicode-ASCII to match third-party software */
         0x0183, 'g', /* ƃ g */
         0x0259, 'e', /* ə e */
         0x027e, 'j', /* ɾ j */
         0x02d9, '.', /* ˙ . */
         0x05df, 'l', /* ן l */
         /* Not perfect, but better than nothing */
         '"', 0x201e, /* " „ */
         /* Misc Unicode */
         0x00e6, 0x1d02, /* æ ᴂ */
         0x0153, 0x1d14, /* œ ᴔ */
         0x03b5, 0x025c, /* ε ɜ */
         0x025b, 0x025c, /* ɛ ɜ */
         /* CP437 */
         0x258c, 0x2590, /* ▌ ▐ */
         0x2584, 0x2580, /* ▄ ▀ */
         0x2596, 0x259d, /* ▖ ▝ */
         0x2597, 0x2598, /* ▗ ▘ */
         0x2599, 0x259c, /* ▙ ▜ */
         0x259f, 0x259b, /* ▟ ▛ */
         /* Box drawing */
         0x250c, 0x2518, /* ┌ ┘ */
         0x2510, 0x2514, /* ┐ └ */
         0x251c, 0x2524, /* ├ ┤ */
         0x252c, 0x2534, /* ┬ ┴ */
         0x250f, 0x251b, /* ┏ ┛ */
         0x2513, 0x2517, /* ┓ ┗ */
         0x2523, 0x252b, /* ┣ ┫ */
         0x2533, 0x253b, /* ┳ ┻ */
         0x2554, 0x255d, /* ╔ ╝ */
         0x2557, 0x255a, /* ╗ ╚ */
         0x2560, 0x2563, /* ╠ ╣ */
         0x2566, 0x2569, /* ╦ ╩ */
         0x2552, 0x255b, /* ╒ ╛ */
         0x2555, 0x2558, /* ╕ ╘ */
         0x255e, 0x2561, /* ╞ ╡ */
         0x2564, 0x2567, /* ╤ ╧ */
         0x2553, 0x255c, /* ╓ ╜ */
         0x2556, 0x2559, /* ╖ ╙ */
         0x255f, 0x2562, /* ╟ ╢ */
         0x2565, 0x2568, /* ╥ ╨ */
         0x2574, 0x2576, /* ╴ ╶ */
         0x2575, 0x2577, /* ╵ ╷ */
         0x2578, 0x257a, /* ╸ ╺ */
         0x2579, 0x257b, /* ╹ ╻ */
         /* Misc Unicode */
         0x22f2, 0x22fa, /* ⋲ ⋺ */
         0x22f3, 0x22fb, /* ⋳ ⋻ */
         0x2308, 0x230b, /* ⌈ ⌋ */
         0x2309, 0x230a, /* ⌉ ⌊ */
         0x230c, 0x230f, /* ⌌ ⌏ */
         0x230d, 0x230e, /* ⌍ ⌎ */
         0x231c, 0x231f, /* ⌜ ⌟ */
         0x231d, 0x231e, /* ⌝ ⌞ */
         0x2326, 0x232b, /* ⌦ ⌫ */
         0x2329, 0x232a, /* 〈 〉 */
         0x2343, 0x2344, /* ⍃ ⍄ */
         0x2345, 0x2346, /* ⍅ ⍆ */
         0x2347, 0x2348, /* ⍇ ⍈ */
         0x239b, 0x23a0, /* ⎛ ⎠ */
         0x239c, 0x239f, /* ⎜ ⎟ */
         0x239e, 0x239d, /* ⎞ ⎝ */
         0x23a1, 0x23a6, /* ⎡ ⎦ */
         0x23a2, 0x23a5, /* ⎢ ⎥ */
         0x23a4, 0x23a3, /* ⎤ ⎣ */
         0x23a7, 0x23ad, /* ⎧ ⎭ */
         0x23a8, 0x23ac, /* ⎨ ⎬ */
         0x23ab, 0x23a9, /* ⎫ ⎩ */
         0x23be, 0x23cc, /* ⎾ ⏌ */
         0x23cb, 0x23bf, /* ⏋ ⎿ */
         0
    };

    for(i = 0; norotate[i]; i++)
        if(ch == norotate[i])
            return ch;

    for(i = 0; pairs[i]; i++)
        if(ch == pairs[i])
            return pairs[i ^ 1];

    return ch;
}

static uint32_t const leftright2[] =
{
    /* ASCII */
    '/', '\\',
    '|', '-',
    '|', '_', /* This is all right because there was already a '|' before */
    /* ASCII-Unicode */
    '|', 0x203e, /* | ‾ */
    /* Misc Unicode */
    0x2571, 0x2572, /* ╱ ╲ */
    /* Box drawing */
    0x2500, 0x2502, /* ─ │ */
    0x2501, 0x2503, /* ━ ┃ */
    0x2550, 0x2551, /* ═ ║ */
    0, 0
};

static uint32_t const leftright4[] =
{
    /* ASCII */
    '<', 'v', '>', '^',
    ',', '.', '\'', '`',
    /* ASCII / Unicode */
    '(', 0x203f, ')', 0x2040,       /* ( ‿ ) ⁀ */
    /* Misc Unicode */
    0x256d, 0x2570, 0x256f, 0x256e, /* ╭ ╰ ╯ ╮ */
    /* CP437 */
    0x258c, 0x2584, 0x2590, 0x2580, /* ▌ ▄ ▐ ▀ */
    0x2596, 0x2597, 0x259d, 0x2598, /* ▖ ▗ ▝ ▘ */
    0x2599, 0x259f, 0x259c, 0x259b, /* ▙ ▟ ▜ ▛ */
    /* Box drawing */
    0x250c, 0x2514, 0x2518, 0x2510, /* ┌ └ ┘ ┐ */
    0x250f, 0x2517, 0x251b, 0x2513, /* ┏ ┗ ┛ ┓ */
    0x251c, 0x2534, 0x2524, 0x252c, /* ├ ┴ ┤ ┬ */
    0x2523, 0x253b, 0x252b, 0x2533, /* ┣ ┻ ┫ ┳ */
    0x2552, 0x2559, 0x255b, 0x2556, /* ╒ ╙ ╛ ╖ */
    0x2553, 0x2558, 0x255c, 0x2555, /* ╓ ╘ ╜ ╕ */
    0x2554, 0x255a, 0x255d, 0x2557, /* ╔ ╚ ╝ ╗ */
    0x255e, 0x2568, 0x2561, 0x2565, /* ╞ ╨ ╡ ╥ */
    0x255f, 0x2567, 0x2562, 0x2564, /* ╟ ╧ ╢ ╤ */
    0x2560, 0x2569, 0x2563, 0x2566, /* ╠ ╩ ╣ ╦ */
    0x2574, 0x2577, 0x2576, 0x2575, /* ╴ ╷ ╶ ╵ */
    0x2578, 0x257b, 0x257a, 0x2579, /* ╸ ╻ ╺ ╹ */
    0, 0, 0, 0
};

static uint32_t leftchar(uint32_t ch)
{
    int i;

    for(i = 0; leftright2[i]; i++)
        if(ch == leftright2[i])
            return leftright2[(i & ~1) | ((i + 1) & 1)];

    for(i = 0; leftright4[i]; i++)
        if(ch == leftright4[i])
            return leftright4[(i & ~3) | ((i + 1) & 3)];

    return ch;
}

static uint32_t rightchar(uint32_t ch)
{
    int i;

    for(i = 0; leftright2[i]; i++)
        if(ch == leftright2[i])
            return leftright2[(i & ~1) | ((i - 1) & 1)];

    for(i = 0; leftright4[i]; i++)
        if(ch == leftright4[i])
            return leftright4[(i & ~3) | ((i - 1) & 3)];

    return ch;
}

static uint32_t const leftright2x2[] =
{
    /* ASCII / Unicode */
    '-', '-', 0x4e28, CACA_MAGIC_FULLWIDTH, /* -- 丨 */
    '|', '|', 0x2f06, CACA_MAGIC_FULLWIDTH, /* || ⼆ */
    /* Unicode */
    0x2584, 0x2580, 0x2580, 0x2584, /* ▄▀ ▀▄ */
    0, 0, 0, 0
};

static uint32_t const leftright2x4[] =
{
    /* ASCII */
    ':', ' ', '.', '.', ' ', ':', '\'', '\'',
    /* ASCII / Unicode */
    ' ', '`', 0x00b4, ' ', 0x02ce, ' ', ' ', ',',      /*  ` ´  ˎ   , */
    ' ', '`', '\'',   ' ', '.',    ' ', ' ', ',',      /* fallback ASCII */
    '`', ' ', ',', ' ', ' ', 0x00b4, ' ', 0x02ce,      /*  ` ,   ˎ  ´ */
    '`', ' ', ',', ' ', ' ', '.',    ' ', '\'',        /* fallback ASCII */
    '/', ' ', '-', 0x02ce, ' ', '/', '`', '-',         /* /  -ˎ  / `- */
    '/', ' ', '-', '.',    ' ', '/', '\'', '-',        /* fallback ASCII */
    '\\', ' ', ',', '-', ' ', '\\', '-', 0x00b4,       /* \  ,-  \ -´ */
    '\\', ' ', '.', '-', ' ', '\\', '-', '\'',         /* fallback ASCII */
    '\\', ' ', '_', ',', ' ', '\\', 0x00b4, 0x203e,    /* \  _,  \ ´‾ */
    '\\', '_', '_', '/', 0x203e, '\\', '/', 0x203e,    /* \_ _/ ‾\ /‾ */
    '_', '\\', 0x203e, '/', '\\', 0x203e, '/', '_',    /* _\ ‾/ \‾ /_ */
    '|', ' ', '_', '_', ' ', '|', 0x203e, 0x203e,      /* |  __  | ‾‾ */
    '_', '|', 0x203e, '|', '|', 0x203e, '|', '_',      /* _| ‾| |‾ |_ */
    '|', '_', '_', '|', 0x203e, '|', '|', 0x203e,      /* |_ _| ‾| |‾ */
    '_', ' ', ' ', 0x2577, ' ', 0x203e, 0x2575, ' ',   /* _   ╷  ‾ ╵  */
    ' ', '_', ' ', 0x2575, 0x203e, ' ', 0x2577, ' ',   /*  _  ╵ ‾  ╷  */
    '.', '_', '.', 0x2575, 0x203e, '\'', 0x2577, '\'', /* ._ .╵ ‾' ╷' */
    '(', '_', 0x203f, '|', 0x203e, ')', '|', 0x2040,   /* (_ ‿| ‾) |⁀ */
    '(', 0x203e, '|', 0x203f, '_', ')', 0x2040, '|',   /* (‾ |‿ _) ⁀| */
    '\\', '/', 0xff1e, CACA_MAGIC_FULLWIDTH,
            '/', '\\', 0xff1c, CACA_MAGIC_FULLWIDTH,  /* \/ ＞ /\ ＜ */
    ')', ' ', 0xfe35, CACA_MAGIC_FULLWIDTH,
            ' ', '(', 0xfe36, CACA_MAGIC_FULLWIDTH,   /* )  ︵  ( ︶ */
    '}', ' ', 0xfe37, CACA_MAGIC_FULLWIDTH,
            ' ', '{', 0xfe38, CACA_MAGIC_FULLWIDTH,   /* }  ︷  { ︸ */
    /* Not perfect, but better than nothing */
    '(', ' ', 0x02ce, ',', ' ', ')', 0x00b4, '`',      /* (  ˎ,  ) ´` */
    ' ', 'v', '>', ' ', 0x028c, ' ', ' ', '<',         /*  v >  ʌ   < */
    ' ', 'V', '>', ' ', 0x039b, ' ', ' ', '<',         /*  V >  Λ   < */
    'v', ' ', '>', ' ', ' ', 0x028c, ' ', '<',         /* v  >   ʌ  < */
    'V', ' ', '>', ' ', ' ', 0x039b, ' ', '<',         /* V  >   Λ  < */
    '\\', '|', 0xff1e, CACA_MAGIC_FULLWIDTH,
            '|', '\\', 0xff1c, CACA_MAGIC_FULLWIDTH,  /* \| ＞ |\ ＜ */
    '|', '/', 0xff1e, CACA_MAGIC_FULLWIDTH,
            '/', '|', 0xff1c, CACA_MAGIC_FULLWIDTH,   /* |/ ＞ /| ＜ */
    /* Unicode */
    0x2584, ' ', ' ', 0x2584, ' ', 0x2580, 0x2580, ' ',       /* ▄   ▄  ▀ ▀  */
    0x2588, ' ', 0x2584, 0x2584, ' ', 0x2588, 0x2580, 0x2580, /* █  ▄▄  █ ▀▀ */
    0x2588, 0x2584, 0x2584, 0x2588,
            0x2580, 0x2588, 0x2588, 0x2580,                   /* █▄ ▄█ ▀█ █▀ */
    /* TODO: Braille */
    /* Not perfect, but better than nothing */
    0x2591, ' ', 0x28e4, 0x28e4, ' ', 0x2591, 0x281b, 0x281b, /* ░  ⣤⣤  ░ ⠛⠛ */
    0x2592, ' ', 0x28f6, 0x28f6, ' ', 0x2592, 0x283f, 0x283f, /* ▒  ⣶⣶  ▒ ⠿⠿ */
    0, 0, 0, 0, 0, 0, 0, 0
};

static void leftpair(uint32_t pair[2])
{
    int i;

    for(i = 0; leftright2x2[i]; i += 2)
        if(pair[0] == leftright2x2[i] && pair[1] == leftright2x2[i + 1])
        {
            pair[0] = leftright2x2[(i & ~3) | ((i + 2) & 3)];
            pair[1] = leftright2x2[((i & ~3) | ((i + 2) & 3)) + 1];
            return;
        }

    for(i = 0; leftright2x4[i]; i += 2)
        if(pair[0] == leftright2x4[i] && pair[1] == leftright2x4[i + 1])
        {
            pair[0] = leftright2x4[(i & ~7) | ((i + 2) & 7)];
            pair[1] = leftright2x4[((i & ~7) | ((i + 2) & 7)) + 1];
            return;
        }
}

static void rightpair(uint32_t pair[2])
{
    int i;

    for(i = 0; leftright2x2[i]; i += 2)
        if(pair[0] == leftright2x2[i] && pair[1] == leftright2x2[i + 1])
        {
            pair[0] = leftright2x2[(i & ~3) | ((i - 2) & 3)];
            pair[1] = leftright2x2[((i & ~3) | ((i - 2) & 3)) + 1];
            return;
        }

    for(i = 0; leftright2x4[i]; i += 2)
        if(pair[0] == leftright2x4[i] && pair[1] == leftright2x4[i + 1])
        {
            pair[0] = leftright2x4[(i & ~7) | ((i - 2) & 7)];
            pair[1] = leftright2x4[((i & ~7) | ((i - 2) & 7)) + 1];
            return;
        }
}

