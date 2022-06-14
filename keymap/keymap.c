#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <term.h>
#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stddef.h>

#include "keymap.h"

// It is safe to rearrange and change this array.
//  - name is just a human readable description of the key sequence,
//    like "Ctrl+PgUp", it is not used by 1-2-3.
//  - cap is a terminfo capability, you can look them up in terminfo(5).
//  - kfun is a 1-2-3 function id you want this mapped to, like {HOME}.
//
// You can have multiple sequences mapped to the same kfun, i.e. you can have
// Alt+F8 and Alt+F9 both be {HELP}.
// You cannot have multiple kfuns mapped to the same key sequence, i.e.
// Alt+F8 cannot be {HELP} and {UNDO}.
static KEYDEF keydefs[] = {
    { "Home",               "khome",    KFUN_HOME },
    { "End",                "kend",     KFUN_END },
    { "Up Arrow",           "kcuu1",    KFUN_UP },
    { "Down Arrow",         "kcud1",    KFUN_DOWN },
    { "Left Arrow",         "kcub1",    KFUN_LEFT },
    { "Right Arrow",        "kcuf1",    KFUN_RIGHT },
    { "Pg Up",              "kpp",      KFUN_PG_UP },
    { "Pg Dn",              "knp",      KFUN_PG_DN },
    { "Ctrl Left Arrow",    "kLFT5",    KFUN_BIG_LEFT },
    { "Ctrl Right Arrow",   "kRIT5",    KFUN_BIG_RIGHT },
    { "Ctrl Pg Up",         "kPRV5",    KFUN_NEXT_SHEET, .altcaps = { "kf20" } },
    { "Ctrl Pg Dn",         "kNXT5",    KFUN_PREV_SHEET, .altcaps = { "kf19" } },
    { "Ctrl Home",          "kHOM5",    KFUN_FIRST_CELL },
    { "Ctrl End",           "kEND5",    KFUN_FILE },
    { "Tab",                "ht",       KFUN_TAB },
    { "Backspace",          "kbs",      KFUN_BACKSPACE },
    { "Enter",              "kent",     KFUN_RETURN },
    { "Del",                "kdch1",    KFUN_DEL },
    { "Ins",                "kich1",    KFUN_INS },
    { "F12",                "kf12",     KFUN_SCROLL_LOCK },
    { "F1",                 "kf1",      KFUN_HELP },
    { "F2",                 "kf2",      KFUN_EDIT },
    { "F3",                 "kf3",      KFUN_NAME },
    { "F4",                 "kf4",      KFUN_ABS },
    { "F5",                 "kf5",      KFUN_GOTO },
    { "F6",                 "kf6",      KFUN_WINDOW },
    { "F7",                 "kf7",      KFUN_QUERY },
    { "F8",                 "kf8",      KFUN_TABLE },
    { "F9",                 "kf9",      KFUN_CALC },
    { "F10",                "kf10",     KFUN_GRAPH },
    { "F11",                "kf11",     KFUN_ESC },
    { "Ctrl-F10",           "kf34",     KFUN_GRAPH },   // Alternative {GRAPH}
    { "Alt-F1",             "kf49",     KFUN_COMPOSE },
    { "Ctrl-F1",            "kf25",     KFUN_COMPOSE }, // Alternative {COMPOSE}
    { "Alt-F2",             "kf50",     KFUN_RECORD },
    { "Ctrl-F2",            "kf26",     KFUN_RECORD },  // Alternative {RECORD}
    { "Alt-F3",             "kf51",     KFUN_RUN },
    { "Ctrl-F3",            "kf27",     KFUN_RUN },     // Alternative {RUN}
    { "Alt-F4",             "kf52",     KFUN_UNDO },
    { "Ctrl-F4",            "kf28",     KFUN_UNDO },    // Alternative {UNDO}
    { "Alt-F6",             "kf54",     KFUN_ZOOM },
    { "Ctrl-F6",            "kf29",     KFUN_ZOOM },    // Alternative {ZOOM}
};

// This adds room for a new node and returns the new node offset. The size
// pointer would normally be &header.keytreesize. If you had a pointer you
// were using to navigate the tree, pass it as state to have it adjusted
// (in case realloc moves it).
static int key_new_node(PKEYINFO *base, PKEYINFO *state, uint32_t *keytreesize)
{
    PKEYINFO newnode;
    int newoff;
    int stateidx;

    // Optionally fixup a cursor to handle a moved base.
    if (state) stateidx = keyidx(*base, *state);

    *base = realloc(*base, *keytreesize + sizeof(KEYINFO));

    // Find out where the new node is so we can zero it.
    newnode = (PKEYINFO)((char *)(*base) + *keytreesize);

    // Initialize the new node.
    newoff = keyoff(*base, memset(newnode, 0, sizeof *newnode));

    // Record the new size.
    *keytreesize += sizeof **base;

    // Fixup the cursor pointer.
    if (state) *state = &(*base)[stateidx];

    // Return the new index.
    return newoff;
}

// This is used to sort the definitions array to try to compact the
// tree.
// FIXME: I don't really know what the optimal way to sort it is yet.
static int compare_keyseq(const void *a, const void *b)
{
    const struct KEYDEF *x = a;
    const struct KEYDEF *y = b;

    // Put undefined sequences at the end.
    if (x->kseq == NULL && y->kseq == NULL)
        return 0;
    if (x->kseq == NULL)
        return -1;
    if (y->kseq == NULL)
        return 1;

    return strcmp(x->kseq, y->kseq);
}


static bool append_key_sequence(PKEYINFO *base,
                                PKEYMAP hdr,
                                const char *keyseq,
                                uint16_t kfun)
{
    if (!base || !kfun) {
        errx(EXIT_FAILURE, "The parameters were invalid.");
    }

    // This can happen if terminfo doesn't know a key sequence.
    if (!keyseq || *keyseq == '\0') {
        return false;
    }

    // If this is an empty base we can allocate a new one.
    if (*base == NULL) {
        key_new_node(base, NULL, &hdr->keytreesize);
    }

    // We start at root, and see how far we can get in the existing
    // tree.
    for (PKEYINFO root = *base; root;) {
        // This means wait for a timeout before continuing.
        if (root->keyval == KEY_TIMEOUT) {
            // This is not the end, and there's a next pointer.
            if (*keyseq && root->data.next) {
                root = keyptr(*base, root->data.next);
                continue;
            }
            // This is not the end, and theres no next pointer.
            if (*keyseq && !root->data.next) {
                // We need to add the next pointer.
                int nextoff = key_new_node(base, &root, &hdr->keytreesize);
                root->data.next = nextoff;

                // Now switch to the new node.
                root = keyptr(*base, nextoff);
                continue;
            }
            // We are at the end, so we want to take this node.
            if (root->kfun) {
                errx(EXIT_FAILURE, "Duplicate key sequences requested.");
            }

            // Okay, this node belongs to us now.
            root->kfun = kfun;
            return true;
        }

        // This is an empty node that needs to be filled.
        if (root->keyval == 0) {
            root->keyval = *keyseq;
        }

        // This is the next node in our key sequence.
        if (*keyseq == root->keyval) {
            // Advance sequence to next character. If this is nul
            // then thats the end of the sequence.
            ++keyseq;

            // This seems like an error, this keysequence is already defined?
            if (root->kfun && !*keyseq) {
                // This seems harmless, just adding the same sequence twice.
                if (root->kfun == kfun) {
                    return true;
                }
                // This is not harmless, ignoring the request.
                return false;
            }

            // If this node already has a kfun, then that means
            // another sequence shares this prefix. We need to inject
            // a timeout sequence.
            if (root->kfun) {
                int nextoff = key_new_node(base, &root, &hdr->keytreesize);

                // Create the new node.
                keyptr(*base, nextoff)->keyval       = KEY_TIMEOUT;
                keyptr(*base, nextoff)->kfun         = root->kfun;
                keyptr(*base, nextoff)->data.timeout = 1;

                // Now make sure this node makes sense.
                root->kfun = 0;
                root->next = nextoff;

                // Move to the next node.
                root            = keyptr(*base, nextoff);
                continue;
            }

            // This seems like a pointer to a timeout.
            if (root->kfun == 0 && root->next) {
                root = keyptr(*base, root->next);
                continue;
            }

            // It seems like nobody else wants this, so we can claim it.
            if (root->kfun == 0 && !root->next && !*keyseq) {
                root->kfun = kfun;
                return true;
            }

            if (root->kfun == 0 && !root->next && *keyseq) {
                int nextoff = key_new_node(base, &root, &hdr->keytreesize);
                root->next = nextoff;
                root = keyptr(*base, nextoff);
                continue;
            }

            // Not reached.
            assert(0);
        }

        // This is not the right node, we do a binary search for the correct
        // one.
        if (*keyseq > root->keyval) {
            if (root->node.nextkey_hi == 0) {
                int nextoff = key_new_node(base, &root, &hdr->keytreesize);
                root->node.nextkey_hi = nextoff;
            }
            root = keyptr(*base, root->node.nextkey_hi);
            continue;
        }

        if (*keyseq < root->keyval) {
            if (root->node.nextkey_lo == 0) {
                int nextoff = key_new_node(base, &root, &hdr->keytreesize);
                root->node.nextkey_lo = nextoff;
            }
            root = keyptr(*base, root->node.nextkey_lo);
            continue;
        }

        // How did I get here?
        abort();
    }

    errx(EXIT_FAILURE, "failed to construct keytree");
}

int main(int argc, char **argv)
{
    struct KEYINFO *keys = NULL;
    uint32_t numkeys;
    struct KEYMAP hdr = {
        .magic = 0x101,
        .version = 1,
        .zero = 0,
        .keytreesize = 0,
        .keydatasize = 0,
        .root = sizeof hdr,
    };

    // Let ncurses know what our terminal is.
    if (setupterm(argv[1], STDIN_FILENO, NULL) != OK) {
        err(EXIT_FAILURE, "setupterm failed, is $TERM correct?");
    }

    // We can safely use more capabilities.
    use_extended_names(true);

    if (isatty(STDOUT_FILENO)) {
        errx(EXIT_FAILURE, "Please redirect stdout to a file, e.g. %s > %s",
                           argv[0],
                           getenv("TERM"));
    }

    // Count how many key definitions there are.
    numkeys = sizeof(keydefs) / sizeof(struct KEYDEF);

    // First we need to lookup each of the keys.
    for (int i = 0; i < numkeys; i++) {
        char *sequence = tigetstr(keydefs[i].cap);

        // Keep track of how much data we need to append.
        hdr.keydatasize += strlen(keydefs[i].name);
        hdr.keydatasize += strlen(keydefs[i].cap);
        hdr.keydatasize += 2;

        // Check that ncurses recognizes this capability.
        if (sequence == NULL || sequence == (char *) ERR || *sequence == 0) {
            // No sequence found, lets check if any of the alternatives works.
            for (int alt = 0; alt < MAX_ALTCAPS; alt++) {
                // This might be the end of our caps.
                if (keydefs[i].altcaps[alt] == NULL)
                    break;

                // See if this one works.
                sequence = tigetstr(keydefs[i].altcaps[alt]);

                // Check result.
                if (sequence && sequence != (char *) ERR && *sequence != 0) {
                    warnx("Terminal %s uses alternative cap %s for %s",
                          argv[1] ? argv[1] : getenv("TERM"),
                          keydefs[i].altcaps[alt],
                          kfun_name(keydefs[i].kfun));
                    goto foundalt;
                }
            }

#ifdef DEBUG
            warnx("you do not appear to have a %s key", keydefs[i].name);
#endif
            continue;
        }

        foundalt:

        // Seems okay, add it to the array.
        keydefs[i].kseq = sequence;
    }

    // Now we sort the array so that the generated tree is compact.
    qsort(keydefs,
          numkeys,
          sizeof(struct KEYDEF),
          compare_keyseq);

    // Now we build our tree.
    // First, add the sequences that are the same for everyone.
    append_key_sequence(&keys, &hdr, "\e", KFUN_ESC);
    append_key_sequence(&keys, &hdr, "\f", KFUN_REFRESH);
    append_key_sequence(&keys, &hdr, "\b", KFUN_BACKSPACE);
    append_key_sequence(&keys, &hdr, "\r", KFUN_RETURN);
    append_key_sequence(&keys, &hdr, "\n", KFUN_RETURN);

    // Alternatives for vt100
    append_key_sequence(&keys, &hdr, "\036", KFUN_PREV_SHEET);
    append_key_sequence(&keys, &hdr, "\035", KFUN_NEXT_SHEET);

    // Now append all the shortcuts.
    for (int i = 0; i < numkeys; i++) {
        append_key_sequence(&keys, &hdr, keydefs[i].kseq, keydefs[i].kfun);
    }

    // Now the macro shortcut keys, Alt-A - Alt-Z
    for (int c = 0; c < 26; c++) {
        char seqlower[3] = { '\033', 'a' + c, '\0' };
        char sequpper[3] = { '\033', 'A' + c, '\0' };
        append_key_sequence(&keys, &hdr, seqlower, KFUN_MAC_A + c);
        append_key_sequence(&keys, &hdr, sequpper, KFUN_MAC_A + c);
    }

    // Write the header and key tree out.
    fwrite(&hdr, sizeof hdr, 1, stdout);
    fwrite(keys, hdr.keytreesize, 1, stdout);

    // As far as I know, lotus never looks at the data but I'll append it anyway.
    fwrite(&numkeys, sizeof numkeys, 1, stdout);

    for (int i = 0; i < hdr.keytreesize / sizeof(KEYINFO); i++) {
        static uint16_t dummy;

        if (i < numkeys) {
            fwrite(&keydefs[i].kfun, sizeof keydefs[i].kfun, 1, stdout);
            continue;
        }

        // I think it expects data, even if we dont use it.
        fwrite(&dummy, sizeof dummy, 1, stdout);
    }

    // Write out names.
    for (int i = 0; i < numkeys; i++) {
        fwrite(keydefs[i].name, strlen(keydefs[i].name) + 1, 1, stdout);
    }

    // Write out sequences.
    for (int i = 0; i < numkeys; i++) {
        // Just write an empty string if there was no key sequence.
        if (keydefs[i].kseq == NULL) {
            fputc(0, stdout);
            continue;
        }
        fwrite(keydefs[i].kseq, strlen(keydefs[i].kseq) + 1, 1, stdout);
    }

    // Done.
    return 0;
}

