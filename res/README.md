# Lotus Resources

This is a work in progress attempt to allow 123 to be localized. 1-2-3 stores
all of the strings, help and so on in resource files. These are binary files
and we don't have the originals.

However, we can figure out the format and compile our own. This is a first
attempt at getting that working.

## Usage

The `resgen` command can take a simple text format, and generate binary
resource files.

```
$ ./resgen < l123txt3.txt > l123txt3.ri
```

Assuming that worked, you can put it in `/usr/share/lotus/123.v10/ri/USA-English/l123txt3.ri`.

Try starting 1-2-3, and the string you changed in `l123txt3.txt` should be updated.

## Notes

This doesn't work on the help resources yet, which uses huffman encoding.

Eventually, it would be nice to import the updated help files from 123r4.
