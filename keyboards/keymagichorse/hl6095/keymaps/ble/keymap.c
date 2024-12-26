/* Copyright 2024 keymagichorse
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
#include "config.h"
#include "bhq.h"
#include "outputselect.h"
#include "usb_main.h"

#if defined(RGBLIGHT_WS2812)
#    include "ws2812.h"
#endif

#if defined(BLUETOOTH_BHQ)
#   include "bhq.h"
#   include "battery.h"
#endif

#   if defined(KB_LPM_ENABLED)
#   include "lpm.h"
#endif

// 键盘传输模式枚举
enum  kb_transfer_mode_enum {
    KB_USB_MODE = 1,
    KB_BLE_1_MODE,
    KB_BLE_2_MODE,
    KB_BLE_3_MODE,
    KB_RF_MODE,
};

typedef union {
  uint32_t raw;
  struct {
     uint8_t transfer_mode :8;  // 保存 键盘传输模式
  };
} user_config_t;
user_config_t user_config = {0};

enum keyboard_user_keycodes {
    BT_1 = QK_USER,
    BT_2,
    BT_3,
    BT_4,
    BT_5,
    BT_6,
    BT_7,
    BT_8,
    BT_9,
    BT_10,
    BT_11,
};
#define BLE_TOG     BT_1    // 切换蓝牙输出 并 开启蓝牙广播（非配对类型）
#define RF_TOG      BT_2    // 切换 2.4ghz输出
#define USB_TOG     BT_3    // 打开USB
#define BL_SW_0     BT_4    // 开启蓝牙通道0（需要打开蓝牙的条件下才行） 短按打开广播 长按开启配对广播
#define BL_SW_1     BT_5    // 开启蓝牙通道1（需要打开蓝牙的条件下才行） 短按打开广播 长按开启配对广播
#define BL_SW_2     BT_6    // 开启蓝牙通道2（需要打开蓝牙的条件下才行） 短按打开广播 长按开启配对广播
// #define BLE_DEL     BT_7    // 删除当前蓝牙绑定
// #define BLE_CLR     BT_8    // 清空所有蓝牙绑定
#define BLE_OFF     BT_9    // 关闭蓝牙连接


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_all(
    QK_GESC, KC_1,    KC_2,     KC_3,     KC_4,    KC_5,    KC_6,    KC_7,    KC_8,      KC_9,     KC_0,     KC_MINS,  KC_EQL,  KC_BSLS, KC_BSPC,
    KC_TAB,  BL_SW_0, BL_SW_1,  RF_TOG,     KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,      KC_O,     KC_P,     KC_LBRC,  KC_RBRC, KC_BSLS,
    KC_CAPS, BLE_TOG, USB_TOG,  NK_TOGG,     KC_F,    KC_G,    KC_H,    KC_J,    KC_K,      KC_L,     KC_SCLN,  KC_QUOT,  KC_BSLS, KC_ENT,
    KC_LSFT, KC_Z,    KC_X,     KC_C,     KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,   KC_DOT,   KC_SLSH,  KC_RSFT,  KC_UP,   KC_DEL,
    MS_DOWN, KC_VOLU, KC_LALT,  KC_SPC,   KC_SPC,  KC_SPC,                    KC_SPC,    KC_RALT,  MO(1),    KC_LEFT,  KC_DOWN, KC_RIGHT),
  [1] = LAYOUT_all(
    KC_GRV , KC_F1,   KC_F2,   KC_F3,    KC_F4,   KC_F5,   KC_F6,   KC_F7,    KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_TRNS, KC_DEL,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, RGB_TOG, RGB_MOD, RGB_RMOD, RGB_VAI, RGB_VAD, KC_TRNS, CG_TOGG, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, GU_TOGG,   KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,                    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
  [2] = LAYOUT_all(
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,                    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
  [3] = LAYOUT_all(
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,                    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS)
};


void eeconfig_init_kb(void)
{
    // 上电先读取一次
    user_config.raw = eeconfig_read_user();
    // 判断传输模式有没有被初始化过
    if(user_config.transfer_mode == 0 || user_config.transfer_mode == 0xff)
    {
        // 表示没有被初始化过
        user_config.transfer_mode = KB_USB_MODE;    // 默认先初始化为USB
        set_output(OUTPUT_USB);
        return;
    }
    
    if(user_config.transfer_mode == KB_USB_MODE)
    {
        set_output(OUTPUT_USB);
    }

    if(user_config.transfer_mode > KB_USB_MODE && user_config.transfer_mode != 0xff)
    {
        set_output(OUTPUT_BLUETOOTH);
    }

}



// Keyboard level code can override this, but shouldn't need to.
// Controlling custom features should be done by overriding
// via_custom_value_command_kb() instead.
__attribute__((weak)) bool via_command_kb(uint8_t *data, uint8_t length) {
    uint8_t command_id   = data[0];
    uint8_t i = 0;

    // 此逻辑删除 会失去蓝牙模块升级功能 以及蓝牙改键功能！！！！！！！
    bhq_printf("cmdid:%02x  length:%d\r\n",command_id,length);
    bhq_printf("read host app of data \r\n[");
    for (i = 0; i < length; i++)
    {
        bhq_printf("%02x ",data[i]);
    }
    bhq_printf("]\r\n");

    if(command_id == 0xF1)
    {
        // cmdid + 2 frame headers 
        // The third one is isack the fourth one is length and the fifth one is data frame
        BHQ_SendCmd(0, &data[4], data[3]);
        return true;
    }
    return false;
}

#   if defined(KB_LPM_ENABLED)
// 低功耗外围设备电源控制
void lpm_device_power_open(void) 
{
#if defined(RGBLIGHT_WS2812) && defined(RGBLIGHT_ENABLE) 
    // ws2812电源开启
    ws2812_init();
    rgblight_setrgb_at(255, 60, 50, 0);
    gpio_set_pin_output(B8);        // ws2812 power
    gpio_write_pin_low(B8);
#endif

}
void lpm_device_power_close(void) 
{
#if defined(RGBLIGHT_WS2812) && defined(RGBLIGHT_ENABLE) 
    // ws2812电源关闭
    rgblight_setrgb_at(0, 0, 0, 0);
    gpio_set_pin_output(B8);        // ws2812 power
    gpio_write_pin_high(B8);

    gpio_set_pin_output(WS2812_DI_PIN);        // ws2812 DI Pin
    gpio_write_pin_low(WS2812_DI_PIN);
#endif
}
#endif

// --------------------  都是用于处理按键触发的变量 --------------------
// 这几个变量大致的功能就是用来 作长按短按的。
// 长按打开配对蓝牙广播  短按打开非配对蓝牙广播
static uint32_t key_output_mode_press_time = 0; // 输出模式按下计时器
static uint32_t key_ble_switch_press_time = 0;  // 蓝牙切换通道 按下计时器
static uint8_t key_ble_host_index = 0;          // 这里是用于按键按下的
// --------------------  都是用于处理按键触发的变量 --------------------


// --------------------  蓝牙模块返回的状态 --------------------
uint8_t advertSta = 0;      // 蓝牙广播状态
uint8_t connectSta = 0;     // 连接状态
uint8_t pairingSta = 0;     // 蓝牙是否开启配对广播
uint8_t host_index = 255;   // 蓝牙通道 这里是模块返回的
// --------------------  蓝牙模块返回的状态 --------------------

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#   if defined(KB_LPM_ENABLED)
    lpm_timer_reset();  // 这里用于低功耗，按下任何按键刷新低功耗计时器
#endif

    // 如果蓝牙广播未开启 或 没有连接
    if (advertSta == 0 && connectSta == 0) 
    {
        // 检查按键值是否不在 BT_1 到 BT_11 范围
        if (keycode < BT_1 || keycode > BT_11) 
        {
            // 检查传输模式是否为蓝牙模式
            if (
                user_config.transfer_mode == KB_BLE_1_MODE || 
                user_config.transfer_mode == KB_BLE_2_MODE || 
                user_config.transfer_mode == KB_BLE_3_MODE
            ) 
            {
                // KB_BLE_1_MODE 在枚举 里面是2、在蓝牙通道内是0
                // 那么 2 - 2 = 0 那就是host = 0;
                // 重新打开非配对蓝牙广播。如已开启蓝牙广播或已连接，那么不会断开当前的蓝牙连接。
                bhq_AnewOpenBleAdvertising(user_config.transfer_mode - 2, 15);
            }
        }
    }


    switch (keycode)
    {
        case BLE_TOG:
        {
            if(record->event.pressed)
            {
                key_output_mode_press_time = timer_read32();
            }
            else 
            {
                // gpio_write_pin_high(QMK_RUN_OUTPUT_PIN);
                if(timer_elapsed32(key_output_mode_press_time) >= 300) 
                {
                    key_ble_host_index = 0;
                    // 打开非配对模式蓝牙广播 10 = 10S
                    bhq_OpenBleAdvertising(key_ble_host_index, 30);
                    set_output(OUTPUT_BLUETOOTH);

                    // 这里切换蓝牙模式，默认是打开第一个蓝牙通道
                    user_config.transfer_mode = KB_BLE_1_MODE;  
                    eeconfig_update_user(user_config.raw);
                }
            }
            return false;
        }
        case RF_TOG:
        {
            if(record->event.pressed)
            {
                key_output_mode_press_time = timer_read32();
            }
            else 
            {
                // gpio_write_pin_high(QMK_RUN_OUTPUT_PIN);
                if(timer_elapsed32(key_output_mode_press_time) >= 300) 
                {
                    // TODO: 等待bhq驱动完善，这里还是用蓝牙输出来作为qmk的模式切换，在蓝牙模块内会切换成2.4ghz私有连接
                    bhq_switch_rf_easy_kb();
                    set_output(OUTPUT_BLUETOOTH);
                
                    user_config.transfer_mode = KB_RF_MODE;  
                    eeconfig_update_user(user_config.raw);
                }
            }
            return false;
        }
        case USB_TOG:
        {
            if(record->event.pressed)
            {
                key_output_mode_press_time = timer_read32();
            }
            else
            {
                if(timer_elapsed32(key_output_mode_press_time) >= 300) 
                {
                    // 切换到usb模式 并 关闭蓝牙广播
                    set_output(OUTPUT_USB);
                    bhq_CloseBleAdvertising();

                    user_config.transfer_mode = KB_USB_MODE;  
                    eeconfig_update_user(user_config.raw);
                }
            }
            return false;
        }
        case BLE_OFF:
        {
            if(record->event.pressed)
            {
                key_output_mode_press_time = timer_read32();
            }
            else
            {
                if(timer_elapsed32(key_output_mode_press_time) >= 500) 
                {
                    // 关闭蓝牙广播
                    bhq_CloseBleAdvertising();
                }
            }
            return false;
        }
        case BL_SW_0:
        case BL_SW_1:
        case BL_SW_2:
            if(record->event.pressed)
            {
                key_ble_switch_press_time = timer_read32();
            }
            else
            {
                switch (keycode)
                {
                    case BL_SW_0:
                        key_ble_host_index = 0;
                        break;  
                    case BL_SW_1:
                        key_ble_host_index = 1;
                        break;  
                    case BL_SW_2:
                        key_ble_host_index = 2;
                        break;  
                }
                if(timer_elapsed32(key_ble_switch_press_time) >= 300 && timer_elapsed32(key_ble_switch_press_time) <= 800)
                {
                    // 打开非配对模式蓝牙广播 10 = 10S
                    bhq_OpenBleAdvertising(key_ble_host_index, 30);
                    set_output(OUTPUT_BLUETOOTH);

                    // 这里枚举 + 蓝牙通道就能计算出 KB_BLE_1_MODE、KB_BLE_2_MODE、KB_BLE_3_MODE
                    user_config.transfer_mode = KB_BLE_1_MODE + key_ble_host_index;  
                    eeconfig_update_user(user_config.raw);
                }
                else if(timer_elapsed32(key_ble_switch_press_time) >= 1000)
                {
                    // 打开 配对模式蓝牙广播 10 = 10S
                    bhq_SetPairingMode(key_ble_host_index, 30);
                    set_output(OUTPUT_BLUETOOTH);
                    
                    // 这里枚举 + 蓝牙通道就能计算出 KB_BLE_1_MODE、KB_BLE_2_MODE、KB_BLE_3_MODE
                    user_config.transfer_mode = KB_BLE_1_MODE + key_ble_host_index;  
                    eeconfig_update_user(user_config.raw);
                }
            }
            return false;
    }
    return true;
}

#if defined(RGBLIGHT_ENABLE) 
//  每个通道的颜色 以及大写按键的颜色
const rgblight_segment_t PROGMEM bt_conn1[] = RGBLIGHT_LAYER_SEGMENTS( {0, 1, HSV_RED} );   // 红
const rgblight_segment_t PROGMEM bt_conn2[] = RGBLIGHT_LAYER_SEGMENTS( {0, 1, HSV_GREEN} ); // 绿
const rgblight_segment_t PROGMEM bt_conn3[] = RGBLIGHT_LAYER_SEGMENTS( {0, 1, HSV_BLUE} );  // 蓝
const rgblight_segment_t PROGMEM caps_lock_[] = RGBLIGHT_LAYER_SEGMENTS( {0, 1, HSV_PURPLE} );  // 紫色

const rgblight_segment_t* const PROGMEM _rgb_layers[] = RGBLIGHT_LAYERS_LIST( 
    bt_conn1, bt_conn2, bt_conn3, caps_lock_
);



void rgb_adv_unblink_all_layer(void) {
    for (uint8_t i = 0; i < 3; i++) {
        rgblight_unblink_layer(i);
    }
}

//  这个qmk的基本功能。键盘锁更新就会触发这玩意
bool led_update_user(led_t led_state) {
    rgblight_set_layer_state(3, led_state.caps_lock);
    bhq_printf("led_update_user\r\n");
    return true;
}

#endif

// After initializing the peripheral
void keyboard_post_init_kb(void)
{
#if defined(RGBLIGHT_ENABLE) 
    rgblight_disable();
    rgblight_layers = _rgb_layers;
#endif
#if defined(BLUETOOTH_BHQ)
    bhkDevConfigInfo_t model_parma = {
        .vendor_id_source   = 1,
        .verndor_id         = VENDOR_ID,
        .product_id         = PRODUCT_ID,

        .le_connection_interval_min = 10,   // 6 10 上报速度过快更丝滑 功耗更大
        .le_connection_interval_max = 30,
        .le_connection_interval_timeout = 500,
        .tx_poweer = 0x3D,    

#if BHQ_READ_VOLTAGE_ENABLED == TRUE
        .mk_is_read_battery_voltage = TRUE,
        .mk_adc_pga = 1,
        .mk_rvd_r1 = BHQ_R_UPPER,
        .mk_rvd_r2 = BHQ_R_LOWER,
#else
        .mk_is_read_battery_voltage = FALSE,
        .mk_adc_pga = 1,
        .mk_rvd_r1 = 0,
        .mk_rvd_r2 = 0,
#endif
        .sleep_1_s = 30,            // 一级休眠功耗 （蓝牙保持连接 唤醒后发送按键有一定的延时）
        .sleep_2_s = 300,           // 二级休眠功耗（相当于关机模式 蓝牙会断开）

        .bleNameStrLength = strlen(PRODUCT),
        .bleNameStr = PRODUCT
    };
    bhq_ConfigRunParam(model_parma);    // 将配置信息发送到无线模块中
#endif
}

#if defined(BLUETOOTH_BHQ)
// BHQ Status callback   BHQ状态回调函数
void BHQ_State_Call(uint8_t cmdid, uint8_t *dat) 
{

    advertSta = BHQ_GET_BLE_ADVERT_STA(dat[1]);
    connectSta = BHQ_GET_BLE_CONNECT_STA(dat[1]);
    pairingSta = BHQ_GET_BLE_PAIRING_STA(dat[1]);
#if defined(RGBLIGHT_ENABLE) 
    rgblight_disable();
    rgb_adv_unblink_all_layer();

    bhq_printf("keymape:cmdid:%d\r\n",cmdid);
    if(cmdid == BHQ_ACK_RUN_STA_CMDID)
    {
        bhq_printf("[RSSI:%d]\t",dat[0]);
        bhq_printf("[advertSta: %d]\t", advertSta);
        bhq_printf("[connectSta: %d]\t", connectSta); // 0 = 断开, 1 = 已连接, 2 = 超时
        bhq_printf("[pairingSta: %d]\t", pairingSta);
        bhq_printf("[host_index:%d]\n",dat[2]);
        host_index = dat[2];

        if(host_index == 0)
        {
            bhq_printf("if host_index 1\r\n");
            // 蓝牙没有连接 && 蓝牙广播开启  && 蓝牙配对模式
            if(connectSta != 1 && advertSta == 1 && pairingSta == 1)
            {
                rgblight_blink_layer_repeat(0 , 500, 50);
                bhq_printf("1\r\n");
            }
            // 蓝牙没有连接 && 蓝牙广播开启  && 蓝牙非配对模式
            else if(connectSta != 1 && advertSta == 1 && pairingSta == 0)
            {
                rgblight_blink_layer_repeat(0 , 2000, 50);
                bhq_printf("2\r\n");
            }
            // 蓝牙已连接
            if(connectSta == 1)
            {
                rgblight_blink_layer_repeat(0 , 200, 2);
                bhq_printf("3\r\n");
            }
        }
        // 注释同上的host_index == 0 
        else if(host_index == 1)
        {
            bhq_printf("if host_index 2\r\n");
            if(connectSta != 1 && advertSta == 1 && pairingSta == 1)
            {
                rgblight_blink_layer_repeat(1 , 500, 50);
                bhq_printf("4\r\n");
            }
            else if(connectSta != 1 && advertSta == 1 && pairingSta == 0)
            {
                rgblight_blink_layer_repeat(1 , 2000, 50);
                bhq_printf("5\r\n");
            }
            if(connectSta == 1)
            {
                rgblight_blink_layer_repeat(1 , 200, 2);
                bhq_printf("6\r\n");
            }
        }    
        else if(host_index == 2)
        {
            bhq_printf("if host_index 3\r\n");
            if(connectSta != 1 && advertSta == 1 && pairingSta == 1)
            {
                rgblight_blink_layer_repeat(2 , 500, 50);
            }
            else if(connectSta != 1 && advertSta == 1 && pairingSta == 0)
            {
                rgblight_blink_layer_repeat(2 , 2000, 50);
            }
            if(connectSta == 1)
            {
                rgblight_blink_layer_repeat(2 , 200, 2);
            }
        }
        
    }
    else if(cmdid == BHQ_ACK_LED_LOCK_CMDID)
    {
        // 在蓝牙驱动层内已经处理好 键盘锁了，这边无需在做处理 只是输出一下罢了
        bhq_printf("[%s] Num Lock\t", (dat[0] & (1<<0)) ? "*" : " ");
        bhq_printf("[%s] Caps Lock\t", (dat[0] & (1<<1)) ? "*" : " ");
        bhq_printf("[%s] Scroll Lock\n", (dat[0] & (1<<2)) ? "*" : " ");
    }
#endif
}
#endif
