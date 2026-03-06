#include "geometry.h"

bool Geometry::rayIntersectsSegment(double ray_x, double ray_y, double ray_dir,
                                   const LineSegment& segment, double& t, double& u) {
    double rx1 = ray_x;
    double ry1 = ray_y;
    double rx2 = ray_x + std::cos(ray_dir);
    double ry2 = ray_y + std::sin(ray_dir);
    
    double dx1 = rx2 - rx1;
    double dy1 = ry2 - ry1;
    double dx2 = segment.x2 - segment.x1;
    double dy2 = segment.y2 - segment.y1;
    
    double denominator = dx1 * dy2 - dy1 * dx2;
    
    if (std::abs(denominator) < 1e-10) {
        return false;
    }
    
    double t_num = (segment.x1 - rx1) * dy2 - (segment.y1 - ry1) * dx2;
    double u_num = (segment.x1 - rx1) * dy1 - (segment.y1 - ry1) * dx1;
    
    t = t_num / denominator;
    u = u_num / denominator;
    
    return (t >= 0 && u >= 0 && u <= 1);
}
