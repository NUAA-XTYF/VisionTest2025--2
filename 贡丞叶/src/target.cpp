#include "target.h"

Target::Target(int id, double x, double y, double direction, double width)
    : Object(id, x, y, direction, false), width_(width) {}

Target::~Target() {}
