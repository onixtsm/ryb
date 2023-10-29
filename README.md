# Rocky You Baby

This is TU/e EE group 10 Rock You Baby projects codebase.

## Setup

Installation

```console
$ git clone https://github.com/onixtsm/ryb
$ cp ryb {pynq_board_any_place}
$ cd {pynq_board_any_place}/ryb
$ make # can use make -j 4 to compile faster
```

## Coding rules

**All code is placed in `src/` directory**
To recompile code run `make` from root of the project
You can indent code by runing
```console
$ make indent
```

Compiled code is in `./build/main`

Do not forget to make and include header file if you created a `.c` file
For now it compiles everything in `main` file, but it will be changed when it is needed.

To run the code you can use
```console
$ make run
# or
$ ./build/main
```

## Learning material
* [freecodecamp](https://www.freecodecamp.org/news/learn-the-basics-of-git-in-under-10-minutes-da548267cc91/)
* [git-scm](https://git-scm.com/book/en/v2/Git-Basics-Getting-a-Git-Repository)
* [freecodecamp youtube](https://www.youtube.com/watch?v=RGOj5yH7evk)


#### P.S.
**DOCUMENT YOUR CODE**
