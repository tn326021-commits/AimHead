#pragma once
#include <cmath>
#include "dat_reader.h"
struct Vec2 { float x, y; };
class DragEngine {
public:
    void configure(const AimConfig &cfg) { cfg_ = cfg; }
    Vec2 computeDrag(Vec2 center, Vec2 enemyScreen, Vec2 currentTouch) {
        Vec2 out{0,0};
        if(!cfg_.drag_enabled || !cfg_.valid) return out;
        float ex = enemyScreen.x - center.x;
        float ey = enemyScreen.y - center.y;
        float dist = std::sqrt(ex*ex + ey*ey);
        if(dist > cfg_.drag_radius) return out;
        if(dist < cfg_.drag_deadzone) return out;
        if(cfg_.drag_lock_head) ey -= cfg_.drag_vertical_bias*(cfg_.drag_radius*0.15f);
        float dx = enemyScreen.x - currentTouch.x;
        float dy = enemyScreen.y - currentTouch.y;
        float s = cfg_.drag_strength * (1.0f - cfg_.drag_smoothing);
        float sx = dx*s, sy = dy*s;
        float mag = std::sqrt(sx*sx+sy*sy);
        if(mag > cfg_.drag_max_speed){ float k=cfg_.drag_max_speed/mag; sx*=k; sy*=k; }
        sy -= cfg_.drag_vertical_bias * std::fabs(sy) * 0.15f;
        out.x = sx; out.y = sy;
        return out;
    }
private:
    AimConfig cfg_;
};
