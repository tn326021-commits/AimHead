#include <jni.h>
#include <android/log.h>
#include <cstdio>
#include "dat_reader.h"
#include "drag_engine.h"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "AimLockHead", __VA_ARGS__)
static AimConfig g_cfg;
static DragEngine g_drag;
__attribute__((constructor))
static void on_load() {
    const char *paths[] = {
        "/data/local/tmp/aimlockhead.dat",
        "/sdcard/Android/data/com.dts.freefireth/files/aimlockhead.dat",
        "/data/local/tmp/configs/aimlockhead.dat",
        nullptr
    };
    for(int i=0; paths[i]; i++){
        if(DatReader::load(paths[i], g_cfg)){
            g_drag.configure(g_cfg);
            LOGI("Loaded %s profile=%u", paths[i], g_cfg.profile);
            LOGI("  drag_enabled=%d strength=%.2f smooth=%.2f radius=%.0f",
                 g_cfg.drag_enabled, g_cfg.drag_strength, g_cfg.drag_smoothing, g_cfg.drag_radius);
            return;
        }
    }
    LOGI("aimlockhead.dat not found");
}
