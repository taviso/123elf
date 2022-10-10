#ifndef __KFUN_H
#define __KFUN_H

#pragma pack(1)

enum {
    KFUN_HOME = 0x1801,
    KFUN_FIRST_CELL = 0x1802,
    KFUN_END = 0x1803,
    KFUN_UP = 0x1804,
    KFUN_DOWN = 0x1805,
    KFUN_LEFT = 0x1806,
    KFUN_RIGHT = 0x1807,
    KFUN_PG_UP = 0x1808,
    KFUN_PG_DN = 0x1809,
    KFUN_BIG_LEFT = 0x180a,
    KFUN_BIG_RIGHT = 0x180b,
    KFUN_TAB = 0x180c,
    KFUN_WINDOW = 0x180d,
    KFUN_NEXT_SHEET = 0x180e,
    KFUN_PREV_SHEET = 0x180f,
    KFUN_BREAK = 0x1810,
    KFUN_ESC = 0x1811,
    KFUN_RETURN = 0x1812,
    KFUN_INS = 0x1813,
    KFUN_DEL = 0x1814,
    KFUN_BACKSPACE = 0x1815,
    KFUN_HELP = 0x1816,
    KFUN_ABS = 0x1817,
    KFUN_GOTO = 0x1818,
    KFUN_CALC = 0x1819,
    KFUN_EDIT = 0x181a,
    KFUN_NAME = 0x181b,
    KFUN_ZOOM = 0x181c,
    KFUN_GRAPH = 0x181d,
    KFUN_TABLE = 0x181e,
    KFUN_QUERY = 0x181f,
    KFUN_UNDO = 0x1820,
    KFUN_COMPOSE = 0x1821,
    KFUN_FILE = 0x1822,
    KFUN_HAL = 0x1823,
    KFUN_ADDIN = 0x1824,
    KFUN_APP1 = 0x1825,
    KFUN_APP2 = 0x1826,
    KFUN_APP3 = 0x1827,
    KFUN_MAC_A = 0x1828,
    KFUN_MAC_B = 0x1829,
    KFUN_MAC_C = 0x182a,
    KFUN_MAC_D = 0x182b,
    KFUN_MAC_E = 0x182c,
    KFUN_MAC_F = 0x182d,
    KFUN_MAC_G = 0x182e,
    KFUN_MAC_H = 0x182f,
    KFUN_MAC_I = 0x1830,
    KFUN_MAC_J = 0x1831,
    KFUN_MAC_K = 0x1832,
    KFUN_MAC_L = 0x1833,
    KFUN_MAC_M = 0x1834,
    KFUN_MAC_N = 0x1835,
    KFUN_MAC_O = 0x1836,
    KFUN_MAC_P = 0x1837,
    KFUN_MAC_Q = 0x1838,
    KFUN_MAC_R = 0x1839,
    KFUN_MAC_S = 0x183a,
    KFUN_MAC_T = 0x183b,
    KFUN_MAC_U = 0x183c,
    KFUN_MAC_V = 0x183D,
    KFUN_MAC_W = 0x183e,
    KFUN_MAC_X = 0x183f,
    KFUN_MAC_Y = 0x1840,
    KFUN_MAC_Z = 0x1841,
    KFUN_RUN = 0x1842,
    KFUN_STEP = 0x1843,
    KFUN_RECORD = 0x1844,
    KFUN_FLUSH = 0x1845,
    KFUN_BELL = 0x1846,
    KFUN_SCROLL_LOCK = 0x184e,
    KFUN_REFRESH = 0x1864,
};

static inline const char * kfun_name(uint16_t kfun)
{
    switch (kfun) {
        case KFUN_HOME: return "{HOME}";
        case KFUN_FIRST_CELL: return "{FIRST CELL}";
        case KFUN_END: return "{END}";
        case KFUN_UP: return "{UP}";
        case KFUN_DOWN: return "{DOWN}";
        case KFUN_LEFT: return "{LEFT}";
        case KFUN_RIGHT: return "{RIGHT}";
        case KFUN_PG_UP: return "{PG UP}";
        case KFUN_PG_DN: return "{PG DN}";
        case KFUN_BIG_LEFT: return "{BIG LEFT}";
        case KFUN_BIG_RIGHT: return "{BIG RIGHT}";
        case KFUN_TAB: return "{TAB}";
        case KFUN_WINDOW: return "{WINDOW}";
        case KFUN_NEXT_SHEET: return "{NEXT SHEET}";
        case KFUN_PREV_SHEET: return "{PREV SHEET}";
        case KFUN_BREAK: return "{BREAK}";
        case KFUN_ESC: return "{ESC}";
        case KFUN_RETURN: return "{RETURN}";
        case KFUN_INS: return "{INS}";
        case KFUN_DEL: return "{DEL}";
        case KFUN_BACKSPACE: return "{BACKSPACE}";
        case KFUN_HELP: return "{HELP}";
        case KFUN_ABS: return "{ABS}";
        case KFUN_GOTO: return "{GOTO}";
        case KFUN_CALC: return "{CALC}";
        case KFUN_EDIT: return "{EDIT}";
        case KFUN_NAME: return "{NAME}";
        case KFUN_ZOOM: return "{ZOOM}";
        case KFUN_GRAPH: return "{GRAPH}";
        case KFUN_TABLE: return "{TABLE}";
        case KFUN_QUERY: return "{QUERY}";
        case KFUN_UNDO: return "{UNDO}";
        case KFUN_COMPOSE: return "{COMPOSE}";
        case KFUN_FILE: return "{FILE}";
        case KFUN_HAL: return "{HAL}";
        case KFUN_ADDIN: return "{ADDIN}";
        case KFUN_APP1: return "{APP1}";
        case KFUN_APP2: return "{APP2}";
        case KFUN_APP3: return "{APP3}";
        case KFUN_MAC_A: return "{MAC_A}";
        case KFUN_MAC_B: return "{MAC_B}";
        case KFUN_MAC_C: return "{MAC_C}";
        case KFUN_MAC_D: return "{MAC_D}";
        case KFUN_MAC_E: return "{MAC_E}";
        case KFUN_MAC_F: return "{MAC_F}";
        case KFUN_MAC_G: return "{MAC_G}";
        case KFUN_MAC_H: return "{MAC_H}";
        case KFUN_MAC_I: return "{MAC_I}";
        case KFUN_MAC_J: return "{MAC_J}";
        case KFUN_MAC_K: return "{MAC_K}";
        case KFUN_MAC_L: return "{MAC_L}";
        case KFUN_MAC_M: return "{MAC_M}";
        case KFUN_MAC_N: return "{MAC_N}";
        case KFUN_MAC_O: return "{MAC_O}";
        case KFUN_MAC_P: return "{MAC_P}";
        case KFUN_MAC_Q: return "{MAC_Q}";
        case KFUN_MAC_R: return "{MAC_R}";
        case KFUN_MAC_S: return "{MAC_S}";
        case KFUN_MAC_T: return "{MAC_T}";
        case KFUN_MAC_U: return "{MAC_U}";
        case KFUN_MAC_V: return "{MAC_V}";
        case KFUN_MAC_W: return "{MAC_W}";
        case KFUN_MAC_X: return "{MAC_X}";
        case KFUN_MAC_Y: return "{MAC_Y}";
        case KFUN_MAC_Z: return "{MAC_Z}";
        case KFUN_RUN: return "{RUN}";
        case KFUN_STEP: return "{STEP}";
        case KFUN_RECORD: return "{RECORD}";
        case KFUN_FLUSH: return "{FLUSH}";
        case KFUN_BELL: return "{BELL}";
        case KFUN_SCROLL_LOCK: return "{SCROLL LOCK}";
        case KFUN_REFRESH: return "{REFRESH}";

    }

    return NULL;
}

#endif
