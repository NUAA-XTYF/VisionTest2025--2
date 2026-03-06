#ifndef DAMAGE_CALCULATOR_H
#define DAMAGE_CALCULATOR_H

#include "ammo_type.h"
#include <cmath>

class DamageCalculator {
public:
    static double calculateDamage(AmmoType ammo_type, double distance, 
                                 double base_damage = 100.0, 
                                 double distance_factor = 0.001) {
        if (distance < 0) return 0.0;
        
        double damage_multiplier = 1.0;
        switch (ammo_type) {
            case AmmoType::SMALL:
                damage_multiplier = 0.5;
                break;
            case AmmoType::LARGE:
                damage_multiplier = 1.0;
                break;
            default:
                damage_multiplier = 0.3;
        }
        
        double distance_penalty = std::exp(-distance_factor * distance);
        return base_damage * damage_multiplier * distance_penalty;
    }
    
    static double calculateDamage(AmmoType ammo_type, 
                                 double x1, double y1, double x2, double y2,
                                 double base_damage = 100.0,
                                 double distance_factor = 0.001) {
        double dx = x2 - x1;
        double dy = y2 - y1;
        double distance = std::sqrt(dx * dx + dy * dy);
        return calculateDamage(ammo_type, distance, base_damage, distance_factor);
    }
};

#endif
