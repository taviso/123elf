#ifndef __UNIXTERM_H
#define __UNIXTERM_H

#include <stdint.h>

#pragma pack(push, 1)
struct unixtermios {
    uint16_t    c_iflag;
    uint16_t    c_oflag;
    uint16_t    c_cflag;
    uint16_t    c_lflag;
    char        c_line;
    uint8_t     c_cc[8];
};
#pragma pack(pop)

#define UNIX_VMIN 4
#define UNIX_VTIME 5
#define UNIX_VSWITCH 7
#define UNIX_VMAGIC UNIX_VSWITCH
#define VMAGIC VSWITCH

// I've only figured out the parameters used by cfmakeraw()
#define UNIX_IGNBRK  0x0001
#define UNIX_BRKINT  0x0002
#define UNIX_PARMRK  0x0008
#define UNIX_ISTRIP  0x0020
#define UNIX_INLCR   0x0040
#define UNIX_IGNCR   0x0080
#define UNIX_ICRNL   0x0100
#define UNIX_IXON    0x0400

#define UNIX_OPOST   0x0001

#define UNIX_ECHO    0x0008
#define UNIX_ECHONL  0x0040
#define UNIX_ICANON  0x0002
#define UNIX_ISIG    0x0001
#define UNIX_IEXTEN  0x8000

#define UNIX_CSIZE   0x0030
#define UNIX_PARENB  0x0100

#define UNIX_CS8     0x0030

// These are used by set_raw_mode()
#define UNIX_TABDLY  0x1800

#endif
