// VisionTest2.cpp: 定义应用程序的入口点。
#include "VisionTest2.h"

// 定义静态成员（必须在单独的翻译单元中定义）
int Object::nextId = 1;
int SentryRobot::totalTargetsDetected = 0;
int SentryRobot::instanceCount = 0;

// ---------------------- 实现部分 ----------------------

// EnumUtils 实现
namespace EnumUtils {
    std::string objectColorToString(ObjectColor color) {
        static const std::map<ObjectColor, std::string> colorMap = {
            {ObjectColor::RED, "Red"},
            {ObjectColor::BLUE, "Blue"},
            {ObjectColor::UNKNOWN, "Unknown"}
        };
        return colorMap.count(color) ? colorMap.at(color) : "Unknown";
    }

    ObjectColor stringToObjectColor(const std::string& str) {
        static const std::map<std::string, ObjectColor> reverseMap = {
            {"red", ObjectColor::RED},
            {"blue", ObjectColor::BLUE},
        };

        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);

        if (reverseMap.count(lowerStr)) {
            return reverseMap.at(lowerStr);
        }
        return ObjectColor::UNKNOWN;
    }

    std::string armorSideToString(ArmorSide side) {
        static const char* sideNames[] = { "Front", "Back", "Left", "Right", "Top", "Bottom" };
        return sideNames[static_cast<int>(side)];
    }

    ArmorSide stringToArmorSide(const std::string& s) {
        std::string lower = s;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "front") return ArmorSide::FRONT;
        if (lower == "back") return ArmorSide::BACK;
        if (lower == "left") return ArmorSide::LEFT;
        if (lower == "right") return ArmorSide::RIGHT;
        if (lower == "top") return ArmorSide::TOP;
        if (lower == "bottom") return ArmorSide::BOTTOM;
        return ArmorSide::FRONT;
    }

    std::string ammoTypeToString(AmmoType type) {
        static const char* typeNames[] = { "Large" , "Small" , "Unknown" };
        return typeNames[static_cast<int>(type)];
    }

    AmmoType stringToAmmoType(const std::string& str) {
        static const std::map<std::string, AmmoType> ammoMap = {
            {"large", AmmoType::LARGE}, {"small", AmmoType::SMALL}
        };
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        if (ammoMap.count(lowerStr)) return ammoMap.at(lowerStr);
        return AmmoType::UNKNOWN;
    }
}

// 几何工具实现
Vec2 rotateLocalToWorld(const Vec2& local, double hostDir) {
    double c = std::cos(hostDir);
    double s = std::sin(hostDir);
    return Vec2(local.x * c - local.y * s, local.x * s + local.y * c);
}

double normalizeAngle(double ang) {
    const double PI = 3.14159265358979323846;
    while (ang >= PI) ang -= 2*PI;
    while (ang < -PI) ang += 2*PI;
    return ang;
}

double distance(const Vec2& a, const Vec2& b) {
    return std::hypot(b.x - a.x, b.y - a.y);
}

double angleTo(const Vec2& from, const Vec2& to) {
    return normalizeAngle(std::atan2(to.x - from.x, to.y - from.y));
}

Vec2 rotatePoint(const Vec2& p, double ang) {
    double c = std::cos(ang);
    double s = std::sin(ang);
    return Vec2(p.x * c - p.y * s, p.x * s + p.y * c);
}

double cross2d(const Vec2& a, const Vec2& b) {
    return a.x * b.y - a.y * b.x;
}

bool rayIntersectsSegment(const Vec2& origin, const Vec2& dir, const Vec2& a, const Vec2& b, double* outT, double* outU) {
    Vec2 r = dir;
    Vec2 s = Vec2(b.x - a.x, b.y - a.y);
    Vec2 ap = Vec2(a.x - origin.x, a.y - origin.y);
    double rxs = cross2d(r, s);
    double apxr = cross2d(ap, r);
    if (std::abs(rxs) < 1e-9) {
        return false;
    }
    double t = cross2d(ap, s) / rxs;
    double u = apxr / rxs;
    if (t >= 0.0 && u >= 0.0 && u <= 1.0) {
        if (outT) *outT = t;
        if (outU) *outU = u;
        return true;
    }
    return false;
}

// calculateDamage 实现
double calculateDamage(AmmoType type, double distanceVal) {
    double base = 50.0;
    switch (type) {
    case AmmoType::LARGE: base = 80.0; break;
    case AmmoType::SMALL: base = 40.0; break;
    default: base = 50.0; break;
    }
    double factor = 1.0;
    if (distanceVal > 50.0) factor = 0.1;
    else factor = std::max(0.1, 1.0 - distanceVal / 50.0);
    return base * factor;
}

double calculateDamage(AmmoType type, const Vec2& a, const Vec2& b) {
    double d = distance(a, b);
    return calculateDamage(type, d);
}

double calculateDamage(AmmoType type, double x1, double y1, double x2, double y2) {
    return calculateDamage(type, Vec2(x1,y1), Vec2(x2,y2));
}

// Object 实现
void Object::setupExprTk() {
    symbleTable.add_variable("t", exprTime);
    symbleTable.add_variable("speed", exprSpeed);
    symbleTable.add_variable("angle", exprAngle);
    symbleTable.add_constant("pi", 3.14159265358979323846);

    exprX.register_symbol_table(symbleTable);
    exprY.register_symbol_table(symbleTable);
    exprDir.register_symbol_table(symbleTable);
}

void Object::compileExpressions() {
    try {
        if (!moveExprX.empty()) {
            parser.compile(moveExprX, exprX);
        }
        if (!moveExprY.empty()) {
            parser.compile(moveExprY, exprY);
        }
        if (!moveExprDir.empty()) {
            parser.compile(moveExprDir, exprDir);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ExprTk compile error for Object" << id << ": " << e.what() << std::endl;
    }
}

Object::Object(double x, double y, double dir, bool mov)
    : id(nextId++), posX(x), posY(y), direction(dir), movable(mov),
    exprTime(0), exprSpeed(1.0), exprAngle(0) {
    if (movable) setupExprTk();
}

Object::~Object() {
    std::cout << "Object" << id << "destroyed." << std::endl;
}

void Object::setMovementExpressions(const std::string & exprX_, const std::string & exprY_, const std::string & exprDir_) {
    if (movable) {
        moveExprX = exprX_;
        moveExprY = exprY_;
        moveExprDir = exprDir_;
        compileExpressions();
    }
}

// ColoredObject
ColoredObject::ColoredObject(ObjectColor col, double x, double y, double dir, bool mov)
    : Object(x, y, dir, mov), color(col) {}
ColoredObject::~ColoredObject() {}

// Target
Target::Target(ObjectColor col, double w, double x, double y, double dir, bool mov)
    : ColoredObject(col, x, y, dir, mov), width(w) {}
Target::~Target() {}
double Target::getWidth() const { return width; }

// ArmorPlate
ArmorPlate::ArmorPlate(ArmorSide s, int num, double w, const Vec2& localCtr, std::shared_ptr<Object> hostPtr, ObjectColor col)
    : Target(col, w), side(s), number(num), host(hostPtr), localCenter(localCtr) {}

void ArmorPlate::setHost(std::shared_ptr<Object> hostPtr) { host = hostPtr; }
void ArmorPlate::setLocalCenter(const Vec2& ctr) { localCenter = ctr; }

Vec2 ArmorPlate::worldCenter() const {
    if (auto h = host.lock()) {
        Vec2 offset = rotateLocalToWorld(localCenter, h->getDirection());
        return Vec2(h->getX() + offset.x, h->getY() + offset.y);
    }
    return Vec2(getX(), getY());
}

Vec2 ArmorPlate::outerNormal() const {
    Vec2 localNormal;
    switch (side) {
    case ArmorSide::FRONT: localNormal = Vec2(0, 1); break;
    case ArmorSide::BACK: localNormal = Vec2(0, -1); break;
    case ArmorSide::LEFT: localNormal = Vec2(-1, 0); break;
    case ArmorSide::RIGHT: localNormal = Vec2(1, 0); break;
    case ArmorSide::TOP: localNormal = Vec2(0, 1); break;
    case ArmorSide::BOTTOM: localNormal = Vec2(0, -1); break;
    default: localNormal = Vec2(0, 1); break;
    }

    if (auto h = host.lock()) {
        Vec2 n = rotateLocalToWorld(localNormal, h->getDirection());
        double len = std::hypot(n.x, n.y);
        if (len > 1e-9) { n.x /= len; n.y /= len; }
        return n;
    }
    double len = std::hypot(localNormal.x, localNormal.y);
    return Vec2(localNormal.x/len, localNormal.y/len);
}

bool ArmorPlate::isHittableBy(const Vec2& attackerPos) const {
    Vec2 center = worldCenter();
    Vec2 toAtt(attackerPos.x - center.x, attackerPos.y - center.y);
    Vec2 n = outerNormal();
    double dot = n.x * toAtt.x + n.y * toAtt.y;
    return dot < 0.0;
}

std::vector<std::pair<Vec2, Vec2>> ArmorPlate::GetBounds() const {
    Vec2 c = worldCenter();
    Vec2 n = outerNormal();
    Vec2 tangent(-n.y, n.x);
    double half = getWidth() / 2.0;
    Vec2 p1(c.x - tangent.x * half, c.y - tangent.y * half);
    Vec2 p2(c.x + tangent.x * half, c.y + tangent.y * half);
    return { { {p1, p2} } };
}

std::string ArmorPlate::describe() const {
    std::ostringstream ss;
    ss << "ArmorPlate #" << number << " side=" << EnumUtils::armorSideToString(side)
       << " worldCenter=(" << worldCenter().x << "," << worldCenter().y << ")";
    return ss.str();
}

// EnergyMechanism
EnergyMechanism::EnergyMechanism(double w, double multiplier, double duration, ObjectColor col)
    : Target(col, w), active(true), boostMultiplier(multiplier), boostDuration(duration) {}
void EnergyMechanism::Activate() { active = true; }
void EnergyMechanism::Deactivate() { active = false; }
bool EnergyMechanism::isActive() const { return active; }
double EnergyMechanism::getMultiplier() const { return boostMultiplier; }
double EnergyMechanism::getDuration() const { return boostDuration; }
std::vector<std::pair<Vec2, Vec2>> EnergyMechanism::GetBounds() const {
    Vec2 c(getX(),getY());
    double half = getWidth() / 2.0;
    Vec2 p1(c.x - half, c.y - half);
    Vec2 p2(c.x + half, c.y + half);
    return { { {p1, p2} } };
}
std::string EnergyMechanism::describe() const {
    std::ostringstream ss;
    ss << "EnergyMechanism active=" << (active?"yes":"no") << " pos=(" << getX() << "," << getY() << ")";
    return ss.str();
}

// Obstacle
Obstacle::Obstacle(double w, bool destr, ObjectColor col)
    : Target(col, w), destructible(destr) {}
void Obstacle::addSegment(const Vec2& a, const Vec2& b) { segments.push_back({a,b}); }
const std::vector<std::pair<Vec2, Vec2>>& Obstacle::getSegments() const { return segments; }
std::vector<std::pair<Vec2, Vec2>> Obstacle::GetBounds() const { return segments; }
std::string Obstacle::describe() const { std::ostringstream ss; ss << "Obstacle destructible=" << (destructible?"yes":"no") << " segments=" << segments.size(); return ss.str(); }

// Robot
Robot::Robot(ObjectColor col, double len, double wid, double x, double y, double dir)
    : ColoredObject(col, x, y, dir, true), chassisLength(len), chassisWidth(wid), gimbalDirection(0) {}
Robot::~Robot() {}

void Robot::addArmor(const std::shared_ptr<ArmorPlate>& ap) {
    if (!ap) return;
    armors.push_back(ap);
    std::shared_ptr<Object> asObj = std::static_pointer_cast<Object>(shared_from_this());
    ap->setHost(asObj);
}
void Robot::UpdateArmors() {
    std::shared_ptr<Object> asObj = std::static_pointer_cast<Object>(shared_from_this());
    for (auto &a : armors) { if (a) a->setHost(asObj); }
}
const std::vector<std::shared_ptr<ArmorPlate>>& Robot::getArmors() const { return armors; }
void Robot::setGimbalDirection(double g) { gimbalDirection = g; }
double Robot::getGimbalDirection() const { return gimbalDirection; }

// EnemyRobot
EnemyRobot::EnemyRobot(double hp, double maxHp, ObjectColor col, double len, double wid, double x, double y, double dir)
    : Robot(col, len, wid, x, y, dir), health(hp), maxHealth(maxHp) {}
bool EnemyRobot::isAlive() const { return health > 0.0; }
void EnemyRobot::TakeDamage(double dmg) { if (dmg <= 0) return; health -= dmg; if (health < 0) health = 0; std::cout << "EnemyRobot took " << dmg << " damage, remaining HP=" << health << std::endl; if (!isAlive()) std::cout << "EnemyRobot destroyed." << std::endl; }
double EnemyRobot::getHealth() const { return health; }

// SentryRobot
SentryRobot::SentryRobot(ObjectColor col, double len, double wid, double x, double y, double dir)
    : Robot(col, len, wid, x, y, dir), damageBonus(1.0) { ++instanceCount; }
SentryRobot::~SentryRobot() { --instanceCount; }
void SentryRobot::setDamageBonus(double b) { damageBonus = b; }
double SentryRobot::getDamageBonus() const { return damageBonus; }
void SentryRobot::move(double length, double dirRad) { double dx = length * std::cos(dirRad); double dy = length * std::sin(dirRad); setPosition(getX() + dx, getY() + dy); }
void SentryRobot::changeDirection(double changedRad) { setDirection(getDirection() + changedRad); }
void SentryRobot::fire(const std::shared_ptr<ArmorPlate>& target, AmmoType ammo) {
    if (!target) return;
    Vec2 myPos(getX(), getY());
    Vec2 center = target->worldCenter();
    double dx = center.x - myPos.x;
    double dy = center.y - myPos.y;
    double dist = std::hypot(dx, dy);
    double baseDmg = calculateDamage(ammo, dist);
    double totalDmg = baseDmg * damageBonus;

    std::cout << "SentryRobot fired at Armor#" << target->getNumber()
              << " dist=" << dist << " dmg=" << totalDmg << std::endl;

    if (auto hostObj = target->getHost().lock()) {
        std::shared_ptr<EnemyRobot> enemy = std::dynamic_pointer_cast<EnemyRobot>(hostObj);
        if (enemy) {
            enemy->TakeDamage(totalDmg);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const SentryRobot& s) {
    os << "SentryRobot pos=(" << s.getX() << "," << s.getY() << ") dir=" << s.getDirection()
       << " damageBonus=" << s.damageBonus << " instances=" << SentryRobot::instanceCount;
    return os;
}

bool isArmorVisible(const SentryRobot& sentry, const std::shared_ptr<ArmorPlate>& ap, const std::vector<std::shared_ptr<Obstacle>>& obstacles) {
    Vec2 origin(sentry.getX(), sentry.getY());
    Vec2 center = ap->worldCenter();
    Vec2 dir = Vec2(center.x - origin.x, center.y - origin.y);
    for (const auto& obs : obstacles) {
        if (!obs) continue;
        for (const auto& seg : obs->getSegments()) {
            double t = 0.0, u = 0.0;
            if (rayIntersectsSegment(origin, dir, seg.first, seg.second, &t, &u)) {
                if (t > 1e-9 && t < 1.0 - 1e-9) return false;
            }
        }
    }
    return true;
}

// ---------------------- main ----------------------
int main(int argc, char** argv) {
    // Load YAML config if provided
    YAML::Node config;
    if (argc > 1) {
        try {
            config = YAML::LoadFile(argv[1]);
        } catch (...) {
            std::cerr << "Failed to load provided config, falling back to default." << std::endl;
        }
    }

    if (!config) {
        // build a minimal default config
        config["global"]["armor_plate_width"] = 0.5;
        config["global"]["base_damage"] = 50.0;
        config["ally_sentry"]["position"]["x"] = 0;
        config["ally_sentry"]["position"]["y"] = 0;
        config["ally_sentry"]["direction"]["x"] = 1;
        config["ally_sentry"]["direction"]["y"] = 0;
        config["enemy_robots"] = YAML::Node(YAML::NodeType::Sequence);
        YAML::Node e;
        e["position"]["x"] = 10.0;
        e["position"]["y"] = 0.0;
        e["direction"]["x"] = 1;
        e["direction"]["y"] = 0;
        e["health"] = 150.0;
        e["max_health"] = 150.0;
        e["armors"] = YAML::Node(YAML::NodeType::Sequence);
        YAML::Node a;
        a["side"] = "front";
        a["number"] = 1;
        e["armors"].push_back(a);
        config["enemy_robots"].push_back(e);
    }

    // Create sentry
    double sx = config["ally_sentry"]["position"]["x"].as<double>(0.0);
    double sy = config["ally_sentry"]["position"]["y"].as<double>(0.0);
    auto sentry = std::make_shared<SentryRobot>(ObjectColor::BLUE, 1.0, 1.0, sx, sy, 0.0);

    // Create obstacles
    std::vector<std::shared_ptr<Obstacle>> obstacles;

    // Create enemies
    std::vector<std::shared_ptr<EnemyRobot>> enemies;
    if (config["enemy_robots"]) {
        for (const auto& en : config["enemy_robots"]) {
            double ex = en["position"]["x"].as<double>(0.0);
            double ey = en["position"]["y"].as<double>(0.0);
            double dirx = en["direction"]["x"].as<double>(1.0);
            double diry = en["direction"]["y"].as<double>(0.0);
            double health = en["health"].as<double>(100.0);
            double maxh = en["max_health"].as<double>(health);
            auto enemy = std::make_shared<EnemyRobot>(health, maxh, ObjectColor::RED, 1.0, 1.0, ex, ey, 0.0);
            // armors
            if (en["armors"]) {
                for (const auto& ar : en["armors"]) {
                    std::string side = ar["side"].as<std::string>("front");
                    int num = ar["number"].as<int>(1);
                    ArmorSide s = EnumUtils::stringToArmorSide(side);
                    auto ap = std::make_shared<ArmorPlate>(s, num, config["global"]["armor_plate_width"].as<double>(0.5), Vec2(0,0));
                    enemy->addArmor(ap);
                }
            }
            enemy->UpdateArmors();
            enemies.push_back(enemy);
        }
    }

    const int maxIter = 2000;
    int iter = 0;
    int shotsFired = 0;

    while (iter++ < maxIter) {
        // gather hittable armors
        std::shared_ptr<ArmorPlate> chosen;
        double bestDist = 1e308;
        for (auto& en : enemies) {
            if (!en->isAlive()) continue;
            en->UpdateArmors();
            for (auto& ap : en->getArmors()) {
                if (!ap) continue;
                // check single-side hittable
                if (!ap->isHittableBy(Vec2(sentry->getX(), sentry->getY()))) continue;
                // check visibility
                if (!isArmorVisible(*sentry, ap, obstacles)) continue;
                double d = distance(Vec2(sentry->getX(), sentry->getY()), ap->worldCenter());
                if (d < bestDist) { bestDist = d; chosen = ap; }
            }
        }

        if (chosen) {
            sentry->fire(chosen, AmmoType::LARGE);
            ++shotsFired;
        }

        // end if all enemies dead
        bool anyAlive = false;
        for (auto& en : enemies) { if (en->isAlive()) { anyAlive = true; break; } }
        if (!anyAlive) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "Simulation finished. Iter=" << iter << " shots=" << shotsFired << std::endl;
    return 0;
}
