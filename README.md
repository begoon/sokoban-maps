Sokoban maps
============

This project contains 60 maps (levels or mazes) for the Sokoban game. The
maps have been extracted from the binary file of a Sokoban implementation
for DOS called `pusher`. The `pushermaps.c` is the automated map extractor.

There is a couple of screenshots of `pusher.exe` running in DOSBox.

Build
-----

Type `make` to build the map extractor and run it. The extractor will
parse the `pusher.exe` file, extract the maps and print them to the
standard output. For example:

    *************************************
    Maze: 1
    File offset: 148C, DS:00FC, table offset: 0000
    Size X: 22
    Size Y: 11
    End: 14BD
    Length: 50

        XXXXX
        X   X
        X*  X
      XXX  *XXX
      X  *  * X
    XXX X XXX X     XXXXXX
    X   X XXX XXXXXXX  ..X
    X *  *             ..X
    XXXXX XXXX X@XXXX  ..X
        X      XXX  XXXXXX
        XXXXXXXX

    *************************************
    Maze: 2
    File offset: 14BE, DS:012E, table offset: 0032
    Size X: 14
    Size Y: 10
    End: 14E8
    Length: 43

    XXXXXXXXXXXX
    X..  X     XXX
    X..  X *  *  X
    X..  X*XXXX  X
    X..    @ XX  X
    X..  X X  * XX
    XXXXXX XX* * X
      X *  * * * X
      X    X     X
      XXXXXXXXXXXX

    *************************************

and so on.

Maps
-----

In the `./maps` subdirectory there are two files:

* `sokoban-maps-60-plain.txt` with the maps in the plain text form.
* `sokoban-maps-60-compressed.txt` with the maps in the compressed from as they are in the `pusher.exe` file.

The maps are compressed by a Huffman-like method explained at the beginning
of the `pushermaps.c` file.

Reverse engineering
-------------------

In the `./disasm` subdirectory there is a file called `pusher.lst` containing
a disassembled listing of the `pusher.exe` executable.

Playing pusher
--------------

You can run `pusher.exe` via DOSBox:

    DOSBox -exit pusher.exe

Tested on Windows 7 and Mac OSX 10.7.4 with DOSBox 0.74.
