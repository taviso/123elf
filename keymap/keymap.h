#ifndef __KEYMAP_H
#define __KEYMAP_H

#pragma pack(1)

typedef struct KEYINFO {
    uint16_t    keyval;
    uint16_t    kfun;
    union {
        struct KEYNODE {
            uint32_t nextkey_hi;
            uint32_t nextkey_lo;
        } node;
        struct KEYDATA {
            uint32_t next;
            uint32_t timeout;
        } data;
    };
    uint32_t next;
} KEYINFO, *PKEYINFO;

typedef struct KEYMAP {
    uint16_t magic;
    uint16_t version;
    uint32_t zero;
    uint32_t keytreesize;
    uint32_t keydatasize;
    uint32_t root;
} KEYMAP, *PKEYMAP;

#define MAX_ALTCAPS 8

typedef struct KEYDEF {
    const char *name;
    const char *cap;
    uint16_t kfun;
    const char *kseq;
    const char *altcaps[MAX_ALTCAPS];
} KEYDEF, *PKEYDEF;

// The offsets in the KEYINFO tree are encoded in an unusual way, these
// inline functions abstract away the ugly casting required to use them
// when traversing the tree.
//
// Given the base of a keytree and an offset, give me a new node pointer.
static inline PKEYINFO keyptr(const PKEYINFO base, uint32_t offset)
{
    if (offset == 0)
        return NULL;

    return (PKEYINFO)((uintptr_t)(base) + offset - sizeof(KEYMAP));
}
// Given the base of a keytree and an entry, return the array index.
static inline int keyidx(const PKEYINFO base, const PKEYINFO state)
{
    return ((uintptr_t)(state) - (uintptr_t)(base)) / sizeof (KEYINFO);
}
// Given the base of a keytree and an entry, return an offset suitable for
// storing as node ptr.
static inline int keyoff(const PKEYINFO base, const PKEYINFO state)
{
    return sizeof(KEYMAP) + (uintptr_t)(state) - (uintptr_t)(base);
}

#define KEY_TIMEOUT 0x100

#endif
