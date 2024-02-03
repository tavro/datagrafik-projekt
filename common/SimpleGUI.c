// SimpleGUI prototype 2022.
// By Ingemar R.

// First preliminary version 2022-02. Let's call it 0.1.
// 0.2 with palette and button.
// 0.2.1 cleans up. Better border check. Individually configurable slider fill color.
// 0.3 Adds support for popup menus.
// 0.4 Adds support for "steppers". Added glBindVertexArray(0); to avoid host program to change by VAO.
// Support for multi-line strings (StaticString and DynamicString only, not labels).
// Supports auto-placement by entering negative values.
// 0.4.1: Added the option to scale. This is integer only since other scales look bad, really bad.
// Simplified the buttons, to skip the double border that collided with the text.
// 0.4.2: Fixed a bug concerning dragging outside controls. Reversed text color in buttons.
// 0.4.3: Hacked (mostly) around the M1 problem. The GUI still starts in a strange position but clicks and drags are correct.
// It is a bit sad that the GUI looks pretty nice when scaled down to fit the Retina displays and I don't support that for the
// moment. Pretty easy to hack around though.
// 0.5: Made it more C++ friendly and added a C++ version of the demo.

// I don't claim that this is a perfect GUI but it is a tight, single-file
// solution with minimal dependencies. It should plug nicely into any OpenGL program.
// Based on SimpleFont, using shapes as "characters" in the font system.
// This is not very flexible but it was easy to do and the result is not too bad.

// Lots of hard-coded constants, I'm afraid. A lot of things can be made more flexible.

// Additions after the very first version:
// At 1500 lines it is getting less and less minimal. Should I stop? Trim it down a bit?
// Oops, 1700 with the popup menu. Hm. And now 1800 with 0.4 additions.

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "SimpleGUI.h"

// Ugly hard-coded hack to fix a bug/(incompatibility with Apple's M/ARM CPUs.
// I don't feel 2x is a safe bet but for now it seems to work.
#if (defined(__arm64__) && defined(__APPLE__)) || defined(__aarch64__)
	#define mscale 2
#else
	#define mscale 1
#endif

// Inline characters and symbols. The characters parts are from SimpleFont.

// Original CAPITAL letters
static GLubyte letters[][16] = {
{0, 0, 0, 0, 0, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18}, 
{0, 0, 0, 0, 0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe}, 
{0, 0, 0, 0, 0, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e}, 
{0, 0, 0, 0, 0, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc}, 
{0, 0, 0, 0, 0, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff}, 
{0, 0, 0, 0, 0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff}, 
{0, 0, 0, 0, 0, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e}, 
{0, 0, 0, 0, 0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3}, 
{0, 0, 0, 0, 0, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e}, 
{0, 0, 0, 0, 0, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}, 
{0, 0, 0, 0, 0, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3}, 
{0, 0, 0, 0, 0, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0}, 
{0, 0, 0, 0, 0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3}, 
{0, 0, 0, 0, 0, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3}, 
{0, 0, 0, 0, 0, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e}, 
{0, 0, 0, 0, 0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe}, 
{0, 0, 0, 0, 0, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c}, 
{0, 0, 0, 0, 0, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe}, 
{0, 0, 0, 0, 0, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e}, 
{0, 0, 0, 0, 0, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff}, 
{0, 0, 0, 0, 0, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3}, 
{0, 0, 0, 0, 0, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3}, 
{0, 0, 0, 0, 0, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3}, 
{0, 0, 0, 0, 0, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3}, 
{0, 0, 0, 0, 0, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3}, 
{0, 0, 0, 0, 0, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff}
};

// Lower case
static GLubyte lowletters[][16] = {
{0, 0, 0, 0x00, 0x00, 0x7d, 0xc3, 0xc3, 0xc3, 0x7f, 0x03, 0x7e, 0x00, 0x00, 0x00, 0x00}, // a
{0, 0, 0, 0x00, 0x00, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0}, // b
{0, 0, 0, 0x00, 0x00, 0x7f, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00}, // c
{0, 0, 0, 0x00, 0x00, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x03, 0x03, 0x03, 0x03}, // d
{0, 0, 0, 0x00, 0x00, 0x7e, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00}, // e
{0, 0, 0, 0x00, 0x00, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x18, 0x0e}, // f
{0, 0, 0, 0x7f, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00}, // g
{0, 0, 0, 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0}, // h
{0, 0, 0, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00}, // i
{0, 0, 0, 0x70, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00}, // j
{0, 0, 0, 0x00, 0x00, 0xc3, 0xc7, 0xce, 0xfc, 0xfe, 0xc7, 0xc3, 0xc0, 0xc0, 0xc0, 0xc0}, // k
{0, 0, 0, 0x00, 0x00, 0x0c, 0x1c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // l
{0, 0, 0, 0x00, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xfe, 0x00, 0x00, 0x00, 0x00}, // m
{0, 0, 0, 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0x00, 0x00, 0x00, 0x00}, // n
{0, 0, 0, 0x00, 0x00, 0x7e, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00}, // o
{0, 0, 0, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0x00, 0x00, 0x00, 0x00}, // p
{0, 0, 0, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x00, 0x00, 0x00, 0x00}, // q
{0, 0, 0, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xf0, 0xdf, 0x00, 0x00, 0x00, 0x00}, // r
{0, 0, 0, 0x00, 0x00, 0xfe, 0x03, 0x03, 0x7e, 0xc0, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00}, // s
{0, 0, 0, 0x00, 0x00, 0x0e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x18, 0x18}, // t
{0, 0, 0, 0x00, 0x00, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00}, // u
{0, 0, 0, 0x00, 0x00, 0x18, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00}, // v
{0, 0, 0, 0x00, 0x00, 0x66, 0x7e, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0x00, 0x00, 0x00, 0x00}, // w
{0, 0, 0, 0x00, 0x00, 0xc3, 0xe7, 0x3c, 0x18, 0x3c, 0xe7, 0xc3, 0x00, 0x00, 0x00, 0x00}, // x
{0, 0, 0, 0x7f, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00}, // y
{0, 0, 0, 0x00, 0x00, 0xff, 0xc0, 0x70, 0x1c, 0x06, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00}, // z
};

// Numbers and more (same section of ASCII)
static GLubyte numletters[][16] = {
{0, 0, 0, 0, 0, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c}, // 0
{0, 0, 0, 0, 0, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x38, 0x18}, // 1
{0, 0, 0, 0, 0, 0x7e, 0x60, 0x60, 0x60, 0x60, 0x3c, 0x06, 0x06, 0x66, 0x66, 0x3c}, // 2
{0, 0, 0, 0, 0, 0x3c, 0x66, 0x06, 0x06, 0x06, 0x1c, 0x06, 0x06, 0x06, 0x66, 0x3c}, // 3
{0, 0, 0, 0, 0, 0x06, 0x06, 0x06, 0x06, 0x06, 0x7f, 0x66, 0x36, 0x1e, 0x0e, 0x06}, // 4
{0, 0, 0, 0, 0, 0x3c, 0x66, 0x06, 0x06, 0x06, 0x7c, 0x60, 0x60, 0x60, 0x60, 0x7e}, // 5
{0, 0, 0, 0, 0, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x66, 0x3c}, // 6
{0, 0, 0, 0, 0, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x1f, 0x06, 0x06, 0x06, 0x06, 0x7e}, // 7
{0, 0, 0, 0, 0, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x3c}, // 8
{0, 0, 0, 0, 0, 0x3c, 0x66, 0x06, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x66, 0x3c}, // 9
{0, 0, 0, 0, 0, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00}, // :
{0, 0, 0, 0, 0, 0x30, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00}, // ;
{0, 0, 0, 0, 0, 0x06, 0x1c, 0x30, 0x60, 0x30, 0x1c, 0x06, 0x00, 0x00, 0x00, 0x00}, // <
{0, 0, 0, 0, 0, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00}, // =
{0, 0, 0, 0, 0, 0x60, 0x38, 0x0c, 0x06, 0x0c, 0x38, 0x60, 0x00, 0x00, 0x00, 0x00}, // >
{0, 0, 0, 0, 0, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x0c, 0x06, 0x06, 0x66, 0x3c}, // ?
};

static GLubyte specialletters[][16] = {
{0, 0, 0, 0, 0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // space
{0, 0, 0, 0, 0, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // !
{0, 0, 0, 0, 0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x24, 0x00, 0x00}, // "
{0, 0, 0, 0, 0, 0x24, 0x24, 0x7e, 0x7e, 0x24, 0x7e, 0x7e, 0x24, 0x24, 0x00, 0x00}, // #
{0, 0, 0, 0, 0, 0x18, 0x3c, 0x5a, 0x5a, 0x1a, 0x3c, 0x58, 0x58, 0x5a, 0x3c, 0x18}, // $
{0, 0, 0, 0, 0, 0x44, 0x4a, 0x6a, 0x24, 0x30, 0x18, 0x0c, 0x24, 0x56, 0x52, 0x22}, // %

{0, 0, 0, 0, 0, 0x79, 0xcf, 0xc6, 0xcf, 0x79, 0x70, 0x78, 0xcc, 0xcc, 0xcc, 0x78}, // &
{0, 0, 0, 0, 0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x18, 0x00, 0x00}, // '
{0, 0, 0, 0, 0, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x18, 0x0c}, // (
{0, 0, 0, 0, 0, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x18, 0x30}, // )
{0, 0, 0, 0, 0, 0x00, 0x00, 0x10, 0x54, 0x38, 0x54, 0x10, 0x00, 0x00, 0x00, 0x00}, // *
{0, 0, 0, 0, 0, 0x00, 0x00, 0x10, 0x10, 0x7c, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00}, // +
{0, 0, 0, 0, 0x30, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // ,
{0, 0, 0, 0, 0, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // -
{0, 0, 0, 0, 0, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // .
{0, 0, 0, 0, 0, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06}, // /
};

// NEW for SimpleGUI: Various shapes formulated as inline 16x8 bitmaps.

static GLubyte guiletters[][16] = {
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Small box (used for color clicker)

{ 0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF}, // Bigger box
{ 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF}, // Bigger box
{ 0xFF, 0xE0, 0xF0, 0xB8, 0x9C, 0x8E, 0x87, 0x83, 0x83, 0x87, 0x8E, 0x9C, 0xB8, 0xF0, 0xE0, 0xFF}, // Bb left check
{ 0xFF, 0x07, 0x0F, 0x1D, 0x39, 0x71, 0xE1, 0xC1, 0xC1, 0xE1, 0x71, 0x39, 0x1D, 0x0F, 0x07, 0xFF}, // Bn right check
{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Bigger inside left
{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // Bigger inside right

{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00}, // Slider bar middle
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x80, 0x80, 0x80, 0x7F, 0x00, 0x00, 0x00, 0x00}, // Slider bar left end
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x01, 0x01, 0x01, 0xFE, 0x00, 0x00, 0x00, 0x00}, // Slider bar right end
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00}, // Slider bar inside

{ 0x07, 0x18, 0x20, 0x40, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x40, 0x20, 0x18, 0x07}, // Circle left
{ 0xE0, 0x18, 0x04, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x04, 0x18, 0xE0}, // Circle right
{ 0x00, 0x07, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x3F, 0x3F, 0x1F, 0x07, 0x00}, // Circle left inside
{ 0x00, 0xE0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFC, 0xFC, 0xF8, 0xE0, 0x00}, // Circle right inside
{ 0x00, 0x00, 0x03, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x1F, 0x0F, 0x03, 0x00, 0x00}, // Circle left spot
{ 0x00, 0x00, 0xC0, 0xF0, 0xF8, 0xF8, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xC0, 0x00, 0x00}, // Circle right spot

{ 0x00, 0x00, 0x00, 0x00, 0x3C, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x42, 0x3C, 0x00}, // Thumb
{ 0x00, 0x00, 0x00, 0x00, 0x3C, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x3C, 0x00, 0x00}, // Thumb inside

{ 0x07, 0x18, 0x20, 0x40, 0x40, 0x80, 0x80, 0x80, 0x40, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01}, // drop left
{ 0xE0, 0x18, 0x04, 0x02, 0x02, 0x01, 0x01, 0x01, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}, // drop right
{ 0x00, 0x07, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0x7F, 0x3F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00}, // drop left inside
{ 0x00, 0xE0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFC, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00}, // drop right inside

{ 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, // top+bottom - combine with circle for a button
{ 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00}, // top+bottom inside

{ 0x80, 0xC0, 0xE0, 0xB0, 0x98, 0x8C, 0x86, 0x83, 0x83, 0x86, 0x8C, 0x98, 0xB0, 0xE0, 0xC0, 0x80}, // Step right
{ 0x01, 0x03, 0x07, 0x0D, 0x19, 0x31, 0x61, 0xC1, 0xC1, 0x61, 0x31, 0x19, 0x0D, 0x07, 0x03, 0x01}, // Step left
{ 0x00, 0x00, 0x00, 0x40, 0x60, 0x70, 0x78, 0x7C, 0x7C, 0x78, 0x70, 0x60, 0x40, 0x00, 0x00, 0x00}, // Step right inside
{ 0x00, 0x00, 0x00, 0x02, 0x06, 0x0E, 0x1E, 0x3E, 0x3E, 0x1E, 0x0E, 0x06, 0x02, 0x00, 0x00, 0x00}, // Step left inside
};

// Always update the loader when new items are added! (fontToTexture)

// Outcomented shapes are not used and should be removed

enum
{
	smallbox = 128,
	bigboxleft, bigboxright, bigboxleftchecked, bigboxrightchecked, bigboxleftinside, bigboxrightinside,
	sliderbarmiddle, sliderbarleft, sliderbarright, sliderbarinside,
	circleleft, circleright, circleleftinside, circlerightinside, circleleftspot, circlerightspot,
	thumb, thumbinside,
	dropleft, dropright, dropleftinside, droprightinside,
	topbottom, topbottominside,
	stepright, stepleft, steprightinside, stepleftinside,
};

// Inline shaders
static char frag[] =
"#version 150\n"
"out vec4 outColor;"
"in vec2 texCoord;"
"uniform sampler2D tex;"
"uniform float red,green,blue;"
"void main(void)"
"{"
"	vec4 pixel = texture(tex, texCoord);"
"	outColor = pixel * vec4(red, green, blue, 1.0);"
"}";

static char vert[]=
"#version 150\n"
"in  vec3 inPosition;"
"out vec2 texCoord;"
"uniform int x, y, c;"
"uniform int screenSizeX, screenSizeY;"
"uniform int offsx, offsy;"
"uniform float charWidth, charHeight;"
"uniform int texWidth, texHeight;"
"void main(void)"
"{"
"	int row = c / 16;" // the character (c) gives row and column in the texture
"	int col = c & 15;"
"	texCoord = vec2((col + inPosition.x)*charWidth/texWidth, (row + (1.0 - inPosition.y))*charHeight/texWidth);"
"	texCoord = vec2((col + inPosition.x)*charWidth/texWidth, (row + (inPosition.y))*charHeight/texWidth);"
""
"	float screenx = (x + offsx + inPosition.x*charWidth)*2.0/screenSizeX - 1.0;" // screen coords x and y
"	float screeny = -(y + offsy + inPosition.y*charHeight)*2.0/screenSizeY + 1.0;"
"	gl_Position = vec4(screenx, screeny, 0.0, 1.0);"
"}";

// For background box
static char frag2[] =
"#version 150\n"
"out vec4 outColor;"
"uniform float red,green,blue,alpha;"
"void main(void)"
"{"
"	outColor = vec4(red, green, blue, alpha);"
"}";

static char vert2[]=
"#version 150\n"
"in  vec3 inPosition;"
"uniform int x, y;"
"uniform int offsx, offsy;"
"uniform int screenSizeX, screenSizeY;"
"uniform float charWidth, charHeight;"
"uniform int texWidth, texHeight;"
"void main(void)"
"{"
"	float screenx = (x + offsx + inPosition.x*charWidth)*2.0/screenSizeX - 1.0;" // screen coords x and y
"	float screeny = -(y + offsy + inPosition.y*charHeight)*2.0/screenSizeY + 1.0;"
"	gl_Position = vec4(screenx, screeny, 0.0, 1.0);"
"}";

// Note: The use of charWidth and screenSize instead of a simple scaling is not really what I like
// but working code is working code so I let it be for now. But I know we can do this is a more
// straightforward way!

// Compile a shader, return reference to it
// No error checking - the shaders should be stable
static GLuint sfCompileShaders(const char *vs, const char *fs)
{
	GLuint v,f,p;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	glCompileShader(v);
	glCompileShader(f);
	p = glCreateProgram();
	glAttachShader(p,v);
	glAttachShader(p,f);
	glLinkProgram(p);
	glUseProgram(p);
	return p;
}

static GLfloat vertices[] = {	
							0.0f,0.0f,0.0f,
							0.0f,1.0f,0.0f,
							1.0f,0.0f,0.0f,
							
							0.0f,1.0f,0.0f,
							1.0f,1.0f,0.0f,
							1.0f,0.0f,0.0f};


// vertex array object
static unsigned int vertexArrayObjID;
static GLuint sfProgram = -1;
static GLuint sfBoxProgram;


// Initialize common resources, VAO and shader
static void initVAO()
{
	unsigned int vertexBufferObjID;
	
		// Load and compile shader
	sfBoxProgram = sfCompileShaders(vert2, frag2);
	sfProgram = sfCompileShaders(vert, frag);
	glUseProgram(sfProgram);

	glUniform1i(glGetUniformLocation(sfProgram, "tex"), 0);

	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);
	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(sfProgram, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(sfProgram, "inPosition"));

	glUniform1i(glGetUniformLocation(sfProgram, "tex"), 0); // Texture unit 0	

	glUseProgram(sfBoxProgram);
	glVertexAttribPointer(glGetAttribLocation(sfBoxProgram, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(sfBoxProgram, "inPosition"));
}

static void charToTexture(unsigned char *indata, int theChar, unsigned char *data)
{
	int row = (theChar-32) / 16;
	int col = theChar & 15;
	int startpos = (128 * row*16 + col*8)*4;
	int i, pos, rowchar, bit;
	for (i = 0; i < 16; i++)
	{
		pos = startpos + 128 * (15-i) * 4;
		rowchar = indata[i];
		for (bit = 128; bit != 0; bit = bit >> 1)
		{
			if ((rowchar & bit) == 0)
			{
				data[pos++] = 0;
				data[pos++] = 0;
				data[pos++] = 0;
				data[pos++] = 0;
			}
			else
			{
				data[pos++] = 255;
				data[pos++] = 255;
				data[pos++] = 255;
				data[pos++] = 255;
			}
		}
	} 
}

typedef struct SimpleFontData
{
	float characterHeight;
	float characterWidth;
	int texHeight;
	int texWidth;
	int extraSpace;
	GLuint textureID;
	float fontColorRed, fontColorGreen, fontColorBlue;

} SimpleFontData, *SimpleFontDataPtr;

static SimpleFontData font;
static int gInitialized = 0;

void sfLoadExternalFont(unsigned char *data, float charWidth, float charHeight, int imageWidth, int imageHeight, int extraSpace)
{
	GLint tex;
	GLuint fontTexture;
	GLint saveprogram;

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex); // Save current
	glGetIntegerv(GL_CURRENT_PROGRAM, &saveprogram);
	
	initVAO(); // Init if needed
	
	// Load to texture
	glGenTextures(1, &fontTexture);			// Generate OpenGL texture IDs
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// Not filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// Not filtered
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Save to font structure!
	font.characterHeight = charHeight;
	font.characterWidth = charWidth;
	font.texHeight = imageHeight;
	font.texWidth = imageWidth;
	font.extraSpace = extraSpace;
	font.textureID = fontTexture;
	font.fontColorRed = 1;
	font.fontColorGreen = 1;
	font.fontColorBlue = 1;

	glBindTexture(GL_TEXTURE_2D, tex); // Restore
	glUseProgram(saveprogram);
}

static void fontToTexture()
{
	GLint i, j, tex;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex); // Save current
	unsigned char *data = malloc(128*128*4); // 128x128 pixels, 4 bytes each
	
	// Upper case
	for (i = 0, j = 'A'; i < 26; i++, j++)
		charToTexture(letters[i], j, data);
	// Lower case
	for (i = 0,j = 'a'; i < 26; i++,j++)
		charToTexture(lowletters[i], j, data);
	// Numbers and some more
	for (i = 0,j = '0'; i < 16; i++,j++)
		charToTexture(numletters[i], j, data);
	// Space and some more
	for (i = 0,j = ' '; i < 16; i++,j++)
		charToTexture(specialletters[i], j, data);

	// GUI elements
	// Can NOT go under 32! 128 = smallbox
	for (i = 0,j = 128; i < stepleftinside-smallbox+1; i++,j++)
//	for (i = 0,j = 128; i < 155-128/*32*/; i++,j++)
		charToTexture(guiletters[i], j, data);

	sfLoadExternalFont(data, 8, 16, 128, 128, 2);
	glBindTexture(GL_TEXTURE_2D, tex); // Restore
}

enum
{
	kBox = 1, kString, kSlider, kColor, kCheckbox,
	kDisplayInt, kDisplayFloat, kRadioItem, kDynamicString,
	kColorClicker, kSmallColorClicker, kButton, kMenuItem, kMenu,
	kRightStepper, kLeftStepper
};

typedef struct Item
{
	int itemType;
	int state;
	int intData;
	float sliderMin, sliderMax, r, g, b;
	char *s;
	int x, y; // , w, h;
	int hx, hy, hw, hh; // hot box!
	void *var1,*var2,*var3; // Pointers to controlled variables
} Item;

static Item **items = NULL;
static int itemCount = 0;

static float frameRed = 1.0;
static float frameGreen = 1.0;
static float frameBlue = 1.0;
static float fillRed = 0.5;
static float fillGreen = 0.5;
static float fillBlue = 0.5;
static float sliderFillRed = 0.7;
static float sliderFillGreen = 0.7;
static float sliderFillBlue = 0.7;
static float bgRed = 1.0;
static float bgGreen = 1.0;
static float bgBlue = 1.0;
static float bgAlpha = 0.3;
static float textRed = 1.0;
static float textGreen = 1.0;
static float textBlue = 1.0;
static int bgBorder = 5;
static int gSpacing = 20;

static void drawChar(int x, int y, unsigned char c)
{
	// uniforms that vary for each character
	glUniform1i(glGetUniformLocation(sfProgram, "x"), x);
	glUniform1i(glGetUniformLocation(sfProgram, "y"), y);
	glUniform1i(glGetUniformLocation(sfProgram, "c"), c-32);
	glUniform1f(glGetUniformLocation(sfProgram, "charWidth"), font.characterWidth);
	glUniform1f(glGetUniformLocation(sfProgram, "charHeight"), font.characterHeight);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);	// draw object
}

// Internal upload of temporary color
static void SetColor(float r, float g, float b)
{
	glUniform1f(glGetUniformLocation(sfProgram, "red"), r);
	glUniform1f(glGetUniformLocation(sfProgram, "green"), g);
	glUniform1f(glGetUniformLocation(sfProgram, "blue"), b);
}

// Draws two icons with different colors in the same location
static void drawCharAndBack(int h, int v, unsigned char frame, unsigned char contents, /*colors*/ float fr, float fg, float fb, float cr, float cg, float cb)
{
	SetColor(cr, cg, cb); // contents
	drawChar(h, v-font.characterHeight, contents);
	if (frame != 0)
	{
		SetColor(fr, fg, fb); // frame
		drawChar(h, v-font.characterHeight, frame);
	}
}

// Box size, saved globally to make it possible to click
static int xmin = 10000;
static int ymin = 10000;
static int xmax = -10000;
static int ymax = -10000;

static int offsx = 0, offsy = 0;

static int activeButton = -1; // Remember button
#if (defined(__arm64__) && defined(__APPLE__)) || defined(__aarch64__)
	static int gScale = 2;
#else
	static int gScale = 1;
#endif

// drawString(h, v, items[i]->s);
static void drawString(int h, int v, char *s)
{
	glUniform1f(glGetUniformLocation(sfProgram, "red"), textRed);
	glUniform1f(glGetUniformLocation(sfProgram, "green"), textGreen);
	glUniform1f(glGetUniformLocation(sfProgram, "blue"), textBlue);
	for (;*s != 0;s++)
	{
		drawChar(h, v-font.characterHeight, *s); // -10 to get to the proper height
		h += font.characterWidth + font.extraSpace;
	}
}

static void drawText(int h, int v, char *s, int spacing)
{
	// Go through string, copy parts and draw
	int p = 0;
	int count = 0;
	for (int i = 0; s[i] != 0; i++)
	{
		if ((s[i+1] == 10) || (s[i+1] == 13) || s[i+1] == 0)
		{
			char temp[1024];
			if (i - p > 1023)
				i = p - 1023;
			int j;
			for (j = p; j <= i; j++)
				temp[j-p] = s[j];
			temp[j-p] = 0;
			drawString(h, v + spacing*count, temp);
			count++;
			
			if ((s[i+1] == 10) || (s[i+1] == 13))
				if (s[i] != s[i+1]) // CRLF
					i++;
			p = i+1;
		}
	}
}

// Get dimensions in number of characters and rows
static void GetTextDimensions(char *s, int *width, int *height)
{
	// Go through string, count and measure parts
	int p = 0;
	int count = 0;
	int maxLength = 0;
	for (int i = 0; s[i] != 0; i++)
	{
		if ((s[i+1] == 10) || (s[i+1] == 13) || s[i+1] == 0)
		{
			if (maxLength < i-p) maxLength = i-p;
			count++;
			
			if ((s[i+1] == 10) || (s[i+1] == 13))
				if (s[i] != s[i+1]) // CRLF
					i++;
			p = i+1;
		}
	}
	
	*width = maxLength;
	*height = count-1;
}

// Central function for redrawing
void sgDraw()
{
	GLint tex, activeTxtUnit;
	GLint saveprogram;
		
	// Get some current settings to restore them later
	char saveZ = glIsEnabled(GL_DEPTH_TEST);
	char saveCull = glIsEnabled(GL_CULL_FACE);
	char saveBlend = glIsEnabled(GL_BLEND);
	GLint savedSrcAlpha, savedDstAlpha;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &savedSrcAlpha);
	glGetIntegerv(GL_BLEND_DST_ALPHA, &savedDstAlpha);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTxtUnit);
	glActiveTexture(GL_TEXTURE0);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &tex);
	// I also would like to get and restore the current shader program:
	glGetIntegerv(GL_CURRENT_PROGRAM, &saveprogram);

	GLint a[4];
	glGetIntegerv(GL_VIEWPORT, a);
	int gRasterH = a[2]/gScale;
	int gRasterV = a[3]/gScale;

	if (!gInitialized)
	{
		initVAO();
		fontToTexture();
		gInitialized = 1;
	}

	glBindTexture(GL_TEXTURE_2D, font.textureID);

	glUseProgram(sfBoxProgram);

	if (bgAlpha > 0.01) // If background is not visible, skip all processing of it!
	{
		int w, h; // For GetTextDimensions
	// Background!
	// First, find size of all objects!
	// This can be optimized by recording max and min while drawing instead.
	// Many hard-coded numbers below have to be changed if we want customizable scaling.
		xmin = 10000;
		ymin = 10000;
		xmax = -10000;
		ymax = -10000;
		if (items != NULL)
		for (int i = 0; items[i] != NULL; i++)
		{
			int x;
			if (items[i]->x < xmin) xmin = items[i]->x;
			if (items[i]->y-16 < ymin) ymin = items[i]->y - 16;
			if (items[i]->y > ymax) ymax = items[i]->y;
			// But what is the max x?
			char intstr[1024];
			char floatstr[1024];
			switch (items[i]->itemType)
			{
				case kSlider:
					x = items[i]->x + 16 + items[i]->state*8; break;
				case kBox:
				case kColorClicker:
				case kSmallColorClicker:
				case kButton:
				case kMenuItem:
				case kMenu:
				case kRightStepper:
				case kLeftStepper:
//					x = items[i]->x + 16; break;
					x = items[i]->x + items[i]->hw; break;
				case kCheckbox:
				case kRadioItem:
					x = items[i]->x + 16 + 15 + strlen(items[i]->s)*8; break;
				case kString:
				case kDynamicString:
					GetTextDimensions(items[i]->s, &w, &h);
					x = items[i]->x + w * (font.characterWidth + font.extraSpace) + 6;
					if (items[i]->y + h*gSpacing > ymax) ymax = items[i]->y + h*gSpacing;
					break;
					//x = items[i]->x + strlen(items[i]->s)*(font.characterWidth + font.extraSpace); break;
				case kDisplayInt:
					sprintf(intstr, "%d", *((int *)items[i]->var1));
					x = items[i]->x + (strlen(items[i]->s) + 2 + strlen(intstr))*(font.characterWidth + font.extraSpace);
					break;
				case kDisplayFloat:
					sprintf(floatstr, "%g", *((float *)items[i]->var1));
					x = items[i]->x + (strlen(items[i]->s) + 2 + strlen(floatstr))*(font.characterWidth + font.extraSpace);
					break;
			}
			if (x > xmax) xmax = x; 
		}
		
		glUseProgram(sfBoxProgram);
		glBindVertexArray(vertexArrayObjID);	// Select VAO

		glUniform1i(glGetUniformLocation(sfBoxProgram, "offsx"), offsx);
		glUniform1i(glGetUniformLocation(sfBoxProgram, "offsy"), offsy);

		glUniform1i(glGetUniformLocation(sfBoxProgram, "screenSizeX"), gRasterH);
		glUniform1i(glGetUniformLocation(sfBoxProgram, "screenSizeY"), gRasterV);
		glUniform1i(glGetUniformLocation(sfBoxProgram, "x"), xmin - bgBorder);
		glUniform1i(glGetUniformLocation(sfBoxProgram, "y"), ymin - bgBorder);
		glUniform1f(glGetUniformLocation(sfBoxProgram, "charWidth"), xmax - xmin + bgBorder * 2);
		glUniform1f(glGetUniformLocation(sfBoxProgram, "charHeight"), ymax - ymin + bgBorder * 2);
		glUniform1f(glGetUniformLocation(sfBoxProgram, "red"), bgRed);
		glUniform1f(glGetUniformLocation(sfBoxProgram, "green"), bgGreen);
		glUniform1f(glGetUniformLocation(sfBoxProgram, "blue"), bgBlue);
		glUniform1f(glGetUniformLocation(sfBoxProgram, "alpha"), bgAlpha);
		glDrawArrays(GL_TRIANGLES, 0, 6);	// draw object
// Done drawing background
	}

	glUseProgram(sfProgram);
	glBindVertexArray(vertexArrayObjID);	// Select VAO
	// Uniforms that are the same over all items
	glUniform1i(glGetUniformLocation(sfProgram, "screenSizeX"), gRasterH);
	glUniform1i(glGetUniformLocation(sfProgram, "screenSizeY"), gRasterV);
	glUniform1f(glGetUniformLocation(sfProgram, "charWidth"), font.characterWidth);
	glUniform1f(glGetUniformLocation(sfProgram, "charHeight"), font.characterHeight);
	glUniform1i(glGetUniformLocation(sfProgram, "texWidth"), font.texWidth);
	glUniform1i(glGetUniformLocation(sfProgram, "texHeight"), font.texHeight);
	glUniform1i(glGetUniformLocation(sfProgram, "offsx"), offsx);
	glUniform1i(glGetUniformLocation(sfProgram, "offsy"), offsy);	

// Draw items
	if (items != NULL)
	for (int i = 0; items[i] != NULL; i++)
	{
		
		int h = items[i]->x;
		int v = items[i]->y;

		switch (items[i]->itemType)
		{
		case kSlider:
		{
			//int saveh = h;
			int lefth = h;
			
			// Draw inside and frame

			drawCharAndBack(h, v, sliderbarleft, sliderbarinside, frameRed, frameGreen, frameBlue, items[i]->r, items[i]->g, items[i]->b);
			h += font.characterWidth;
			for (int c = 0; c < items[i]->state; c++)
			{
				drawCharAndBack(h, v, sliderbarmiddle, sliderbarinside, frameRed, frameGreen, frameBlue, items[i]->r, items[i]->g, items[i]->b);
				h += font.characterWidth;
			}
			drawCharAndBack(h, v, sliderbarright, sliderbarinside, frameRed, frameGreen, frameBlue, items[i]->r, items[i]->g, items[i]->b);

			// Draw thumb
			// Update intData if the value has changed
			if (items[i]->var1 != NULL)
			{
				float var1s = (*((float *)items[i]->var1) - items[i]->sliderMin) / (items[i]->sliderMax - items[i]->sliderMin); // Normalize the slider value
				if (var1s < 0) var1s = 0; // Even if the value is out of bounds, make the thumb stay within the slider
				if (var1s > 1) var1s = 1;
				items[i]->intData = var1s * (items[i]->hw - 8);
			}
			h = lefth + items[i]->intData;
			drawCharAndBack(h, v, thumb, thumbinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
			break;
		}

		case kBox: // Box (or drop) that displays a color
		{
			//int saveh = h;
			float r = 0.5, g = 0.5, b = 0.5;
			
			if (items[i]->var1 != NULL)
				r = *((float *)items[i]->var1);
			if (items[i]->var2 != NULL)
				g = *((float *)items[i]->var2);
			if (items[i]->var3 != NULL)
				b = *((float *)items[i]->var3);
			
			if (items[i]->state == 1)
			{
				drawCharAndBack(h, v, dropleft, dropleftinside, frameRed, frameGreen, frameBlue, r, g, b);
				h += font.characterWidth;
				drawCharAndBack(h, v, dropright, droprightinside, frameRed, frameGreen, frameBlue, r, g, b);
			}
			else
			{
				drawCharAndBack(h, v, bigboxleft, bigboxleftinside, frameRed, frameGreen, frameBlue, r, g, b);
				h += font.characterWidth;
				drawCharAndBack(h, v, bigboxright, bigboxrightinside, frameRed, frameGreen, frameBlue, r, g, b);
			}
			break;
		}
		
		case kColorClicker: // Box to select a color
		{
			drawCharAndBack(h, v, 0, bigboxleftinside, frameRed, frameGreen, frameBlue, items[i]->r, items[i]->g, items[i]->b);
			h += font.characterWidth;
			drawCharAndBack(h, v, 0, bigboxrightinside, frameRed, frameGreen, frameBlue, items[i]->r, items[i]->g, items[i]->b);
			break;
		}
		
		case kSmallColorClicker: // Box to select a color
		{
			drawCharAndBack(h, v, 0, smallbox, frameRed, frameGreen, frameBlue, items[i]->r, items[i]->g, items[i]->b);
			break;
		}
		
		case kCheckbox:
		{
			//int saveh = h;
			int state;
			if (items[i]->var1 != NULL)
			{
				state = *((int *)items[i]->var1);
			}
			else
				state = items[i]->state;
			
			if (state != 0)
			{
				drawCharAndBack(h, v, bigboxleftchecked, bigboxleftinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
				h += font.characterWidth;
				drawCharAndBack(h, v, bigboxrightchecked, bigboxrightinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
			}
			else
			{
				drawCharAndBack(h, v, bigboxleft, bigboxleftinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
				h += font.characterWidth;
				drawCharAndBack(h, v, bigboxright, bigboxrightinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
			}

			h += 15;
			v += 3;
			drawString(h, v, items[i]->s);
			break;
		}

		case kRadioItem:
		{
			int saveh = h;
			int state;
			if (items[i]->var1 != NULL)
			{
				state = *((int *)items[i]->var1);
			}
			else
				state = items[i]->state;
			
			drawCharAndBack(h, v, circleleft, circleleftinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
			h += font.characterWidth;
			drawCharAndBack(h, v, circleright, circlerightinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);

			if (state == items[i]->intData)
			{
				h = saveh;

				SetColor(font.fontColorRed, font.fontColorGreen, font.fontColorBlue); // frame
				drawChar(h, v-font.characterHeight, circleleftspot);
				h += font.characterWidth;
				drawChar(h, v-font.characterHeight, circlerightspot);
			}

			h += 15;
			v += 3;
			drawString(h, v, items[i]->s);
			break;
		}

		case kRightStepper:
			if (items[i]->state == 0)
				drawCharAndBack(h, v, stepright, steprightinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
			else
				drawCharAndBack(h, v, stepright, steprightinside, frameRed, frameGreen, frameBlue, fmod(fillRed + 0.5, 1), fmod(fillGreen + 0.5, 1), fmod(fillBlue + 0.5, 1));
			break;

		case kLeftStepper:
			if (items[i]->state == 0)
				drawCharAndBack(h, v, stepleft, stepleftinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
			else
				drawCharAndBack(h, v, stepleft, stepleftinside, frameRed, frameGreen, frameBlue, fmod(fillRed + 0.5, 1), fmod(fillGreen + 0.5, 1), fmod(fillBlue + 0.5, 1));
			break;

		case kButton:
		{
			int saveh = h;
			int theSize;
			if (items[i]->s != NULL)
				theSize = strlen(items[i]->s)*10/8; // 10 = charwith + extra
			else
				theSize = 1;
			// Buttons do NOT take state from a variable but from events only
			
			#define cmod 0.8
			if (items[i]->state == 0)
			{
				drawCharAndBack(h, v, circleleft, circleleftinside, frameRed, frameGreen, frameBlue, fmod(frameRed+cmod, 1), fmod(frameGreen+cmod, 1), fmod(frameBlue+cmod, 1));
				h += font.characterWidth;
				for (int i = 0; i < theSize; i++)
				{
					drawCharAndBack(h, v, topbottom, topbottominside, frameRed, frameGreen, frameBlue, fmod(frameRed+cmod, 1), fmod(frameGreen+cmod, 1), fmod(frameBlue+cmod, 1));
					h += font.characterWidth;
				}
				drawCharAndBack(h, v, circleright, circlerightinside, frameRed, frameGreen, frameBlue, fmod(frameRed+cmod, 1), fmod(frameGreen+cmod, 1), fmod(frameBlue+cmod, 1));
			}
			else
			{
				drawCharAndBack(h, v, circleleft, circleleftinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
				h += font.characterWidth;
				for (int i = 0; i < theSize; i++)
				{
					drawCharAndBack(h, v, topbottom, topbottominside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
					h += font.characterWidth;
				}
				drawCharAndBack(h, v, circleright, circlerightinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
			}

			h = saveh + 8;
			v += 3;
			// Different color depending on state
			if (items[i]->state == 0)
			{
				glUniform1f(glGetUniformLocation(sfProgram, "red"), textRed);
				glUniform1f(glGetUniformLocation(sfProgram, "green"), textGreen);
				glUniform1f(glGetUniformLocation(sfProgram, "blue"), textBlue);
			}
			else
			{
				glUniform1f(glGetUniformLocation(sfProgram, "red"), 1-textRed);
				glUniform1f(glGetUniformLocation(sfProgram, "green"), 1-textGreen);
				glUniform1f(glGetUniformLocation(sfProgram, "blue"), 1-textBlue);
			}
			char *s = items[i]->s;
			for (;*s != 0;s++)
			{
				drawChar(h, v-font.characterHeight, *s); // -10 to get to the proper height
				h += font.characterWidth + font.extraSpace;
			}
			break;
		}

		case kMenu:
		{
			int saveh = h;
			//int state;
			int theSize;
				theSize = items[i]->intData*10/8; // 10 = charwith + extra
			
			SetColor(font.fontColorRed, font.fontColorGreen, font.fontColorBlue);

			for (int i = 0; i < theSize+2; i++)
			{
				drawChar(h, v-16, bigboxleftinside); // -10 to get to the proper height
				h += font.characterWidth;
			}

			h = saveh + 8;
			v += 3;
			
			SetColor(1-textRed, 1-textGreen, 1-textBlue);
			char *s = items[i]->s;
			for (;*s != 0;s++)
			{
				drawChar(h, v-font.characterHeight, *s); // -10 to get to the proper height
				h += font.characterWidth + font.extraSpace;
			}
			break;
		}

		case kString:
		case kDynamicString:
		{
			drawText(h, v, items[i]->s, gSpacing); break;
		}
		
		case kDisplayInt:
		{
			drawString(h, v, items[i]->s);
			h += strlen(items[i]->s)*10;
			
			if (items[i]->var1 != NULL)
			{
				char intstr[1024];
				sprintf(intstr, "%d", *((int *)items[i]->var1));
				drawString(h, v, intstr);
			}
			break;
		}
		
		case kDisplayFloat:
		{
			drawString(h, v, items[i]->s);
			h += strlen(items[i]->s)*10;

			if (items[i]->var1 != NULL)
			{
				char floatstr[1024];
				sprintf(floatstr, "%g", *((float *)items[i]->var1));
				drawString(h, v, floatstr);
			}
			break;
		}
	} // case switch
	} // for
	
	// Draw menu items last!
	if (items != NULL)
	for (int i = 0; items[i] != NULL; i++)
	{
		int h = items[i]->x;
		int v = items[i]->y;
		
		if (items[i]->itemType == kMenuItem)
		{
			if (items[i]->state != 0) // not hidden
			{
				int saveh = h;
				//int state;
				int theSize;
				if (items[i]->s != NULL)
//					theSize = strlen(items[i]->s)*10/8; // 10 = charwith + extra
					theSize = items[i]->intData*10/8; // 10 = charwith + extra
				else
					theSize = 1;
				// Buttons do NOT take state from a variable but from events only
				
				// for a number depending on string length
				for (int ii = 0; ii < theSize+2; ii++)
				{
					drawCharAndBack(h, v, 0, bigboxleftinside, frameRed, frameGreen, frameBlue, fillRed, fillGreen, fillBlue);
					h += font.characterWidth;
				}

				h = saveh + 8;
				v += 3;

				glUniform1f(glGetUniformLocation(sfProgram, "red"), textRed);
				glUniform1f(glGetUniformLocation(sfProgram, "green"), textGreen);
				glUniform1f(glGetUniformLocation(sfProgram, "blue"), textBlue);
				char *s = items[i]->s;
				for (;*s != 0;s++)
				{
					drawChar(h, v-font.characterHeight, *s); // -10 to get to the proper height
					h += font.characterWidth + font.extraSpace;
				}
			}
		}
	}

	
	glBindTexture(GL_TEXTURE_2D, tex);
	glActiveTexture(activeTxtUnit); // or should this be GL_TEXTURE0 + activeTxtUnit? Needs testing!
	glUseProgram(saveprogram);
	if (saveZ) glEnable(GL_DEPTH_TEST);
	if (saveCull) glEnable(GL_CULL_FACE);
	if (!saveBlend) glDisable(GL_BLEND);
	glBlendFunc(savedSrcAlpha, savedDstAlpha);
	glBindVertexArray(0); // Make sure the host program doesn't mess up our VAO
}

// We need different "hit" functions to perform what each control does on a click.

static int hitBox(Item *item, int x, int y)
{
	// x, y inside item->hx, item->hy and item->hx+hw, item->hy+hh = inside the hitbox
	if (x >= item->hx && y >= item->hy-item->hh && x <= item->hx+item->hw && y <= item->hy)
	{
		// Also set the variable connected to the checkbox
		// If an external variable is referred, change it, otherwise only an internal state
		if (item->var1 != NULL)
		{
			*((int *)item->var1) = 1 - *((int *)item->var1);
			item->state = *((int *)item->var1);
		}
		else
			item->state = 1 - item->state; // switch between 0 and 1
		return 1;
	}
	return 0;
}

static int hitColorClicker(Item *item, int x, int y)
{
	if (x >= item->hx && y >= item->hy-item->hh && x <= item->hx+item->hw && y <= item->hy)
	{
		// Set the variable connected to the clicker
		if (item->var1 != NULL)
			*((float *)item->var1) = item->r;
		if (item->var2 != NULL)
			*((float *)item->var2) = item->g;
		if (item->var3 != NULL)
			*((float *)item->var3) = item->b;
		return 1;
	}
	return 0;
}

static int hitRadio(Item *item, int x, int y)
{
	if (x > item->hx && y > item->hy-item->hh && x < item->hx+item->hw && y < item->hy)
	{
		if (item->var1 != NULL)
		{
			*((int *)item->var1) = item->intData;
			item->state = *((int *)item->var1);
		}
		else
			item->state = item->intData;
		return 1;
	}
	return 0;
}

// Just test the hit. Also used for menu items.
static int hitButton(Item *item, int x, int y)
{
	if (x > item->hx && y > item->hy-item->hh && x < item->hx+item->hw && y < item->hy)
	{
		return 1;
	}
	return 0;
}

static int hitSlider(Item *item, int x, int y)
{
	if (x >= item->hx && y > item->hy-item->hh && x <= item->hx+item->hw && y < item->hy)
	{
		item->intData = x - item->hx;
		if (item->intData < 0) item->intData = 0;
		if (item->intData > (item->hw - 4)) item->intData = (item->hw - 8);
		
		if (item->var1 != NULL)
		{
			*((float *)item->var1) = 1.0*item->intData / (item->hw - 8);
			// Then scale to range
			*((float *)item->var1) *= (item->sliderMax - item->sliderMin);
			*((float *)item->var1) += item->sliderMin;
		}
		return 1;
	}
	return 0;
}

//static float myTime = 0;
static int lastxloc = 0, lastyloc = 0;

// Help function for the item creation functions.
static int sgCreateItem(int x, int y)
{
	// Auto-placement
	if (x < 0) x = lastxloc;
	if (y < 0) y = lastyloc + gSpacing;
	lastxloc = x;
	lastyloc = y;
	
	if (items == NULL)
	{
		items = malloc(sizeof(Item *) * 2);
		items[0] = malloc(sizeof(Item));
		items[1] = NULL;
		itemCount++;
	}
	else
	{
		// Try reusing disabled items
		for (int i = 0; i < itemCount; i++)
			if (items[i]->itemType == -1)
				return i;		
		
		// Otherwise expand the array.
		items = realloc(items, sizeof(Item *) * (itemCount+2));
		items[itemCount] = malloc(sizeof(Item));
		items[itemCount+1] = NULL;
		itemCount++;
	}
	items[itemCount-1]->x = x;
	items[itemCount-1]->y = y;
	items[itemCount-1]->hx = x; // Common default
	items[itemCount-1]->hy = y;
	return itemCount-1; // # of new item
}

// A slider. Should connect to a variable, which is set to a value between 0 and 1.
int sgCreateSlider(int x, int y, int w, float *variable, float min, float max)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kSlider;
	
	w = (w / 8)*8; // or w = w and 0xfff8
	
	items[i]->hw = w; // - 8;
	items[i]->hh = 16; // items[i]->h;

	items[i]->state = (w-16) / 8; // # of segments
	items[i]->intData = 0; // or calc from *variable
	items[i]->var1 = variable;
	
	items[i]->sliderMin = min;
	items[i]->sliderMax = max;
	
	items[i]->r = sliderFillRed;
	items[i]->g = sliderFillGreen;	
	items[i]->b = sliderFillBlue;	
	
	return i;
}

// A square color box. I think the drop is better, doesn't look like a checkbox.
int sgCreateColorBox(int x, int y, float *r, float *g, float *b)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kBox;
	items[i]->hx = 0;//x+2; // Adjusted to fit
	items[i]->hy = 0;//y;
	items[i]->hw = 0;//w-4;
	items[i]->hh = 0;//h;
	items[i]->var1 = r;
	items[i]->var2 = g;
	items[i]->var3 = b;
	items[i]->state = 0; // 1 = drop
	return i;
}

// A drop-shaped color box.
int sgCreateColorDrop(int x, int y, float *r, float *g, float *b)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kBox;
	items[i]->hx = 0;//x+2; // Adjusted to fit
	items[i]->hy = 0;//y;
	items[i]->hw = 0;//w-4;
	items[i]->hh = 0;//h;
	items[i]->var1 = r;
	items[i]->var2 = g;
	items[i]->var3 = b;
	items[i]->state = 1; // 1 = drop
	return i;
}

// Created three sliders and a "drop" color box for color selection. Controls three float variables.
void sgCreateSliderColorGroup(int x, int y, int w, float *r, float *g, float *b)
{
	sgCreateColorDrop(x, y+20, r, g, b);
	int rs = sgCreateSlider(x+30, y, w, r, 0, 1);
	int gs = sgCreateSlider(x+30, y+20, w, g, 0, 1);
	int bs = sgCreateSlider(x+30, y+40, w, b, 0, 1);
	
	sgSetSliderFillColor(rs, 1, 0, 0);
	sgSetSliderFillColor(gs, 0, 1, 0);
	sgSetSliderFillColor(bs, 0, 0, 1);
}

// Checkbox = box + string. Drawn with or without the X mark.
int sgCreateCheckBox(int x, int y, const char *s, int *variable)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kCheckbox;
	items[i]->state = 0;
	items[i]->hw = 16; // items[i]->w;
	items[i]->hh = 16; // items[i]->h;
	items[i]->var1 = variable;
	items[i]->s = calloc(strlen(s)+1, 1);
	strcpy(items[i]->s, s);
	return i;
}

// A plan static string that is not supposed to be changed.
int sgCreateStaticString(int x, int y, const char *s)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kString;
	items[i]->s = calloc(strlen(s)+1, 1);
	strcpy(items[i]->s, s);
	return i;
}

// A string that is stored elsewhere and that can change.
int sgCreateDynamicString(int x, int y, char *s)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kDynamicString;
	items[i]->s = s;
	return i;
}

// String + number connected to an integer variable
int sgCreateDisplayInt(int x, int y, const char *s, int *variable)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kDisplayInt;
	items[i]->s = calloc(strlen(s)+1, 1);
	items[i]->var1 = variable;
	strcpy(items[i]->s, s);
	return i;
}

// String + number connected to a float variable
int sgCreateDisplayFloat(int x, int y, const char *s, float *variable)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kDisplayFloat;
	items[i]->var1 = variable;
	items[i]->s = calloc(strlen(s)+1, 1);
	strcpy(items[i]->s, s);
	
	return i;
}

// A radio button. Make several connected to the same variable to create a radio group.
int sgCreateRadio(int x, int y, const char *s, int *variable, int index)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kRadioItem;
	items[i]->state = 0;
	items[i]->hw = 16;
	items[i]->hh = 16;
	items[i]->intData = index; // set value to this value when hit
	items[i]->var1 = variable;
	items[i]->s = calloc(strlen(s)+1, 1);
	strcpy(items[i]->s, s);
	return i;
}

// A pushbutton.
int sgCreateButton(int x, int y, const char *s, NoArgProcPtr callback)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kButton;
	items[i]->state = 0;
//	items[i]->hw = 16 + 16 + 8 * strlen(s);
	items[i]->hw = strlen(s)*(10) + 16; //font.characterWidth + font.extraSpace);
	items[i]->hh = 16;
	items[i]->s = calloc(strlen(s)+1, 1);
	strcpy(items[i]->s, s);
	items[i]->var1 = (void *)callback;
	return i;
}

void UpdateMenuSize(int i)
{
	int maxLen = strlen(items[i]->s);
	for (int j = i; j > 0 && items[j] != NULL && (items[j]->itemType == kMenuItem || items[j]->itemType == kMenu); j--)
		if (strlen(items[j]->s) > maxLen)
			maxLen = strlen(items[j]->s);
	for (int j = i; j > 0 && items[j] != NULL && (items[j]->itemType == kMenuItem || items[j]->itemType == kMenu); j--)
	{
		items[j]->intData = maxLen;
		items[j]->hw = maxLen*10 + 16;		
	}
}

// A menu header. Can show or hide following menu items
int sgCreateMenu(int x, int y, const char *s)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kMenu;
	items[i]->state = 0; // Menu not visible
//	items[i]->hw = 16 + 16 + 8 * strlen(s);
	items[i]->hw = strlen(s)*(10) + 16; //font.characterWidth + font.extraSpace);
	items[i]->hh = 16;
	items[i]->s = calloc(strlen(s)+1, 1);
	strcpy(items[i]->s, s);
	return i;
}

// A menu item. Works just like a button.
int sgCreateMenuItem(const char *s, NoArgProcPtr callback)
{
	if (itemCount < 1) return 0;
	int x, y;
	x = items[itemCount-1]->x;
	y = items[itemCount-1]->y+16;
	int i = sgCreateItem(x, y);
	items[i]->itemType = kMenuItem;
	items[i]->state = 0; // Hidden
//	items[i]->hw = 16 + 16 + 8 * strlen(s);
	items[i]->hw = 0; // Do not hit. Changed on hit in kMenu
//	items[i]->hw = strlen(s)*(10) + 16; //font.characterWidth + font.extraSpace);
	items[i]->hh = 16;
	items[i]->s = calloc(strlen(s)+1, 1);
	strcpy(items[i]->s, s);
	items[i]->var1 = (void *)callback;
	UpdateMenuSize(i);
	return i;
}

// Created three sliders with strings to the right of each. Controls three float variables in an array (cast vec3 to *float).
void sgCreateSliderVec3Group(int x, int y, int w, float *v, float min, float max)
{
	sgCreateSlider(x, y, w, &v[0], min, max); sgCreateDisplayFloat(x + w + 20, y, "", &v[0]);
	sgCreateSlider(x, y+20, w, &v[1], min, max); sgCreateDisplayFloat(x + w + 20, y+20, "", &v[1]);
	sgCreateSlider(x, y+40, w, &v[2], min, max); sgCreateDisplayFloat(x + w + 20, y+40, "", &v[2]);
}

// Created three sliders with strings to the right of each. Controls three float variables in an array (cast vec3 to *float).
int sgCreateColorClicker(int x, int y, float r, float g, float b, float *dr, float *dg, float *db)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kColorClicker;
	items[i]->hw = 16;//w-4;
	items[i]->hh = 16;//h;
	items[i]->var1 = dr;
	items[i]->var2 = dg;
	items[i]->var3 = db;
	items[i]->r = r; // Internal color
	items[i]->g = g;
	items[i]->b = b;
	return i;
}

// Created three sliders with strings to the right of each. Controls three float variables in an array (cast vec3 to *float).
int sgCreateSmallColorClicker(int x, int y, float r, float g, float b, float *dr, float *dg, float *db)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kSmallColorClicker;
	items[i]->hx = x+0;//x+2; // Adjusted to fit
	items[i]->hy = y-8;//y;
	items[i]->hw = 8;//w-4;
	items[i]->hh = 8;//h;
	items[i]->var1 = dr;
	items[i]->var2 = dg;
	items[i]->var3 = db;
	items[i]->r = r; // Internal color
	items[i]->g = g;
	items[i]->b = b;
	return i;
}

void internalCreateColorPalette(int x, int y, float *dr, float *dg, float *db, int tiny)
{
	for (int i = 0; i < 12; i++)
		for (int j = 0; j < 5; j++)
		{
			// i = all eight corners except black and white
			// j = value
			
			float r,g,b;
			
			switch (i)
			{
				case 0:
					r = 1; g = 0; b = 0;break;
				case 1:
					r = 1; g = 0.5; b = 0;break;
				case 2:
					r = 1; g = 1; b = 0;break;
				case 3:
					r = 0.5; g = 1; b = 0;break;
				case 4:
					r = 0; g = 1; b = 0;break;
				case 5:
					r = 0; g = 1; b = 0.5;break;
				case 6:
					r = 0; g = 1; b = 1;break;
				case 7:
					r = 0; g = 0.5; b = 1;break;
				case 8:
					r = 0; g = 0; b = 1;break;
				case 9:
					r = 0.5; g = 0; b = 1;break;
				case 10:
					r = 1.0; g = 0; b = 1;break;
				case 11:
					r = 1; g = 0; b = 0.5;break;
				break;
			}
			if (j <= 1)
			{
				if (r == 0.5) r = 0.75;
				if (g == 0.5) g = 0.75;
				if (b == 0.5) b = 0.75;
				if (r == 0) r = 0.5;
				if (g == 0) g = 0.5;
				if (b == 0) b = 0.5;
			}
			float q;
			if (j == 0)
				q = 0.75;
			else
			if (j == 1)
				q = 1.0;
			else
				q = (float)(6-j)/4.0;
			if (tiny)
				sgCreateSmallColorClicker(x + i * 8, y + j * 8, r*q, g*q, b*q, dr, dg, db);
			else
				sgCreateColorClicker(x + i * 16, y + j * 16, r*q, g*q, b*q, dr, dg, db);
		}
	for (int i = 0; i < 12; i++)
	{
		// Grayscales!
		if (tiny)
			sgCreateSmallColorClicker(x + i * 8, y + 5 * 8, i / 11.0, i/11.0, i/11.0, dr, dg, db);
		else
			sgCreateColorClicker(x + i * 16, y + 5 * 16, i / 11.0, i/11.0, i/11.0, dr, dg, db);
	}
}

void sgCreateColorPalette(int x, int y, float *dr, float *dg, float *db)
{
	internalCreateColorPalette(x, y, dr, dg, db, 0);
}

void sgCreateSmallColorPalette(int x, int y, float *dr, float *dg, float *db)
{
	internalCreateColorPalette(x, y, dr, dg, db, 1);
}


int sgCreateLeftStepper(int x, int y, int *variable)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kLeftStepper;
	items[i]->state = 0;
	items[i]->hw = 8; //font.characterWidth + font.extraSpace);
	items[i]->hh = 16;
	items[i]->var1 = (void *)variable;
	return i;
}

int sgCreateRightStepper(int x, int y, int *variable)
{
	int i = sgCreateItem(x, y);
	items[i]->itemType = kRightStepper;
	items[i]->state = 0;
	items[i]->hw = 8; //font.characterWidth + font.extraSpace);
	items[i]->hh = 16;
	items[i]->var1 = (void *)variable;
	return i;
}

void sgCreateSteppers(int x, int y, int *variable)
{
	sgCreateLeftStepper(x, y, variable);
	sgCreateRightStepper(x+10, y, variable);
}



void sgRemoveItem(int i)
{
	if (i >= 0 && i < itemCount)
	{
		if (items[i]->itemType == kString)
			free(items[i]->s);
		items[i]->itemType = -1;
		// I only disable, set the item to a nonexisting type, not dispose. This is not
		// a memory leak since it will be reused if any more item are allocated.
	}
}

int lasthit; // Remember drag
//int activeButton = -1; // Remember button

// For dragging bg box
int lastx;
int lasty;
int hasHitColorClicker = 0;

// Should be called when the mouse is pressed. Returns 1 if something was hit.
int sgMouse(int state, int x, int y)
{
	x = x / gScale * mscale;
	y = y / gScale * mscale;
	
	x -= offsx;
	y -= offsy;
	hasHitColorClicker = 0;
	
	if (state == 0) // DOWN
	{
		// Must test menu items first!
		for (int i = 0; items[i] != NULL; i++)
		{
			if (items[i]->itemType == kMenuItem)
			if (items[i]->state != 0)
			{
				if (hitButton(items[i], x, y))
				{
					// Call callback function!
					if (items[i]->var1 != NULL)
						((NoArgProcPtr)items[i]->var1)();
					for (int j = i; j > 0 && items[j] != NULL && items[j]->itemType == kMenuItem; j--)
						items[j]->state = 0;
					for (int j = i; items[j] != NULL && items[j]->itemType == kMenuItem; j++)
						items[j]->state = 0;
					return 1;
				}
			}
		}
		
		// Then we test the rest.
		if (items != NULL)
		for (int i = 0; items[i] != NULL; i++)
		{
			switch (items[i]->itemType)
			{
			case kSlider:
				if (hitSlider(items[i], x, y))
				{
					lasthit = i; // This is really the only thing lasthit is needed for.
					return 1;
				}
				break;
			case kCheckbox:
				if (hitBox(items[i], x, y))
				{
					lasthit = i;
					return 1;
				}
				break;
			case kColorClicker:
			case kSmallColorClicker:
				if (hitColorClicker(items[i], x, y))
				{
					lasthit = i;
					hasHitColorClicker = 1;
					return 1;
				}
				break;
			case kRadioItem:
				if (hitRadio(items[i], x, y))
				{
					lasthit = i;
					return 1;
				}
				break;
			case kButton:
			case kLeftStepper:
			case kRightStepper:
				if (hitButton(items[i], x, y))
				{
					activeButton = i;
					items[activeButton]->state = 1;
					return 1;
				}
				break;
			case kMenu:
				if (hitButton(items[i], x, y))
				{
					items[i]->state = items[i]->state;
					for (int j = i+1; items[j] != NULL && items[j]->itemType == kMenuItem; j++)
					{
						items[j]->state = 1 - items[j]->state;
						items[j]->hw = strlen(items[j]->s)*(10) + 16;
					}
					return 1;
				}
				break;
			}
		}
		
		// Finally test the bg box:
		if (x > xmin-bgBorder && y > ymin-bgBorder && x < xmax+bgBorder && y < ymax+bgBorder)
		{
			lasthit = -2; // Special index for box
			lastx = x;
			lasty = y;
			return 1;
		}
	}
	else // state = 1 = mouse up
	{
		if (activeButton > -1)
		{
			items[activeButton]->state = 0;
			if (hitButton(items[activeButton], x, y)) // Mouse up inside button = valid
			{
				
				if (items[activeButton]->var1 != NULL)
				{
					switch (items[activeButton]->itemType)
					{
						case kButton:
								((NoArgProcPtr)items[activeButton]->var1)(); break;
						case kLeftStepper:
								(*(int *)items[activeButton]->var1)--; break;
						case kRightStepper:
								(*(int *)items[activeButton]->var1)++; break;	
					}
				}
			}
			activeButton = -1;
		}
	}
	
	lasthit = -1; // Nothing hit
	return 0;
}

// Should be called when the mouse moved when the button is down. Returns 1 if something was hit.
int sgMouseDrag(int x, int y)
{
 	x = x / gScale * mscale;
 	y = y / gScale * mscale;
 
 	x -= offsx;
	y -= offsy;
	
	// NEW: "Lazy slider", allows a drag to be outside the slider.
	if (lasthit >= 0)
	{
		if (items[lasthit]->itemType == kSlider)
		{
			y = items[lasthit]->hy-1;
			if (x < items[lasthit]->hx) x = items[lasthit]->hx;
			if (x > items[lasthit]->hx+items[lasthit]->hw) x = items[lasthit]->hx+items[lasthit]->hw;
			hitSlider(items[lasthit], x, y);
			return 1;
		}
	}
	
	if (items != NULL)
	for (int i = 0; items[i] != NULL; i++)
	{		
		if (items[i]->itemType == kColorClicker || items[i]->itemType == kSmallColorClicker)
			if (hasHitColorClicker)
			// I should also check if it is in the same range as the clicked color clicker.
				if (hitColorClicker(items[i], x, y))
				{
					lasthit = i;
					return 1;
				}
	}
	
	if (activeButton > -1)
	{
		items[activeButton]->state = (hitButton(items[activeButton], x, y)); // Mouse up inside button = valid
	}

	// If we get here, we do not drag anything else
	if (lasthit == -2) // hit bg box
	{
		offsx += x - lastx;
		offsy += y - lasty;
		return 1;
	}

	// If we drag outside controls but inside the bg box, ignore
	if (x > xmin-bgBorder && y > ymin-bgBorder && x < xmax+bgBorder && y < ymax+bgBorder)
	{
		return 1;
	}

	return 0;
}

void sgSetFrameColor(float r, float g, float b)
{
	frameRed = r;
	frameGreen = g;
	frameBlue = b;
}

void sgSetFillColor(float r, float g, float b)
{
	fillRed = r;
	fillGreen = g;
	fillBlue = b;
}

void sgSetSliderFillColor(int sliderID, float r, float g, float b)
{
	items[sliderID]->r = r;
	items[sliderID]->g = g;
	items[sliderID]->b = b;
}

void sgSetSliderDefaultFillColor(float r, float g, float b)
{
	sliderFillRed = r;
	sliderFillGreen = g;
	sliderFillBlue = b;
}

void sgSetBackgroundColor(float r, float g, float b, float a)
{
	bgRed = r;
	bgGreen = g;
	bgBlue = b;
	bgAlpha = a;
}

void sgSetBackgroundBorder(int b)
{
	bgBorder = b;
}

void sgSetTextColor(float r, float g, float b)
{
	textRed = r;
	textGreen = g;
	textBlue = b;
}

void sgSetPosition(int x, int y)
{
	offsx = x;
	offsy = y;
}

// Spacing for auto-placement and multi-line text
void sgSetSpacing(int spacing)
{
	gSpacing = spacing;
}

void sgSetScale(int scale)
{
	gScale = scale;
}
