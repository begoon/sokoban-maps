/*
  Pusher Map Decoder
  Version 0.3
  Copyright (c) 1998, 2012 by Alexander Demin

  The map compression format
  --------------------------

  Each map consists of three parts:
    * prolog (2 bytes):
        - char size_x
        - char size_y
    * elements (size_x*size_y bytes)
    * epilog (2 bytes):
        - char man_x
        - char man_y

  The middle part called "elements" is a bit stream. Each character
  on the map is represented as a pair of a counter and a character
  code. The counter and the character code are encoded by bit
  sequences of variable lengths.

  The counter determines how many times the character is repeated.
  The counter can have 2 values (bit sequences):
    - 0          (1 bit)  - one character
    - 1 D3 D2 D1 (4 bits) - N characters (N = 2 + D3*4 + D2*2 + D1)
                            N can encode values from 2 to 9.

  The character code can have 5 values (bit sequences):
    - 0 0    - an empty space (2 bits)
    - 0 1    - the wall (2 bits)
    - 1 0    - the barrel (2 bits)
    - 1 1 0  - the place for a barrel (3 bits)
    - 1 1 1  - the barrel already in place (3 bits)

  -----------------------------------------------------------------

  In short:

  -- Prolog
         char size_x
         char size_y
  -- Elements
         counter (bits)
                 0                - 1 symbol
                 1 D3 D2 D1       - 2+D3*4+D2*2+D1 symbols (9 max)
         char (bits)
                 0 0              - an empty space
                 0 1              - the wall
                 1 0              - the barrel
                 1 1 0            - the place for a barrel
                 1 1 1            - the barrel already in place
  -- Epilog
         char man_x
         char man_y
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NR_MAZES       60      // The number of mazes.
#define DS_OFFSET      0x1390  // The offset of the DS segment area in the file.
#define OFFSET_TABLE   0x1480  // The offset of the maze offset table within the
                               // DS segment.
#define PACKED_DATA    0x00FC  // The offset of the maze compressed data within
                               // the DS segment.
#define EXE_SIZE 10632

// We load the entire "pusher.exe" file into this buffer.
static char exe[EXE_SIZE];

// This variable points to the current position in the "exe"
// buffer during parsing.
static int offset;

// These two variables are used in "getbit()" function to extract bits.
static int bit_counter = -1;
static int current_byte = 0;

int getbit(void) {
    if (bit_counter < 0) {
        bit_counter = 7;
        current_byte = exe[offset];
        offset += 1;
    }
    return (current_byte & (1 << bit_counter--)) != 0;
}

// These two variables are used in "getbyte()" function to decode
// bit sequences into characters.
static int current_char = -1;
static int repeat_counter = 0;

char getbyte(void) {
    if (repeat_counter > 0 ) {
        --repeat_counter;
        return current_char;
    }

    if (getbit())
        repeat_counter = 2 + (getbit() << 2) + (getbit() << 1) + getbit() - 1;

    current_char = '?';
    if (getbit()) {
        if (getbit()) {
            if (getbit()) {
                current_char = '&';    // 111, the barrel already in place
            } else {
                current_char = '.';    // 110, the place for a barrel
            }
        } else {
            current_char = '*';        // 10, the barrel
        }
    } else {
        if (getbit()) {
            current_char = 'X';        // 01, the wall
        } else {
            current_char = ' ';        // 00, an empty space
        }
    }

    return current_char;
}

// Extracts two bytes into a little endian word.
int extract_16(const char* addr) {
    return (unsigned char)addr[0] | ((unsigned char)addr[1] << 8);
}

int main() {
    FILE* f;
    int maze;
    int read;

    f = fopen("pusher.exe", "rb");
    assert(f != NULL);

    read = 0;
    while (!feof(f)) {
        int const sz = fread(exe + read, 1, sizeof(exe) - read, f);
        assert(sz >= 0);
        if (sz == 0) break;
        read += sz;
    }

    fclose(f);

    assert(read == sizeof(exe)); 

    for (maze = 0; maze < NR_MAZES; ++maze) {
        // The maximum maze size is 40x25.
        char map[25 * 41];
        int begin, size_x, size_y, x, y;
        int man_x, man_y, man_offset;

        memset(map, 0, sizeof(map));

        printf("*************************************\n");
        printf("Maze: %d\n", maze + 1);

        offset = extract_16(&exe[0] + DS_OFFSET + OFFSET_TABLE + maze * 2) +
                 DS_OFFSET + PACKED_DATA;

        begin = offset;
        printf("File offset: %04X, DS:%04X, table offset: %04X\n",
                offset, offset - DS_OFFSET, offset - DS_OFFSET - PACKED_DATA);

        size_x = exe[offset++];
        assert(size_x < 40);
        size_y = exe[offset++];
        assert(size_y < 25);
        printf("Size X: %d\nSize Y: %d\n", size_x, size_y);

        bit_counter = -1;
        repeat_counter = 0;

        for (y = 0; y < size_y; y++) {
            for (x = 0; x < size_x; x++) {
                const int ch = getbyte();
                map[y * (size_x + 1) + x] = ch;
            }
        }

        man_x = exe[offset++];
        assert(man_x < 40);
        man_y = exe[offset++];
        assert(man_y < 25);

        printf("End: %04X\nLength: %d\n", offset - 1, offset - begin);

        man_offset = man_y * (size_x + 1) + man_x;
        assert(map[man_offset] == ' ');
        map[man_y * (size_x + 1) + man_x] = '@';

        printf("\n");
        for (y = 0; y < size_y; y++) {
            printf("%s\n", map + y * (size_x + 1));
        }
        printf("\n");
    }

    printf("*************************************\n");
    return 0;
}
