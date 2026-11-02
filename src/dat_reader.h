#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

struct AimConfig {
    bool  aim_lock_head = false;
    int   sensitivity_general = 100, sensitivity_red_dot = 100;
    int   sensitivity_2x = 100, sensitivity_4x = 100, sensitivity_awm = 100;
    int   sensitivity_free_look = 100;
    float touch_offset_x = 0, touch_offset_y = 0;
    float fov = 90, smoothing = 0.3f, max_distance = 150;
    float predict_time = 0, head_offset = 1.7f, recoil_control = 0;
    int   trigger_delay_ms = 0, bone_id = 6, fps_limit = 60, graphics_quality = 2;
    bool  drag_enabled = false;
    float drag_strength = 0.5f, drag_smoothing = 0.4f, drag_radius = 180.0f;
    float drag_deadzone = 12.0f, drag_vertical_bias = 0.7f, drag_max_speed = 20.0f;
    bool  drag_lock_head = true;
    float drag_fov = 110.0f;
    int   drag_hold_ms = 40;
    uint8_t profile = 0;
    uint8_t flags = 0;
    bool valid = false;
};

class DatReader {
public:
    static bool load(const std::string &path, AimConfig &out) {
        FILE *f = fopen(path.c_str(), "rb");
        if (!f) return false;
        unsigned char h[32];
        if (fread(h, 1, 32, f) != 32) { fclose(f); return false; }
        if (memcmp(h, "AIMC", 4) != 0) { fclose(f); return false; }
        uint16_t ver; memcpy(&ver, h + 4, 2);
        if (ver != 3) { fclose(f); return false; }
        out.profile = h[6]; out.flags = h[7];
        uint32_t crc_exp, plen;
        memcpy(&crc_exp, h + 8, 4); memcpy(&plen, h + 12, 4);
        std::vector<unsigned char> p(plen);
        if (fread(p.data(), 1, plen, f) != plen) { fclose(f); return false; }
        fclose(f);
        if (crc32(p.data(), plen) != crc_exp) return false;
        size_t off = 0;
        uint16_t kc; memcpy(&kc, p.data()+off, 2); off+=2;
        std::vector<std::string> keys;
        for (uint16_t i=0;i<kc;i++){ uint16_t l; memcpy(&l,p.data()+off,2); off+=2;
            keys.emplace_back((char*)p.data()+off, l); off+=l; }
        uint16_t ec; memcpy(&ec, p.data()+off, 2); off+=2;
        std::unordered_map<std::string,double> v;
        for (uint16_t i=0;i<ec;i++){ uint16_t ki; memcpy(&ki,p.data()+off,2); off+=2;
            uint8_t t=p[off++]; unsigned char vb[8]={0}; memcpy(vb,p.data()+off,8); off+=8;
            double val;
            if(t==0){float x;memcpy(&x,vb,4);val=x;}
            else if(t==1){int32_t x;memcpy(&x,vb,4);val=x;}
            else if(t==2){val=vb[0]?1.0:0.0;}
            else{memcpy(&val,vb,8);}
            v[keys[ki]]=val; }
        auto g=[&](const char*k,double d){auto it=v.find(k);return it==v.end()?d:it->second;};
        out.aim_lock_head = g("aim_lock_head",0)!=0;
        out.sensitivity_general=(int)g("sensitivity_general",100);
        out.sensitivity_red_dot=(int)g("sensitivity_red_dot",100);
        out.sensitivity_2x=(int)g("sensitivity_2x",100);
        out.sensitivity_4x=(int)g("sensitivity_4x",100);
        out.sensitivity_awm=(int)g("sensitivity_awm",100);
        out.sensitivity_free_look=(int)g("sensitivity_free_look",100);
        out.touch_offset_x=(float)g("touch_offset_x",0);
        out.touch_offset_y=(float)g("touch_offset_y",0);
        out.fov=(float)g("fov",90);
        out.smoothing=(float)g("smoothing",0.3);
        out.max_distance=(float)g("max_distance",150);
        out.predict_time=(float)g("predict_time",0);
        out.head_offset=(float)g("head_offset",1.7);
        out.recoil_control=(float)g("recoil_control",0);
        out.trigger_delay_ms=(int)g("trigger_delay_ms",0);
        out.bone_id=(int)g("bone_id",6);
        out.fps_limit=(int)g("fps_limit",60);
        out.graphics_quality=(int)g("graphics_quality",2);
        out.drag_enabled=g("drag_enabled",0)!=0;
        out.drag_strength=(float)g("drag_strength",0.5);
        out.drag_smoothing=(float)g("drag_smoothing",0.4);
        out.drag_radius=(float)g("drag_radius",180);
        out.drag_deadzone=(float)g("drag_deadzone",12);
        out.drag_vertical_bias=(float)g("drag_vertical_bias",0.7);
        out.drag_max_speed=(float)g("drag_max_speed",20);
        out.drag_lock_head=g("drag_lock_head",1)!=0;
        out.drag_fov=(float)g("drag_fov",110);
        out.drag_hold_ms=(int)g("drag_hold_ms",40);
        out.valid = true;
        return true;
    }
private:
    static uint32_t crc32(const unsigned char *d, size_t n) {
        uint32_t c=0xFFFFFFFF;
        for(size_t i=0;i<n;i++){ c^=d[i]; for(int j=0;j<8;j++) c=(c>>1)^(0xEDB88320&-(c&1)); }
        return ~c;
    }
};
