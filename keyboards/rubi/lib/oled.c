#include QMK_KEYBOARD_H
#include "./lib/oled.h"

/* const char *read_logo(void); */
/* extern const unsigned char font[] PROGMEM; */

#define OLED_ROW_1 OLED_DISPLAY_WIDTH
#define OLED_ROW_2 (OLED_DISPLAY_WIDTH * 2)

static uint32_t oled_sleep_timer = 0;
static uint16_t oled_frame_timer = 0;
static uint16_t oled_logo_timer  = 0;
static bool     oled_logo_expired = false;

#define OLED_FRAME_TIMEOUT (1000 / 40)  // 40 fps
#define OLED_SLEEP_TIMEOUT 30000        // 30 sec
#define OLED_LOGO_TIMEOUT 3000          // 3 sec

static const char PROGMEM rubi_logo[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   128, 128, 128, 128, 128, 128, 0,   0,   0, 0, 0, 0, 0, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0,  0,   0,   0, 0, 0, 128, 128, 128, 128, 0,   0,   0,   0,   0,   0,   128, 128, 128, 128, 0, 0, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 0,   0,   0,  0, 0, 128, 128, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 224, 112, 184, 220, 204, 182, 123, 254, 251, 187, 223, 231, 203, 126, 254, 0, 0, 0, 0, 0, 0, 255, 0, 0, 0, 248, 8, 8, 8, 8, 8, 249, 225, 2, 12, 248, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 0, 0, 255, 255, 255, 255, 199, 199, 199, 199, 199, 207, 255, 255, 254, 124, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 191, 188, 211, 223, 223, 247, 187, 187, 252, 126, 123, 247, 239, 223, 190, 115, 142, 0, 0, 0, 0, 0, 255, 0,   0,   0,   227, 34,  34,  34,  98,  194, 1,   56, 236, 135, 1, 0, 0, 31,  63,  127, 255, 248, 240, 240, 240, 240, 248, 255, 127, 63,  31,  0, 0, 255, 255, 255, 255, 227, 227, 227, 227, 227, 243, 255, 255, 127, 62, 0, 0, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   0,   0,   0,   1,   1,   0, 0, 0, 0, 0, 0, 1,   1, 1, 1, 1,   0, 0, 0, 0, 1, 1,   1,   1, 1,  0,   0, 0, 0,   0,   0,   0,   0, 1, 1, 1, 1, 0, 0,   0,   0,   0,   0, 0, 1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0, 0, 1,   1,   1,   1,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

//static const char PROGMEM code_to_name[0xFF] = {
//    //   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
//    ' ', ' ',  ' ', ' ', 'A',  'B',  'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',  // 0x
//    'M', 'N',  'O', 'P', 'Q',  'R',  'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2',  // 1x
//    '3', '4',  '5', '6', '7',  '8',  '9', '0', 20,  19,  27,  26,  22,  '-', '=', '[',  // 2x
//    ']', '\\', '#', ';', '\'', '`',  ',', '.', '/', 128, 7,   7,   7,   7,   7,   7,    // 3x
//    7,   7,    7,   7,   7,    7,    'P', 'S', 'P', 'I', 'H', 30,  16,  31,  17,  27,   // 4x
//    26,  25,   24,  ' ', ' ',  '/',  '*', '-', '+', 20,  '1', '+', '3', '4', '5', '6',  // 5x
//    '7', '8',  '9', '0', '.',  '\\', 'A', 'P', '=', 7,   7,   7,   7,   7,   7,   7,    // 6x
//    7,   7,    7,   7,   'X',  'H',  'M', 'S', 'S', 'A', 'U', 'C', 'C', 'P', 'F', 'M',  // 7x
//    ' ', ' ',  ' ', ' ', ' ',  ',',  '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 8x
//    ' ', ' ',  ' ', ' ', ' ',  ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 9x
//    ' ', ' ',  ' ', ' ', ' ',  ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Ax
//    ' ', ' ',  ' ', ' ', ' ',  ' ',  '(', ')', '{', '}', ' ', ' ', 'A', 'B', 'C', 'D',  // Bx
//    'E', 'F',  'X', '^', '%',  '<',  '>', '&', '&', '|', '|', ':', '#', ' ', '@', '!',  // Cx
//    ' ', ' ',  ' ', ' ', ' ',  ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Dx
//    'C', 'S',  'A', 'C', ' ',  ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Ex
//    ' ', ' ',  ' ', ' ', ' ',  ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '        // Fx
//};

bool process_record_user_oled(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        oled_sleep_timer = timer_read32();
    }

    return true;
}

uint8_t render_layer_state(void) {
    uint8_t len = 0;
    oled_write_P(PSTR("LAYER: "), false);
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            /* oled_write_P(PSTR("BASE"), false); */
            oled_write_P(PSTR("BASE"), false);
            len = 4;
            break;
        case _FN:
            oled_write_P(PSTR("FUNC"), false);
            len = 4;
            break;
    }

    return len;
}

void render_lock_status(void) {
    led_t led_state = host_keyboard_led_state();
    static const char PROGMEM numlock[] = {
                                           0x80, 0x81, 0x82, 0x83, 0x0A, 0xA0, 0xA1, 0xA2, 0xA3

    };
    oled_write_P(led_state.num_lock ? numlock : PSTR("    "), false);
}

void render_status(void) {
    uint8_t len = render_layer_state();
    uint8_t t   = 6 - len;
    if (t > len) {
        t = 0;
    }
    len = t + 4;

    for (; len; len--) {
        oled_write_char(' ', false);
    }

    render_lock_status();
}

void render_frame(void) {
    if (oled_logo_expired) {
        render_status();
    } else {
        oled_write_raw_P(rubi_logo, sizeof(rubi_logo));
        oled_logo_expired = timer_elapsed(oled_logo_timer) > OLED_LOGO_TIMEOUT;
        if (oled_logo_expired) {
            oled_clear();
        }
    }
}

void oled_task_user(void) {
    // sleep if it has been long enough since last key is pressed
    if (timer_elapsed32(oled_sleep_timer) > OLED_SLEEP_TIMEOUT) {
        oled_off();
        return;
    } else {
        oled_on();
    }

    // is this time for the next frame?
    if (timer_elapsed(oled_frame_timer) > OLED_FRAME_TIMEOUT) {
        render_frame();
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    oled_frame_timer = timer_read();
    return rotation;
}
