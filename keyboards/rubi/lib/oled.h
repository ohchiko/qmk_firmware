#pragma once

enum rubi_layers { _BASE, _FN };

#ifdef OLED_FONT_H
#define OLED_FONT_H "./lib/glcdfont_rubi.c"
#endif

bool process_record_user_oled(uint16_t keycode, keyrecord_t *record);
