#include QMK_KEYBOARD_H

// please change this value to the desired layer definitions
#define LAYERNUM 3

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /*
     * | Knob: Layer +/-            Knob: Volume +/- |
     * .---------------------------------------------.
     * | RGB Toggle | Media Play/Pause | Volume Mute |
     * |------------|------------------|-------------|
     * | Media Prev | Up               | Media Next  |
     * |------------|------------------|-------------|
     * | Left       | Down             | Right       |
     * '---------------------------------------------'
     */
    [0] = LAYOUT(
        RGB_TOG, KC_MPLY, KC_MUTE,
        KC_MPRV, KC_UP, KC_MNXT,
        KC_LEFT, KC_DOWN, KC_RGHT
    ),

    /*
     * | Knob: Layer +/-                Knob: Volume +/- |
     * .-------------------------------------------------.
     * | RGB Toggle     | Media Play/Pause | Volume Mute |
     * |----------------|------------------|-------------|
     * | Media Previous | RGB Hue+         | Media Next  |
     * |----------------|------------------|-------------|
     * | RGB Anim-      | RGB Hue-         | RGB Anim+   |
     * '-------------------------------------------------'
     */
    [1] = LAYOUT(
        _______, _______, _______,
        _______, RGB_HUI, _______,
        RGB_RMOD, RGB_HUD, RGB_MOD
    ),

    /*
     * | Knob: Layer +/-  Knob: Volume +/- |
     * .-----------------------------------.
     * | N/A          | N/A   | N/A        |
     * |--------------|-------|------------|
     * | RESET EEPROM | RESET | DEBUG MODE |
     * |--------------|-------|------------|
     * | N/A          | N/A   | N/A        |
     * '-----------------------------------'
     */
    [2] = LAYOUT(
        XXXXXXX, XXXXXXX, XXXXXXX,
        EEP_RST, RESET  , DEBUG  ,
        XXXXXXX, XXXXXXX, XXXXXXX
    ),

};

const rgblight_segment_t PROGMEM _FIRST_[]  = RGBLIGHT_LAYER_SEGMENTS({0, RGBLED_NUM, HSV_BLUE});
const rgblight_segment_t PROGMEM _SECOND_[] = RGBLIGHT_LAYER_SEGMENTS({0, RGBLED_NUM, HSV_GREEN});
const rgblight_segment_t PROGMEM _THIRD_[]  = RGBLIGHT_LAYER_SEGMENTS({0, RGBLED_NUM, HSV_RED});
const rgblight_segment_t PROGMEM _FOURTH_[] = RGBLIGHT_LAYER_SEGMENTS({0, RGBLED_NUM, HSV_WHITE});

const rgblight_segment_t* const PROGMEM _rgb_layers[] = RGBLIGHT_LAYERS_LIST(_FIRST_, _SECOND_, _THIRD_, _FOURTH_);

void keyboard_post_init_user(void) { rgblight_layers = _rgb_layers; }

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case 0:
            rgblight_blink_layer(0, 500);
            break;
        case 1:
            rgblight_blink_layer(1, 500);
            break;
        case 2:
            rgblight_blink_layer(2, 500);
            break;
        case 3:
            rgblight_blink_layer(3, 500);
            break;
    }
    return state;
}

uint8_t selected_layer = 0;
void    encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise && selected_layer < (LAYERNUM - 1)) {
            selected_layer++;
            layer_move(selected_layer);
        } else if (!clockwise && selected_layer > 0) {
            selected_layer--;
            layer_move(selected_layer);
        }
    } else if (index == 1) {
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
}
