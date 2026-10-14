#ifndef __LIGHT_MODE_H
#define __LIGHT_MODE_H

// 灯光控制模式枚举
typedef enum {
    MODE_AUTO = 0,   // 自动模式：根据人体存在和光照强度自动控制
    MODE_MANUAL = 1, // 手动模式：用户通过按键控制
    MODE_ONLINE = 2  // 在线模式：通过云平台控制
} LightMode_t;

#endif /* __LIGHT_MODE_H */ 