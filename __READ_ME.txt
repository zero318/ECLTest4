
This file is for documenting introductory information about the codebase.

Hopefully that will help people figure out WTF they're looking at when they
try to develop/use this mess of a project.

Project Goals:
-Make a working ECL compiler that has more features than THTK.
-Make a working C tokenizer for general purpose use.

Style/Design Notes:
-Don't be an idiot.
-Maximize legibility without sacrificing performance.
-Avoid pointer arithmetic if at all possible.
-Try to conform to the type system rather than spamming void* casts.

Important Things:
-The main function is in "ECLTest3.c"
-Currently "Tokenizer.c" is where most of the code is buried.