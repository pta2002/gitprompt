# gitprompt
Add a little git to your prompt!

`gitprompt` is a 100% customizable git prompt that is 100% written in C.
Everything can be customized through themes, which are simple header files that
define a `print_status` function.

## It's fast
`gitprompt` is 100% written in C, and spawns a single git command. This is
actually faster than using something like `libgit2` (I've tested!) and more
accurate, so there's really no reason not to do it.

Since the themes are just header files, everything is completely customizable.
You can turn features you need on or off, so there won't be any unused code
running (or even compiled!)

## TODO: benchmarks

## Installing
```bash
make
```

Then, just add the `gitprompt` executable to your shell
