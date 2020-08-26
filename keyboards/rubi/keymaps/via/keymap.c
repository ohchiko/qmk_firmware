/* Copyright 2020 gregorio
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include <stdio.h>
#include <print.h>

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BASE,
    _FN
};
char wpm_str[10];

// 60 sec
#define OLED_SLEEP_TIMEOUT 60000
// 30 fps
#define OLED_FRAME_TIMEOUT (1000/30)
// 3 sec
#define OLED_LOGO_TIMEOUT 3000
// oled timers
static uint32_t oled_sleep_timer = 0;
static uint16_t oled_frame_timer = 0;
static uint16_t oled_logo_timer = 0;
// indicator if logo timer expired
static bool oled_logo_expired = false;

static const char PROGMEM raw_logo[] = {
                                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128,128,  0,  0,  0,  0,  0,  0,  0,128,128,128,128,128,128,128,128,128,128,128,  0,  0,  0,  0,  0,  0,128,128,128,128,  0,  0,  0,  0,  0,  0,128,128,128,128,  0,  0,128,128,128,128,128,128,128,128,128,128,128,  0,  0,  0,  0,  0,128,128,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,224,112,184,220,204,182,123,254,251,187,223,231,203,126,254,  0,  0,  0,  0,  0,  0,255,  0,  0,  0,248,  8,  8,  8,  8,  8,249,225,  2, 12,248,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,255,255,255,255,  0,  0,255,255,255,255,199,199,199,199,199,207,255,255,254,124,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,127,191,188,211,223,223,247,187,187,252,126,123,247,239,223,190,115,142,  0,  0,  0,  0,  0,255,  0,  0,  0,227, 34, 34, 34, 98,194,  1, 56,236,135,  1,  0,  0, 31, 63,127,255,248,240,240,240,240,248,255,127, 63, 31,  0,  0,255,255,255,255,227,227,227,227,227,243,255,255,127, 62,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
                                        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  0,  0,  0,  0,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
};

// Defines names for use in layer keycodes and the keymap

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(
                                                KC_MUTE,
            KC_NLCK, KC_PSLS, KC_PAST,          KC_PMNS,
            KC_P7,   KC_P8,   KC_P9,            KC_PPLS,
            KC_P4,   KC_P5,   KC_P6,
            KC_P1,   KC_P2,   KC_P3,            KC_PENT,
                     KC_P0,   LT(_FN, KC_PDOT)
            ),
    [_FN] = LAYOUT(
                                       RESET,
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS,
            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                     KC_TRNS, KC_TRNS
            )
};

#ifdef OLED_DRIVER_ENABLE
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        oled_sleep_timer = timer_read32();
    }

    return true;
}
static void render_logo(void) {
    oled_write_raw_P(raw_logo, sizeof(raw_logo));
}

static void render_status(void) {
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(PSTR("Default\n"), false);
            break;
        case _FN:
            oled_write_P(PSTR("Fn\n"), false);
            break;
        default:
            oled_write_P(PSTR("Undefined\n"), false);
    }

    uint8_t led_usb_state = host_keyboard_leds();
    if (IS_LED_ON(led_usb_state, USB_LED_NUM_LOCK)) {
        oled_set_cursor(0, oled_max_lines()-2);
        oled_write_P(PSTR("NUMLCK "), false);
    }
}

void render_frame(void) {
    if (oled_logo_expired) {
        render_status();
    } else {
        render_logo();
        oled_logo_expired = timer_elapsed(oled_logo_timer) > OLED_LOGO_TIMEOUT;
    }
}

void oled_task_user(void) {
    // re-render oled and reset timer if it's time for the next frame
    if (timer_elapsed32(oled_sleep_timer) > OLED_SLEEP_TIMEOUT) {
        oled_off();
        return;
    } else {
        oled_on();
    }

    if (timer_elapsed(oled_frame_timer) > OLED_FRAME_TIMEOUT) {
        oled_clear();
        oled_frame_timer = timer_read();
        render_frame();
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    oled_logo_timer = timer_read();
    oled_frame_timer = timer_read();
    return rotation;
}
#endif

bool led_update_user(led_t led_state) {
    writePin(C6, led_state.num_lock);
    return true;
}

void encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) { /* First encoder */
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
}
