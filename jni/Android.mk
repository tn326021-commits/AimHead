LOCAL_PATH := $(call my-dir)/..

# ---- Dobby (pre-built static lib, fetched by GitHub Actions) ----
include $(CLEAR_VARS)
LOCAL_MODULE    := dobby
LOCAL_SRC_FILES := deps/dobby/libdobby_$(TARGET_ARCH_ABI).a
include $(PREBUILT_STATIC_LIBRARY)

# ---- Main patch module ----
include $(CLEAR_VARS)

LOCAL_MODULE     := libaimmod
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/deps/dobby/include

LOCAL_SRC_FILES  := src/main.cpp

LOCAL_STATIC_LIBRARIES := dobby

LOCAL_LDLIBS     := -llog -ldl -landroid

# ARM64 release flags
LOCAL_CPPFLAGS   := -std=c++17 \
                    -O2 \
                    -fvisibility=hidden \
                    -ffunction-sections \
                    -fdata-sections \
                    -DANDROID \
                    -DARM64

LOCAL_LDFLAGS    := -Wl,--gc-sections \
                    -Wl,--strip-all

include $(BUILD_SHARED_LIBRARY)
