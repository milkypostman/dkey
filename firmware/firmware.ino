/** dkey firmware written in teensyduino.
 *
 * Copyright (c) 2015 Donald Curtis <d@milkbox.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

// Rows starting at 0 (top) to 4 (bottom). The top row wiring is weird
// because I'm an idiot and every other row is wired together, but for
// the top row I forgot there were 5 rows and not 4.

#include <keylayouts.h>
#include <Bounce.h>

#define NUM_ROWS 5
#define NUM_COLS 16
#define BOUNCE_MS 6

// This is defined by the Teensyduino keyboard module.
#define MAX_KEYS_PRESSED 6

int rows[NUM_ROWS][2] = {{PIN_D3, PIN_B4}, {PIN_D7, 0}, {PIN_C6, 0}, {PIN_D4, 0}, {PIN_D5, 0}};
int cols[NUM_COLS] = {PIN_B0, PIN_B1, PIN_B2, PIN_B3, PIN_B7, PIN_D0, PIN_D1, PIN_D2,
                      PIN_B5, PIN_B6, PIN_F7, PIN_F6, PIN_F5, PIN_F4, PIN_F1, PIN_F0};

Bounce *buttons[NUM_ROWS][NUM_COLS];

int keys[NUM_ROWS][NUM_COLS] =
{{KEY_EQUAL, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_F2, KEY_F1, KEY_F5, KEY_F6, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS},
 {KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_F3, KEY_ESC, KEY_TILDE, KEY_F7, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_BACKSLASH},
 {0, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_F4, 0, KEY_PAGE_UP, KEY_F7, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEMICOLON, KEY_QUOTE},
 {0, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, 0, KEY_ESC, KEY_PAGE_DOWN, KEY_TAB, KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_SLASH, 0},
 {KEY_HOME, KEY_TILDE, KEY_LEFT, KEY_RIGHT, KEY_BACKSPACE, 0, 0, KEY_DELETE, KEY_DELETE, KEY_ENTER, KEY_SPACE, KEY_DOWN, KEY_UP, KEY_LEFT_BRACE, KEY_RIGHT_BRACE, KEY_END}};

// Modifiers overwrite keys above.
unsigned int modifiers[NUM_ROWS][NUM_COLS] =
{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {MODIFIERKEY_CTRL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
 {MODIFIERKEY_SHIFT, 0, 0, 0, 0, 0, MODIFIERKEY_ALT, 0, 0, 0, 0, 0, 0, 0, 0, MODIFIERKEY_SHIFT},
 {0, 0, 0, 0, 0, MODIFIERKEY_CTRL, MODIFIERKEY_GUI, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int keyState[NUM_ROWS][NUM_COLS];

void setup() {
    Serial.begin(9600);

    for (int i=0; i < NUM_ROWS; i++) {
        for (int j=0; j < 2; j++) {
            if (rows[i][j]) {
                pinMode(rows[i][j], INPUT);
            }
        }
    }
    for (int i=0; i < NUM_COLS; i++) {
        pinMode(cols[i], INPUT_PULLUP);
    }

    for (int r=0; r < NUM_ROWS; r++) {
        for (int c=0; c < NUM_COLS; c++) {
            keyState[r][c] = 0;
            buttons[r][c] = new Bounce(cols[c], BOUNCE_MS);
        }
    }
}

void loop() {
    int mods = 0;
    int keys_pressed[MAX_KEYS_PRESSED];
    int keys_set = 0;
    for (int i=0; i < MAX_KEYS_PRESSED; i++) {
        keys_pressed[i] = 0;
    }

    for (int r=0; r < NUM_ROWS; r++) {
        for (int i=0; i < 2; i++) {
            if (rows[r][i]) {
                pinMode(rows[r][i], OUTPUT);
            }
        }
        for (int c=0; c < NUM_COLS; c++) {
            buttons[r][c]->update();
            if (buttons[r][c]->fallingEdge()) {
                keyState[r][c] = 1;
            } else if (buttons[r][c]->risingEdge()) {
                keyState[r][c] = 0;
            }
            if (keyState[r][c]) {
                if (modifiers[r][c]) {
                    mods |= modifiers[r][c];
                } else if (keys[r][c] && keys_set < MAX_KEYS_PRESSED) {
                    keys_pressed[keys_set] = keys[r][c];
                    keys_set++;
                }
            }
        }
        for (int i=0; i < 2; i++) {
            if (rows[r][i]) {
                pinMode(rows[r][i], INPUT);
            }
        }
    }
    Keyboard.set_key1(keys_pressed[0]);
    Keyboard.set_key2(keys_pressed[1]);
    Keyboard.set_key3(keys_pressed[2]);
    Keyboard.set_key4(keys_pressed[3]);
    Keyboard.set_key5(keys_pressed[4]);
    Keyboard.set_key6(keys_pressed[5]);
    Keyboard.set_modifier(mods);
    Keyboard.send_now();
}
