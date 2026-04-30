# mshell

A minimalist Shell written in C

## DISCLAIMER

This is not (yet) a safe Shell. There are a lot of issues regarding bounds checks etc. That may lead to Crashes or Worse.

## Features

- [x] Executing Line by line
- [x] parsing "text in hyphons" as one Argument
- [x] Autocomplete executables on PATH for the first Argument
- [x] Autocomplete Files/Directories for non-first Argument
- [x] History
- [x] Using ~ as an alias for /home/$USER (modify Preprocesssor)
- [x] ~ alias works with autocomplete
- [x] Variable Substitution (echo $PATH)
- [x] Wildcards (* and ?)
- [ ] Math preprocessor with (echo \`3+4\`)
- [ ] outputting/appending to files with > / >> (ONLY IF >> and FILE are the last 2 args);
- [ ] remove last word with CTRL + BACKSPACE
- [ ] navigating the character pointer and inserting characters mid string
- [ ] Piping
