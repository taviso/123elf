#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <err.h>

#pragma pack(1)
#include "coff.h"

// This is coffsyrup, a tiny tool for patching relocatable COFF objects.
//
// This tool was written to be used as part of the build process for Lotus
// 1-2-3 for Linux, it may or may not work on any other objects.
//
// Tavis Ormandy <taviso@gmail.com>, May 2022
//
// Usage: coffsyrup orig.o patched.o [SYMBOL...]
//
// This will undefine all the symbols specified on the commandline, i.e. make
// them undefined and external.
//
// Q: Why not just use objcopy -N, isn't that basically the same thing?
// A: The problem is objcopy will refuse to remove any symbol named in
//    a relocation. coffsyrup will just do what you asked, and assume you know
//    what you're doing... the UNIX way!
//

int main(int argc, char **argv)
{
    FILE *infile;
    FILE *outfile;
    FILHDR hdr;
    AOUTHDR opt;
    SCNHDR *scn;
    SYMENT *symtab;
    RELOC **relocs;
    LINENO **lines;
    uint32_t strtabsz;
    char *strtab;
    char **sdata;

    if (argc < 3) {
        errx(EXIT_FAILURE, "Not enough arguments specified.");
    }

    infile  = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");

    if (infile == NULL || outfile ==  NULL) {
        err(EXIT_FAILURE, "Unable to open the file specified.");
    }

    if (fread(&hdr, sizeof hdr, 1, infile) != 1) {
        err(EXIT_FAILURE, "Failed to read in COFF header");
    }

    // fprintf(stdout, ".f_magic       %8x /* magic number */\n", hdr.f_magic);
    // fprintf(stdout, ".f_nscns       %8x /* number of sections */\n", hdr.f_nscns);
    // fprintf(stdout, ".f_timdat      %8x /* time & date stamp */\n", hdr.f_timdat);
    // fprintf(stdout, ".f_symptr      %8x /* file pointer to symtab */\n", hdr.f_symptr);
    // fprintf(stdout, ".f_nsyms       %8x /* number of symtab entries */\n", hdr.f_nsyms);
    // fprintf(stdout, ".f_opthdr      %8x /* sizeof(optional hdr) */\n", hdr.f_opthdr);
    // fprintf(stdout, ".f_flags       %8x /* flags */\n", hdr.f_flags);

    if (hdr.f_magic != I386MAGIC) {
        errx(EXIT_FAILURE, "This f_magic value is not recognized");
    }

    if (hdr.f_opthdr != sizeof(opt)) {
        errx(EXIT_FAILURE, "Expected to read an ZMAGIC optional header.");
    }

    fread(&opt, sizeof opt, 1, infile);

    // fprintf(stdout, "opt.magic      %8x /* type of file */\n", opt.magic);

    if (opt.magic != ZMAGIC) {
        errx(EXIT_FAILURE, "The magic value in the optional header is not recognized.");
    }

    // fprintf(stdout, "opt.vstamp     %8x /* version stamp */\n", opt.vstamp);
    // fprintf(stdout, "opt.tsize      %8x /* text size in bytes, padded to FW bdry */\n", opt.tsize);
    // fprintf(stdout, "opt.dsize      %8x /* initialized data  */\n", opt.dsize);
    // fprintf(stdout, "opt.bsize      %8x /* uninitialized data  */\n", opt.bsize);
    // fprintf(stdout, "opt.entry      %8x /* entry pt  */\n", opt.entry);
    // fprintf(stdout, "opt.text_start %8x /* base of text used for this file  */\n", opt.text_start);
    // fprintf(stdout, "opt.data_start %8x /* base of data used for this file  */\n", opt.data_start);

    // Seek to the string table first.
    if (fseek(infile, hdr.f_symptr + hdr.f_nsyms * sizeof *symtab, SEEK_SET) != 0) {
        err(EXIT_FAILURE, "Failed to seek to the string table.");
    }

    // The first dword is the size.
    if (fread(&strtabsz, sizeof strtabsz, 1, infile) != 1) {
        err(EXIT_FAILURE, "Failed to read size of string table.");
    }

    // Make sure that number is sane.
    if (strtabsz < sizeof(strtabsz)) {
        errx(EXIT_FAILURE, "The string table size does not appear to be valid.");
    }

    // Allocate buffers for all the variable sized tables.
    scn = calloc(hdr.f_nscns, sizeof *scn);
    symtab = calloc(hdr.f_nsyms, sizeof *symtab);
    strtab = calloc(strtabsz, 1);
    sdata = calloc(hdr.f_nscns, sizeof *sdata);
    relocs = calloc(hdr.f_nscns, sizeof *relocs);
    lines = calloc(hdr.f_nscns, sizeof *lines);

    // We should already be at the string table, so read them in.
    if (fread(strtab + sizeof(strtabsz), strtabsz - sizeof(strtabsz), 1, infile) != 1) {
        err(EXIT_FAILURE, "Failed to read string table from file.");
    }

    // Seek to the symbol table.
    if (fseek(infile, hdr.f_symptr, SEEK_SET) != 0) {
        err(EXIT_FAILURE, "Failed to seek to symbol table.");
    }

    // Read in each symbol.
    for (int i = 0; i < hdr.f_nsyms; i++) {
        char *symname;
        int symnamedup = 0;

        if (fread(&symtab[i], sizeof *symtab, 1, infile) != 1) {
            err(EXIT_FAILURE, "Failed to read in a symbol.");
        }

        switch (symtab[i].e_scnum) {
            case N_UNDEF:
            case N_ABS:
            case N_DEBUG:
                break;
            default:
                if (symtab[i].e_scnum > hdr.f_nscns) {
                    errx(EXIT_FAILURE, "A symbol was found with an invalid section number %d.", symtab[i].e_scnum);
                }
        }

        if (symtab[i].e_numaux > 1 || i + symtab[i].e_numaux > hdr.f_nsyms) {
            errx(EXIT_FAILURE, "The number of auxiliary entries (%u) was illegal.", symtab[i].e_numaux);
        }

        if (!symtab[i].e.e.e_zeroes && symtab[i].e.e.e_offset > strtabsz) {
            errx(EXIT_FAILURE, "The symbol name does not appear to be valid.\n");
        }

        // Figure out the symbol name.
        if (symtab[i].e.e.e_zeroes) {
            symname = strndup(symtab[i].e.e_name, sizeof symtab[i].e.e_name);
            symnamedup = 1;
        } else {
            symname = strtab + symtab[i].e.e.e_offset;
        }

        // See if we are supposed to be adjusting this symbol.
        for (int check = 3; check < argc; check++) {
            if (strcmp(symname, argv[check]) == 0) {
                fprintf(stdout, "MATCH %s\n", symname);
                symtab[i].e_scnum = N_UNDEF;
                symtab[i].e_sclass = C_EXT;
                symtab[i].e_value = 0;
            }
        }

        // free symname if it was duplicated on heap
        if (symnamedup) free(symname);

        // XXX: Note the ++i, this must be last in the loop!!!
        if (symtab[i].e_numaux) {
            // Read in any auxiliary entry.
            if (fread(&symtab[++i], sizeof *symtab, 1, infile) != 1) {
                err(EXIT_FAILURE, "Failed to read auxiliary entry.");
            }
        }
    }

    // Now we can parse the section headers.
    for (int i = 0; i < hdr.f_nscns; i++) {
        // Seek to this scnhdr.
        if (fseek(infile, sizeof(hdr) + hdr.f_opthdr + i * sizeof(*scn), SEEK_SET) != 0) {
            err(EXIT_FAILURE, "Failed to seek to section header.");
        }

        if (fread(&scn[i], sizeof *scn, 1, infile) != 1) {
            err(EXIT_FAILURE, "Failed to read in a section header.");
        }

        // fprintf(stdout, ".s_name      \"%-8s\" /* section name */\n", scn[i].s_name);
        // fprintf(stdout, ".s_paddr       %8x /* physical address */\n", scn[i].s_paddr);
        // fprintf(stdout, ".s_vaddr       %8x /* virtual address */\n", scn[i].s_vaddr);
        // fprintf(stdout, ".s_size        %8x /* section size */\n", scn[i].s_size);
        // fprintf(stdout, ".s_scnptr      %8x /* file ptr to raw data for section */\n", scn[i].s_scnptr);
        // fprintf(stdout, ".s_relptr      %8x /* file ptr to relocation */\n", scn[i].s_relptr);
        // fprintf(stdout, ".s_lnnoptr     %8x /* file ptr to line numbers */\n", scn[i].s_lnnoptr);
        // fprintf(stdout, ".s_nreloc      %8x /* number of relocation entries */\n", scn[i].s_nreloc);
        // fprintf(stdout, ".s_nlnno       %8x /* number of line number entries */\n", scn[i].s_nlnno);
        // fprintf(stdout, ".s_flags       %8x /* flags */\n", scn[i].s_flags);

        // Check if this section has any raw data we need to read.
        if (scn[i].s_size) {
            // Allocate buffer for the raw data.
            sdata[i] = calloc(scn[i].s_size, 1);

            // Try to read in that data.
            if (fseek(infile, scn[i].s_scnptr, SEEK_SET) != 0) {
                err(EXIT_FAILURE, "The section pointer was invalid or file truncated.");
            }

            if (fread(sdata[i], scn[i].s_size, 1, infile) != 1) {
                err(EXIT_FAILURE, "The section data could not be read.");
            }
        }

        // Seek to the relocations.
        if (fseek(infile, scn[i].s_relptr, SEEK_SET) != 0) {
            err(EXIT_FAILURE, "Could not seek to relocations for section %u.", i);
        }

        // Allocate space to store them.
        relocs[i] = calloc(scn[i].s_nreloc, sizeof(RELOC));

        for (int r = 0; r < scn[i].s_nreloc; r++) {
            char *symname;
            int symnamedup = 0;
            RELOC *rel = &relocs[i][r];

            if (fread(rel, sizeof *rel, 1, infile) != 1) {
                err(EXIT_FAILURE, "Failed to read in relocation %u/%u", r, scn[i].s_nreloc);
            }

            //fprintf(stdout, "[%02u].r_vaddr     %8x /* address of relocation */\n", r, rel.r_vaddr);
            //fprintf(stdout, "[%02u].r_symndx    %8x /* symbol we're adjusting for */\n", r, rel.r_symndx);
            //fprintf(stdout, "[%02u].r_type      %8x /* type of relocation */\n", r, rel.r_type);

            if (rel->r_type != RELOC_ADDR32 && rel->r_type != RELOC_REL32) {
                errx(EXIT_FAILURE, "Unsupported relocation type %u", rel->r_type);
            }

            if (rel->r_symndx > hdr.f_nsyms) {
                errx(EXIT_FAILURE, "A relocation references a non-existent symbol %u.", rel->r_symndx);
            }

            if (rel->r_vaddr < scn[i].s_vaddr || rel->r_vaddr > scn[i].s_vaddr + scn[i].s_size) {
                errx(EXIT_FAILURE, "A relocation appears to be out of range.");
            }

            // Figure out the symbol name.
            if (symtab[rel->r_symndx].e.e.e_zeroes) {
                symname = strndup(symtab[rel->r_symndx].e.e_name, sizeof symtab[rel->r_symndx].e.e_name);
                symnamedup = 1;
            } else {
                symname = (strtab + symtab[rel->r_symndx].e.e.e_offset);
            }

            // See if we are supposed to be adjusting this symbol.
            for (int check = 3; check < argc; check++) {
                if (strcmp(symname, argv[check]) == 0) {
                    uint32_t patchaddr = (rel->r_vaddr - scn[i].s_vaddr);
                    uint32_t *baseaddr = (uint32_t*) &sdata[i][rel->r_vaddr - scn[i].s_vaddr];

                    fprintf(stdout, "RELOC %s %s @%#x ~%#x\n",
                                        rel->r_type == RELOC_ADDR32 ? "abs" : "rel",
                                        symname,
                                        patchaddr,
                                        *baseaddr);

                    // I don't know how to handle this properly, if this doesn't work
                    // it's because I don't know what I'm doing - I'm just hacking
                    // around until it works :(
                    if (rel->r_type == RELOC_REL32) {
                        *baseaddr = - (rel->r_vaddr + sizeof(uint32_t));
                    } else if (rel->r_type == RELOC_ADDR32) {
                        // I think I just want this to be zero? I legit don't
                        // know what I'm doing, if this isn't working don't assume
                        // I must have known something you dont lol.
                        *baseaddr = 0;
                    } else {
                        errx(EXIT_FAILURE, "Cannot patch unhandled relocation type.");
                    }
                }
            }

            // free symname if it was duplicated on heap
            if (symnamedup) free(symname);
        }

        // Seek to the line numbers
        if (fseek(infile, scn[i].s_lnnoptr, SEEK_SET) != 0) {
            err(EXIT_FAILURE, "Failed to seek to line numbers.");
        }

        // Allocate space to store them.
        lines[i] = calloc(scn[i].s_nlnno, sizeof(LINENO));

        for (int l = 0; l < scn[i].s_nlnno; l++) {
            LINENO *lin = &lines[i][l];

            if (fread(lin, sizeof *lin, 1, infile) != 1) {
                err(EXIT_FAILURE, "Failed to read in line number %u/%u", l, scn[i].s_nlnno);
            }

            if (lin->l_lnno == 0) {
                if (lin->l_addr.l_symndx > hdr.f_nsyms) {
                    errx(EXIT_FAILURE, "A line number is for an invalid function.");
                }
            }
        }
    }

    // Okay, now try to write out the new object.
    if (fwrite(&hdr, sizeof hdr, 1, outfile) != 1) {
        err(EXIT_FAILURE, "Could not write output header.");
    }
    if (fwrite(&opt, sizeof opt, 1, outfile) != 1) {
        err(EXIT_FAILURE, "Could not write optional header.");
    }
    if (fwrite(scn, sizeof *scn, hdr.f_nscns, outfile) != hdr.f_nscns) {
        err(EXIT_FAILURE, "Failed to write section headers.");
    }

    // Now copy over the section data.
    for (int i = 0; i < hdr.f_nscns; i++) {
        // Not all sections have raw data.
        if (scn[i].s_size) {
            // Seek to the start.
            if (fseek(outfile, scn[i].s_scnptr, SEEK_SET) != 0) {
                err(EXIT_FAILURE, "Could not seek to section %8s in output", scn[i].s_name);
            }

            // Write out everything we have.
            if (fwrite(sdata[i], scn[i].s_size, 1, outfile) != 1) {
                err(EXIT_FAILURE, "Could not write section %8s to output", scn[i].s_name);
            }
        }

        // Do we have relocations?
        if (scn[i].s_nreloc) {
            if (fseek(outfile, scn[i].s_relptr, SEEK_SET) != 0) {
                err(EXIT_FAILURE, "Could not seek to the relptr for %8s in output", scn[i].s_name);
            }
            if (fwrite(relocs[i], sizeof(RELOC), scn[i].s_nreloc, outfile) != scn[i].s_nreloc) {
                err(EXIT_FAILURE, "Failed to write out relocations.");
            }
        }

        // What about line numbers?
        if (scn[i].s_nlnno) {
            if (fseek(outfile, scn[i].s_lnnoptr, SEEK_SET) != 0) {
                err(EXIT_FAILURE, "Could not seek to the lnnoptr for %8s in output", scn[i].s_name);
            }
            if (fwrite(lines[i], sizeof(LINENO), scn[i].s_nlnno, outfile) != scn[i].s_nlnno) {
                err(EXIT_FAILURE, "Failed to write out line numbers.");
            }
        }
    }

    // Symbols
    if (hdr.f_nsyms) {
        if (fseek(outfile, hdr.f_symptr, SEEK_SET) != 0) {
            err(EXIT_FAILURE, "Failed to seek to symbol table in output.");
        }
        if (fwrite(symtab, sizeof *symtab, hdr.f_nsyms, outfile) != hdr.f_nsyms) {
            err(EXIT_FAILURE, "Failed to write out symbol table.");
        }
    }

    // Strings
    // First, we write out the string table size.
    if (fwrite(&strtabsz, sizeof strtabsz, 1, outfile) != 1) {
        err(EXIT_FAILURE, "The string table size could not be written.");
    }
    if (fwrite(strtab + sizeof(strtabsz), strtabsz - sizeof(strtabsz), 1, outfile) != 1) {
        err(EXIT_FAILURE, "Failed to write out the string table.");
    }

    fclose(infile);
    fclose(outfile);
    free(scn);
    free(symtab);
    free(strtab);

    for (int i = 0; i < hdr.f_nscns; i++) {
        free(sdata[i]);
        free(relocs[i]);
        free(lines[i]);
    }

    free(sdata);
    free(relocs);
    free(lines);
    return 0;
}
