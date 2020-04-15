# gitprompt
Add a little git to your prompt!

`gitprompt` is a 100% customizable git prompt that is 100% written in C.
Everything can be customized through themes, which are simple header files that
define a `print_status` function.

## It's fast
`gitprompt` is 100% written in C, and relies on libgit2 to get the repository
information. This means that it doesn't spawn any external processes or run any
commands, which makes it much faster than `git`, `grep` and `awk` combinations
that you might have used were you to make this completely within your shell.

Since the themes are just header files, everything is completely customizable.
You can turn features you need on or off, so there won't be any unused code
running (or even compiled!)

## TODO: benchmarks

## Installing
```bash
make
```

Then, just add the `gitprompt` executable to your shell
