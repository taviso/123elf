/*
 *  libcaca     Colour ASCII-Art library
 *  Copyright © 2002—2018 Sam Hocevar <sam@hocevar.net>
 *              2007 Ben Wiley Sittler <bsittler@gmail.com>
 *              All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains functions for converting characters between
 *  various character sets.
 */
#include <curses.h>
#include "config.h"

#if !defined(__KERNEL__)
#   include <string.h>
#endif

#include "ttydraw.h"
#include "ttyint.h"

/*
 * UTF-8 handling
 */

static uint8_t const trailing[256] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

static uint32_t const offsets[6] =
{
    0x00000000UL, 0x00003080UL, 0x000E2080UL,
    0x03C82080UL, 0xFA082080UL, 0x82082080UL
};

/*
 * CP437 handling
 */

static uint32_t const cp437_lookup1[] =
{
    /* 0x01 - 0x0f: ☺ ☻ ♥ ♦ ♣ ♠ • ◘ ○ ◙ ♂ ♀ ♪ ♫ ☼ */
            0x263a, 0x263b, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
    0x25d8, 0x25cb, 0x25d9, 0x2642, 0x2640, 0x266a, 0x266b, 0x263c,
    /* 0x10 - 0x1f: ► ◄ ↕ ‼ ¶ § ▬ ↨ ↑ ↓ → ← ∟ ↔ ▲ ▼ */
    0x25ba, 0x25c4, 0x2195, 0x203c, 0xb6, 0xa7, 0x25ac, 0x21a8,
    0x2191, 0x2193, 0x2192, 0x2190, 0x221f, 0x2194, 0x25b2, 0x25bc
};

static uint32_t const cp437_lookup2[] =
{
    /* 0x7f: ⌂ */
    0x2302,
    /* 0x80 - 0x8f: Ç ü é â ä à å ç ê ë è ï î ì Ä Å */
    0xc7, 0xfc, 0xe9, 0xe2, 0xe4, 0xe0, 0xe5, 0xe7,
    0xea, 0xeb, 0xe8, 0xef, 0xee, 0xec, 0xc4, 0xc5,
    /* 0x90 - 0x9f: É æ Æ ô ö ò û ù ÿ Ö Ü ¢ £ ¥ ₧ ƒ */
    0xc9, 0xe6, 0xc6, 0xf4, 0xf6, 0xf2, 0xfb, 0xf9,
    0xff, 0xd6, 0xdc, 0xa2, 0xa3, 0xa5, 0x20a7, 0x192,
    /* 0xa0 - 0xaf: á í ó ú ñ Ñ ª º ¿ ⌐ ¬ ½ ¼ ¡ « » */
    0xe1, 0xed, 0xf3, 0xfa, 0xf1, 0xd1, 0xaa, 0xba,
    0xbf, 0x2310, 0xac, 0xbd, 0xbc, 0xa1, 0xab, 0xbb,
    /* 0xb0 - 0xbf: ░ ▒ ▓ │ ┤ ╡ ╢ ╖ ╕ ╣ ║ ╗ ╝ ╜ ╛ ┐ */
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
    0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
    /* 0xc0 - 0xcf: └ ┴ ┬ ├ ─ ┼ ╞ ╟ ╚ ╔ ╩ ╦ ╠ ═ ╬ ╧ */
    0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
    0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
    /* 0xd0 - 0xdf: ╨ ╤ ╥ ╙ ╘ ╒ ╓ ╫ ╪ ┘ ┌ █ ▄ ▌ ▐ ▀ */
    0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
    0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
    /* 0xe0 - 0xef: α ß Γ π Σ σ µ τ Φ Θ Ω δ ∞ φ ε ∩ */
    0x3b1, 0xdf, 0x393, 0x3c0, 0x3a3, 0x3c3, 0xb5, 0x3c4,
    0x3a6, 0x398, 0x3a9, 0x3b4, 0x221e, 0x3c6, 0x3b5, 0x2229,
    /* 0xf0 - 0xff: ≡ ± ≥ ≤ ⌠ ⌡ ÷ ≈ ° ∙ · √ ⁿ ² ■ <nbsp> */
    0x2261, 0xb1, 0x2265, 0x2264, 0x2320, 0x2321, 0xf7, 0x2248,
    0xb0, 0x2219, 0xb7, 0x221a, 0x207f, 0xb2, 0x25a0, 0xa0
};

/** \brief Convert a UTF-8 character to UTF-32.
 *
 *  Convert a UTF-8 character read from a string and return its value in
 *  the UTF-32 character set. If the second argument is not null, the total
 *  number of read bytes is written in it.
 *
 *  If a null byte was reached before the expected end of the UTF-8 sequence,
 *  this function returns zero and the number of read bytes is set to zero.
 *
 *  This function never fails, but its behaviour with illegal UTF-8 sequences
 *  is undefined.
 *
 *  \param s A string containing the UTF-8 character.
 *  \param bytes A pointer to a size_t to store the number of bytes in the
 *         character, or NULL.
 *  \return The corresponding UTF-32 character, or zero if the character
 *  is incomplete.
 */
uint32_t caca_utf8_to_utf32(char const *s, size_t *bytes)
{
    int todo = trailing[(int)(unsigned char)*s];
    int i = 0;
    uint32_t ret = 0;

    for(;;)
    {
        if(!*s)
        {
            if(bytes)
                *bytes = 0;
            return 0;
        }

        ret += ((uint32_t)(unsigned char)*s++) << (6 * (todo - i));

        if(todo == i++)
        {
            if(bytes)
                *bytes = i;
            return ret - offsets[todo];
        }
    }
}

/** \brief Convert a UTF-32 character to UTF-8.
 *
 *  Convert a UTF-32 character read from a string and write its value in
 *  the UTF-8 character set into the given buffer.
 *
 *  This function never fails, but its behaviour with illegal UTF-32 characters
 *  is undefined.
 *
 *  \param buf A pointer to a character buffer where the UTF-8 sequence will
 *  be written.
 *  \param ch The UTF-32 character.
 *  \return The number of bytes written.
 */
size_t caca_utf32_to_utf8(char *buf, uint32_t ch)
{
    static const uint8_t mark[7] =
    {
        0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
    };

    char *parser = buf;
    size_t bytes;

    if(ch < 0x80)
    {
        *parser++ = ch;
        return 1;
    }

    bytes = (ch < 0x800) ? 2 : (ch < 0x10000) ? 3 : 4;
    parser += bytes;

    switch(bytes)
    {
        case 4: *--parser = (ch | 0x80) & 0xbf; ch >>= 6;
        case 3: *--parser = (ch | 0x80) & 0xbf; ch >>= 6;
        case 2: *--parser = (ch | 0x80) & 0xbf; ch >>= 6;
    }
    *--parser = ch | mark[bytes];

    return bytes;
}

/** \brief Convert a UTF-32 character to CP437.
 *
 *  Convert a UTF-32 character read from a string and return its value in
 *  the CP437 character set, or "?" if the character has no equivalent.
 *
 *  This function never fails.
 *
 *  \param ch The UTF-32 character.
 *  \return The corresponding CP437 character, or "?" if not representable.
 */
uint8_t caca_utf32_to_cp437(uint32_t ch)
{
    unsigned int i;

    if(ch < 0x00000020)
        return '?';

    if(ch < 0x00000080)
        return ch;

    for(i = 0; i < sizeof(cp437_lookup1) / sizeof(*cp437_lookup1); i++)
        if(cp437_lookup1[i] == ch)
            return 0x01 + i;

    for(i = 0; i < sizeof(cp437_lookup2) / sizeof(*cp437_lookup2); i++)
        if(cp437_lookup2[i] == ch)
            return 0x7f + i;

    return '?';
}

/** \brief Convert a CP437 character to UTF-32.
 *
 *  Convert a CP437 character read from a string and return its value in
 *  the UTF-32 character set, or zero if the character is a CP437 control
 *  character.
 *
 *  This function never fails.
 *
 *  \param ch The CP437 character.
 *  \return The corresponding UTF-32 character, or zero if not representable.
 */
uint32_t caca_cp437_to_utf32(uint8_t ch)
{
    if(ch > 0x7f)
        return cp437_lookup2[ch - 0x7f];

    if(ch >= 0x20)
        return (uint32_t)ch;

    if(ch > 0)
        return cp437_lookup1[ch - 0x01];

    return 0x00000000;
}

/** \brief Convert a UTF-32 character to ASCII.
 *
 *  Convert a UTF-32 character into an ASCII character. When no equivalent
 *  exists, a graphically close equivalent is sought.
 *
 *  This function never fails, but its behaviour with illegal UTF-32 characters
 *  is undefined.
 *
 *  \param ch The UTF-32 character.
 *  \return The corresponding ASCII character, or a graphically close
 *  equivalent if found, or "?" if not representable.
 */
char caca_utf32_to_ascii(uint32_t ch)
{
    /* Standard ASCII */
    if(ch < 0x80)
        return ch;

    /* Fullwidth Forms */
    if(ch > 0x0000ff00 && ch < 0x0000ff5f)
        return ' ' + (ch - 0x0000ff00);

    switch (ch)
    {
    case 0x000000a0: /*   (nbsp) */
    case 0x00003000: /* 　 (ideographic space) */
        return ' ';
    case 0x000000a3: /* £ */
        return 'f';
    case 0x000000b0: /* ° */
        return '\'';
    case 0x000000b1: /* ± */
        return '#';
    case 0x000000b7: /* · */
    case 0x00002219: /* ∙ */
    case 0x000030fb: /* ・ */
        return '.';
    case 0x000003c0: /* π */
        return '*';
    case 0x00002018: /* ‘ */
    case 0x00002019: /* ’ */
        return '\'';
    case 0x0000201c: /* “ */
    case 0x0000201d: /* ” */
        return '"';
    case 0x00002190: /* ← */
        return '<';
    case 0x00002191: /* ↑ */
        return '^';
    case 0x00002192: /* → */
        return '>';
    case 0x00002193: /* ↓ */
        return 'v';
    case 0x00002260: /* ≠ */
        return '!';
    case 0x00002261: /* ≡ */
        return '=';
    case 0x00002264: /* ≤ */
        return '<';
    case 0x00002265: /* ≥ */
        return '>';
    case 0x000023ba: /* ⎺ */
    case 0x000023bb: /* ⎻ */
    case 0x000023bc: /* ⎼ */
    case 0x000023bd: /* ⎽ */
    case 0x00002500: /* ─ */
    case 0x00002550: /* ═ */
        return '-';
    case 0x00002502: /* │ */
    case 0x00002551: /* ║ */
        return '|';
    case 0x0000250c: /* ┌ */
    case 0x00002552: /* ╒ */
    case 0x00002553: /* ╓ */
    case 0x00002554: /* ╔ */
    case 0x00002514: /* └ */
    case 0x00002558: /* ╘ */
    case 0x00002559: /* ╙ */
    case 0x0000255a: /* ╚ */
    case 0x0000251c: /* ├ */
    case 0x0000255e: /* ╞ */
    case 0x0000255f: /* ╟ */
    case 0x00002560: /* ╠ */
    case 0x0000252c: /* ┬ */
    case 0x00002564: /* ╤ */
    case 0x00002565: /* ╥ */
    case 0x00002566: /* ╦ */
    case 0x00002534: /* ┴ */
    case 0x00002567: /* ╧ */
    case 0x00002568: /* ╨ */
    case 0x00002569: /* ╩ */
    case 0x0000253c: /* ┼ */
    case 0x0000256a: /* ╪ */
    case 0x0000256b: /* ╫ */
    case 0x0000256c: /* ╬ */
        return '+';
    case 0x00002510: /* ┐ */
    case 0x00002555: /* ╕ */
    case 0x00002556: /* ╖ */
    case 0x00002557: /* ╗ */
    case 0x00002518: /* ┘ */
    case 0x0000255b: /* ╛ */
    case 0x0000255c: /* ╜ */
    case 0x0000255d: /* ╝ */
    case 0x00002524: /* ┤ */
    case 0x00002561: /* ╡ */
    case 0x00002562: /* ╢ */
    case 0x00002563: /* ╣ */
        return '+';
    case 0x00002591: /* ░ */
    case 0x00002592: /* ▒ */
    case 0x00002593: /* ▓ */
    case 0x00002588: /* █ */
    case 0x0000258c: /* ▌ */
    case 0x00002590: /* ▐ */
    case 0x000025a0: /* ■ */
    case 0x000025ac: /* ▬ */
    case 0x000025ae: /* ▮ */
        return '#';
    case 0x00002580: /* ▀ */
        return '"';
    case 0x00002584: /* ▄ */
        return ',';
    case 0x000025c6: /* ◆ */
    case 0x00002666: /* ♦ */
        return '+';
    case 0x00002022: /* • */
    case 0x000025cb: /* ○ */
    case 0x000025cf: /* ● */
    case 0x00002603: /* ☃ */
    case 0x0000263c: /* ☼ */
        return 'o';
    case 0x0000301c: /* 〜 */
        return '~';
    }

    return '?';
}

/** \brief Tell whether a UTF-32 character is fullwidth.
 *
 *  Check whether the given UTF-32 character should be printed at twice
 *  the normal width (fullwidth characters). If the character is unknown
 *  or if its status cannot be decided, it is treated as a standard-width
 *  character.
 *
 *  This function never fails.
 *
 *  \param ch The UTF-32 character.
 *  \return 1 if the character is fullwidth, 0 otherwise.
 */
int caca_utf32_is_fullwidth(uint32_t ch)
{
    if(ch < 0x2e80) /* Standard stuff */
        return 0;
    if(ch < 0xa700) /* Japanese, Korean, CJK, Yi... */
        return 1;
    if(ch < 0xac00) /* Modified Tone Letters, Syloti Nagri */
        return 0;
    if(ch < 0xd800) /* Hangul Syllables */
        return 1;
    if(ch < 0xf900) /* Misc crap */
        return 0;
    if(ch < 0xfb00) /* More CJK */
        return 1;
    if(ch < 0xfe20) /* Misc crap */
        return 0;
    if(ch < 0xfe70) /* More CJK */
        return 1;
    if(ch < 0xff00) /* Misc crap */
        return 0;
    if(ch < 0xff61) /* Fullwidth forms */
        return 1;
    if(ch < 0xffe0) /* Halfwidth forms */
        return 0;
    if(ch < 0xffe8) /* More fullwidth forms */
        return 1;
    if(ch < 0x20000) /* Misc crap */
        return 0;
    if(ch < 0xe0000) /* More CJK */
        return 1;
    return 0;
}
