// Copyright 2022 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H  

enum custom_keycodes {
    // ALT_TAB = SAFE_RANGE,
    WIN_POS = SAFE_RANGE,
    LED_LEVEL,
};

#define WIN_POS G(A(KC_SPACE))

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max)
{
    switch (get_highest_layer(layer_state))
    {
    default:
        rgb_matrix_set_color_all(0, 0, 0);
    }
    return false;
};
#endif

#define SCR_LFT G(S(KC_LEFT))
#define SCR_RHT G(S(KC_RGHT))
#define VD_LEFT G(C(KC_LEFT))
#define VD_RGHT G(C(KC_RIGHT))
#define OS_LOCK G(KC_L)
#define TSK_VIW G(KC_TAB)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( //default layer
        KC_NO,   KC_ESC,  KC_F5,   KC_NO,
        WIN_POS, TSK_VIW, G(KC_X), KC_LGUI,
        KC_F13,  SCR_LFT, SCR_RHT, KC_MPLY,
        TO(3),   KC_F15,  KC_MUTE, TO(1)
    ),
    [1] = LAYOUT( //utility layer
        _______, KC_SLEP, OS_LOCK, _______,
        WIN_POS, KC_PGDN, KC_PGUP, G(KC_X),
        VD_LEFT, SCR_LFT, SCR_RHT, VD_RGHT,
        TO(0),   KC_PGDN, KC_PGUP, TO(2)
    ),
    [2] = LAYOUT( //media layer
        _______, _______, _______, _______,
        _______, KC_VOLU, KC_VOLD, _______,
        KC_MSTP, KC_MPRV, KC_MNXT, KC_MPLY,
        TO(1),   KC_F15,  KC_MUTE, TO(3)
    ),
    [3] = LAYOUT( //utility layer
        _______, RM_TOGG, _______, _______,
        _______, _______, _______, _______,
        _______, _______, QK_RBT,  _______,
        TO(2),   EE_CLR,  QK_BOOT, TO(0)
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    { ENCODER_CCW_CW(KC_WFWD, KC_WBAK),           ENCODER_CCW_CW(KC_WH_L, KC_WH_R)  },
    { ENCODER_CCW_CW(_______, _______),           ENCODER_CCW_CW(_______, _______)  },
    { ENCODER_CCW_CW(_______, _______),           ENCODER_CCW_CW(_______, _______)  },
    { ENCODER_CCW_CW(_______, _______),           ENCODER_CCW_CW(_______, _______)  }
};
#endif


typedef union {
    uint32_t raw;
    struct {
        uint8_t led_level : 3;
    };
} work_louder_config_t;

work_louder_config_t work_louder_config;

#define WL_LED_MAX_BRIGHT 75

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LED_LEVEL:
            if (record->event.pressed) {
                work_louder_config.led_level++;
                if (work_louder_config.led_level > 4) {
                    work_louder_config.led_level = 0;
                }
                work_louder_micro_led_all_set((uint8_t)(work_louder_config.led_level * 255 / 4));
                eeconfig_update_user(work_louder_config.raw);
                layer_state_set_kb(layer_state);
            }
            break;
    }
    return true;
}


layer_state_t layer_state_set_user(layer_state_t state) {
    layer_state_cmp(state, 1) ? work_louder_micro_led_1_on(): work_louder_micro_led_1_off();
    layer_state_cmp(state, 2) ? work_louder_micro_led_2_on(): work_louder_micro_led_2_off();
    layer_state_cmp(state, 3) ? work_louder_micro_led_3_on(): work_louder_micro_led_3_off();

    return state;
}

void eeconfig_init_user(void) {
    work_louder_config.raw = 0;
    work_louder_config.led_level = 1;
    work_louder_micro_led_all_set((uint8_t)(work_louder_config.led_level * WL_LED_MAX_BRIGHT / 4));
    eeconfig_update_user(work_louder_config.raw);
}

void matrix_init_user(void) {
    work_louder_config.raw = eeconfig_read_user();
    work_louder_micro_led_all_set((uint8_t)(work_louder_config.led_level * 255 / 4));
}

// RGB time out
static uint32_t key_timer;           // timer to track the last keyboard activity, use 32bit value and function to make longer idle time possible
static void refresh_rgb(void);       // refreshes the activity timer and RGB, invoke whenever activity happens
static void check_rgb_timeout(void); // checks if enough time has passed for RGB to timeout
bool is_rgb_timeout = false;         // store if RGB has timed out or not in a boolean

void refresh_rgb(void)
{
    key_timer = timer_read32(); // store time of last refresh
    if (is_rgb_timeout)

    {
        is_rgb_timeout = false;
        rgblight_wakeup();
    }
}
void check_rgb_timeout(void)
{
    if (!is_rgb_timeout && timer_elapsed32(key_timer) > RGBLIGHT_TIMEOUT)
    {
        rgblight_suspend();
        is_rgb_timeout = true;
    }
}
/* Then, call the above functions from QMK's built in post processing functions like so */
/* Runs at the end of each scan loop, check if RGB timeout has occured */
void housekeeping_task_user(void)
{
#ifdef RGBLIGHT_TIMEOUT
    check_rgb_timeout();
#endif
}
/* Runs after each key press, check if activity occurred */
void post_process_record_user(uint16_t keycode, keyrecord_t *record)
{
#ifdef RGBLIGHT_TIMEOUT
    if (record->event.pressed)
        refresh_rgb();
#endif
}
/* Runs after each encoder tick, check if activity occurred */
void post_encoder_update_user(uint8_t index, bool clockwise)
{
#ifdef RGBLIGHT_TIMEOUT
    refresh_rgb();
#endif
}
