#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <vector>

namespace Geometry {
    inline double normalizeAngle(double angle) {
        const double PI = 3.14159265358979323846;
        const double TWO_PI = 6.28318530717958647692;
        
        while (angle > PI) {
            angle -= TWO_PI;
        }
        while (angle < -PI) {
            angle += TWO_PI;
        }
        return angle;
    }
    
    inline double distance(double x1, double y1, double x2, double y2) {
        double dx = x2 - x1;
        double dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    inline std::pair<double, double> rotatePoint(double x, double y, double angle) {
        double cos_a = std::cos(angle);
        double sin_a = std::sin(angle);
        return {x * cos_a - y * sin_a, x * sin_a + y * cos_a};
    }
    
    struct LineSegment {
        double x1, y1, x2, y2;
        LineSegment(double x1, double y1, double x2, double y2) 
            : x1(x1), y1(y1), x2(x2), y2(y2) {}
    };
    
    bool rayIntersectsSegment(double ray_x, double ray_y, double ray_dir,
                             const LineSegment& segment, double& t, double& u);
    
    inline double getNormalAngle(double segment_dir) {
        return segment_dir + 1.5707963267948966; // π/2
    }
}

#endif
