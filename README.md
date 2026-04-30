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
- [ ] Using ~ as an aLias for /home/$USER (modify Autocompletion code AND wildcard Code AND Preprocesssor)
- [ ] Math preprocessor with (echo \`3+4\`)
- [x] Variable Substitution (echo $PATH)
- [ ] Wildcards (* and ?)
- [ ] outputting to files with >>
- [ ] navigating the character pointer and inserting characters mid string
- [ ] Piping
