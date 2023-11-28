# Rocky You Baby

This is TU/e EE group 10 Rock You Baby projects codebase.

## Setup to just run and see how it works
You will not be able to push code here.
Installation

```console
$ git clone https://github.com/onixtsm/ryb
$ cp ryb {pynq_board_any_place}
$ cd {pynq_board_any_place}/ryb
$ make # can use make -j 4 to compile faster
```

## Setup so you can edit code here
Follow these steps to contribute this project.
1. Make [Github accout](https://github.com/signup)
2. Fork this project
3. Clone it with
```console
$ git clone https://github.com/[username]/ryb
```
4. Make new branch with command
```console
$ git checkout -b [branch name]
```
5. Edit code
6. Commit all changes (**message should be meaningful and well describing your changes**)
```console
$ git add [files or dirs you changes]
$ git commit
```
7. Push changes to your account
```console
$ git push
```
8. Make pull request on github

## Getting newest chages

```console
$ git pull -p
```

## Coding rules

**All code is placed in `src/` directory**
To recompile code run `make` from root of the project
If you want to compile code on your computer (for instance you do not need any pynqboard IO), you can use `make nopynq=1`. If you do that you need to recompile whole library with this option.
You can indent code by runing
```console
$ make indent
```

Compiled code is in `./build/main`

Do not forget to make and include header file if you created a `.c` file
If you create header file add header guard to it.
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
* [Making pull request](https://www.youtube.com/watch?v=8lGpZkjnkt4)


#### P.S.
**DOCUMENT YOUR CODE**
