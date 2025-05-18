#include "hcmcampaign.h"
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// ====================== Position ==========================
Position::Position(int r, int c) : r(r), c(c) {}
Position::Position(const string &str_pos) {
    sscanf(str_pos.c_str(), "(%d,%d)", &r, &c);
}
int Position::getRow() const { return r; }
int Position::getCol() const { return c; }
void Position::setRow(int r) { this->r = r; }
void Position::setCol(int c) { this->c = c; }
string Position::str() const {
    ostringstream oss;
    oss << "(" << r << "," << c << ")";
    return oss.str();
}

// ====================== Unit (Abstract) ======================
Unit::Unit(int quantity, int weight, Position pos)
    : quantity(quantity), weight(weight), pos(pos) {}
Unit::~Unit() {}
Position Unit::getCurrentPosition() const { return pos; }
int Unit::getQuantity() const { return quantity; }
int Unit::getWeight() const { return weight; }
void Unit::setQuantity(int q) { quantity = q; }
void Unit::setWeight(int w) { weight = w; }

// ====================== Vehicle ==========================
Vehicle::Vehicle(int quantity, int weight, const Position pos, VehicleType vehicleType)
    : Unit(quantity, weight, pos), vehicleType(vehicleType) {}
int Vehicle::getAttackScore() {
    return vehicleType * 304 + (int)ceil((double)quantity * weight / 30.0);
}
string Vehicle::str() const {
    ostringstream oss;
    oss << "Vehicle[vehicleType=" << vehicleType
        << ",quantity=" << quantity
        << ",weight=" << weight
        << ",pos=" << pos.str() << "]";
    return oss.str();
}
VehicleType Vehicle::getVehicleType() const { return vehicleType; }

// ====================== Infantry ==========================
Infantry::Infantry(int quantity, int weight, const Position pos, InfantryType infantryType)
    : Unit(quantity, weight, pos), infantryType(infantryType) {}
int Infantry::getAttackScore() {
    int score = infantryType * 56 + quantity * weight;
    if (infantryType == SPECIALFORCES) {
        int sq = (int)sqrt(weight);
        if (sq * sq == weight) score += 75;
    }
    int y = 1975, n = score + y;
    while (n >= 10) {
        int s = 0, t = n;
        while (t) { s += t % 10; t /= 10; }
        n = s;
    }
    if (n > 7) quantity = (int)ceil(quantity * 1.2);
    if (n < 3) quantity = (int)floor(quantity * 0.9);
    // Re-calc score
    score = infantryType * 56 + quantity * weight;
    if (infantryType == SPECIALFORCES) {
        int sq = (int)sqrt(weight);
        if (sq * sq == weight) score += 75;
    }
    return score;
}
string Infantry::str() const {
    ostringstream oss;
    oss << "Infantry[infantryType=" << infantryType
        << ",quantity=" << quantity
        << ",weight=" << weight
        << ",pos=" << pos.str() << "]";
    return oss.str();
}
InfantryType Infantry::getInfantryType() const { return infantryType; }

// ====================== UnitList ==========================
UnitList::UnitList(int capacity)
    : capacity(capacity), head(nullptr), count_vehicle(0), count_infantry(0) {}
UnitList::~UnitList() {
    Node *cur = head;
    while (cur) {
        Node *tmp = cur;
        cur = cur->next;
        delete tmp->data;
        delete tmp;
    }
}
bool UnitList::insert(Unit *unit) {
    Vehicle *v = dynamic_cast<Vehicle*>(unit);
    Infantry *i = dynamic_cast<Infantry*>(unit);

    Node *cur = head;
    while (cur) {
        if (v && dynamic_cast<Vehicle*>(cur->data) && ((Vehicle*)cur->data)->getVehicleType() == v->getVehicleType()) {
            cur->data->setQuantity(cur->data->getQuantity() + v->getQuantity());
            delete v;
            return true;
        }
        if (i && dynamic_cast<Infantry*>(cur->data) && ((Infantry*)cur->data)->getInfantryType() == i->getInfantryType()) {
            cur->data->setQuantity(cur->data->getQuantity() + i->getQuantity());
            delete i;
            return true;
        }
        cur = cur->next;
    }

    if (v) {
        Node **tail = &head;
        while (*tail) tail = &((*tail)->next);
        *tail = new Node(unit);
        count_vehicle++;
    } else if (i) {
        Node *n = new Node(unit);
        n->next = head;
        head = n;
        count_infantry++;
    }
    return true;
}
bool UnitList::isContain(VehicleType vehicleType) {
    Node *cur = head;
    while (cur) {
        Vehicle *v = dynamic_cast<Vehicle *>(cur->data);
        if (v && v->getVehicleType() == vehicleType) return true;
        cur = cur->next;
    }
    return false;
}
bool UnitList::isContain(InfantryType infantryType) {
    Node *cur = head;
    while (cur) {
        Infantry *i = dynamic_cast<Infantry *>(cur->data);
        if (i && i->getInfantryType() == infantryType) return true;
        cur = cur->next;
    }
    return false;
}
string UnitList::str() const {
    ostringstream oss;
    oss << "UnitList[count_vehicle=" << count_vehicle
        << ";count_infantry=" << count_infantry << ";";
    Node *cur = head;
    bool first = true;
    while (cur) {
        if (!first) oss << ",";
        oss << cur->data->str();
        first = false;
        cur = cur->next;
    }
    oss << "]";
    return oss.str();
}
int UnitList::getCountVehicle() const { return count_vehicle; }
int UnitList::getCountInfantry() const { return count_infantry; }
int UnitList::getTotalCount() const { return count_vehicle + count_infantry; }
Unit* UnitList::getUnitAt(int idx) const {
    Node *cur = head;
    int cnt = 0;
    while (cur) {
        if (cnt == idx) return cur->data;
        cnt++;
        cur = cur->next;
    }
    return nullptr;
}
void UnitList::removeIfAttackScoreLE5() {}

// ====================== BattleField (optional for now) ==========================
BattleField::BattleField(int n_rows, int n_cols, vector<Position *> arrayForest,
                         vector<Position *> arrayRiver, vector<Position *> arrayFortification,
                         vector<Position *> arrayUrban, vector<Position *> arraySpecialZone)
    : n_rows(n_rows), n_cols(n_cols) {}
BattleField::~BattleField() {}
string BattleField::str() const {
    ostringstream oss;
    oss << "BattleField[n_rows=" << n_rows << ",n_cols=" << n_cols << "]";
    return oss.str();
}
int BattleField::getRows() const { return n_rows; }
int BattleField::getCols() const { return n_cols; }

// ====================== Army / LiberationArmy / ARVN ==========================
Army::Army(Unit **unitArray, int size, string name, BattleField *battleField)
    : LF(0), EXP(0), name(name), unitList(new UnitList(size)), battleField(battleField) {
    for (int i = 0; i < size; ++i) unitList->insert(unitArray[i]);
    updateLF_EXP();
}
Army::~Army() { delete unitList; }
int Army::getLF() const { return LF; }
int Army::getEXP() const { return EXP; }
string Army::getName() const { return name; }
UnitList* Army::getUnitList() const { return unitList; }
void Army::updateLF_EXP() {
    LF = 0; EXP = 0;
    UnitList::Node* cur = unitList->head;
    while (cur) {
        Vehicle* v = dynamic_cast<Vehicle*>(cur->data);
        Infantry* i = dynamic_cast<Infantry*>(cur->data);
        if (v) LF += v->getAttackScore();
        if (i) EXP += i->getAttackScore();
        cur = cur->next;
    }
    if (LF > 1000) LF = 1000;
    if (EXP > 500) EXP = 500;
}

LiberationArmy::LiberationArmy(Unit **unitArray, int size, string name, BattleField *battleField)
    : Army(unitArray, size, name, battleField) {}
void LiberationArmy::fight(Army *enemy, bool defense) {}
string LiberationArmy::str() const {
    ostringstream oss;
    oss << "LiberationArmy[name=" << name
        << ",LF=" << LF
        << ",EXP=" << EXP
        << "," << unitList->str()
        << "]";
    return oss.str();
}

ARVN::ARVN(Unit **unitArray, int size, string name, BattleField *battleField)
    : Army(unitArray, size, name, battleField) {}
void ARVN::fight(Army *enemy, bool defense) {}
string ARVN::str() const {
    ostringstream oss;
    oss << "ARVN[name=" << name
        << ",LF=" << LF
        << ",EXP=" << EXP
        << "," << unitList->str()
        << "]";
    return oss.str();
}

// ====================== Configuration (đọc file config.txt thật) ==========================
static vector<string> splitListStr(const string &arr) {
    vector<string> res;
    int depth = 0; string tmp;
    for (char ch : arr) {
        if (ch == '(') depth++;
        if (ch == ')') depth--;
        if (ch == ',' && depth == 0) {
            if (!tmp.empty()) res.push_back(tmp);
            tmp.clear();
        } else if (ch != '[' && ch != ']') {
            tmp += ch;
        }
    }
    if (!tmp.empty()) res.push_back(tmp);
    return res;
}
static Position* parsePos(const string& s) {
    int r, c;
    sscanf(s.c_str(), "(%d,%d)", &r, &c);
    return new Position(r, c);
}
Configuration::Configuration(const string& filepath)
    : num_rows(0), num_cols(0), liberationUnits(nullptr), liberationUnitsCount(0),
      ARVNUnits(nullptr), ARVNUnitsCount(0), eventCode(0) {
    ifstream fin(filepath);
    string line;
    vector<Unit*> liber, arvn;
    while (getline(fin, line)) {
        if (line.find("NUM_ROWS") != string::npos) {
            num_rows = stoi(line.substr(line.find("=") + 1));
        }
        if (line.find("NUM_COLS") != string::npos) {
            num_cols = stoi(line.substr(line.find("=") + 1));
        }
        if (line.find("EVENT_CODE") != string::npos) {
            eventCode = stoi(line.substr(line.find("=") + 1));
        }
        if (line.find("ARRAY_FOREST") != string::npos) {
            size_t start = line.find('['), end = line.find(']');
            vector<string> lst = splitListStr(line.substr(start + 1, end - start - 1));
            for (auto &s : lst) arrayForest.push_back(parsePos(s));
        }
        if (line.find("ARRAY_RIVER") != string::npos) {
            size_t start = line.find('['), end = line.find(']');
            vector<string> lst = splitListStr(line.substr(start + 1, end - start - 1));
            for (auto &s : lst) arrayRiver.push_back(parsePos(s));
        }
        if (line.find("ARRAY_FORTIFICATION") != string::npos) {
            size_t start = line.find('['), end = line.find(']');
            vector<string> lst = splitListStr(line.substr(start + 1, end - start - 1));
            for (auto &s : lst) arrayFortification.push_back(parsePos(s));
        }
        if (line.find("ARRAY_URBAN") != string::npos) {
            size_t start = line.find('['), end = line.find(']');
            vector<string> lst = splitListStr(line.substr(start + 1, end - start - 1));
            for (auto &s : lst) arrayUrban.push_back(parsePos(s));
        }
        if (line.find("ARRAY_SPECIAL_ZONE") != string::npos) {
            size_t start = line.find('['), end = line.find(']');
            vector<string> lst = splitListStr(line.substr(start + 1, end - start - 1));
            for (auto &s : lst) arraySpecialZone.push_back(parsePos(s));
        }
        if (line.find("UNIT_LIST") != string::npos) {
            size_t start = line.find('['), end = line.find(']');
            vector<string> lst = splitListStr(line.substr(start + 1, end - start - 1));
            for (auto &s : lst) {
                // VD: TANK(5,2,(1,2),0)
                size_t nameEnd = s.find('(');
                string name = s.substr(0, nameEnd);
                vector<string> fields;
                string content = s.substr(nameEnd + 1, s.length() - nameEnd - 2); // bỏ ()
                stringstream ss(content);
                string field;
                while (getline(ss, field, ',')) fields.push_back(field);
                int q = stoi(fields[0]), w = stoi(fields[1]);
                Position pos(fields[2]);
                int army = stoi(fields[3]);
                if (name == "TANK") {
                    Vehicle *unit = new Vehicle(q, w, pos, TANK);
                    if (army == 0) liber.push_back(unit);
                    else arvn.push_back(unit);
                } else if (name == "REGULARINFANTRY") {
                    Infantry *unit = new Infantry(q, w, pos, REGULARINFANTRY);
                    if (army == 0) liber.push_back(unit);
                    else arvn.push_back(unit);
                }
                // Thêm các loại khác nếu cần (nếu đề yêu cầu)
            }
        }
    }
    liberationUnitsCount = liber.size();
    ARVNUnitsCount = arvn.size();
    liberationUnits = liberationUnitsCount ? new Unit*[liberationUnitsCount] : nullptr;
    ARVNUnits = ARVNUnitsCount ? new Unit*[ARVNUnitsCount] : nullptr;
    for (int i = 0; i < liberationUnitsCount; ++i) liberationUnits[i] = liber[i];
    for (int i = 0; i < ARVNUnitsCount; ++i) ARVNUnits[i] = arvn[i];
    fin.close();
}
Configuration::~Configuration() {
    for (auto p : arrayForest) delete p;
    for (auto p : arrayRiver) delete p;
    for (auto p : arrayFortification) delete p;
    for (auto p : arrayUrban) delete p;
    for (auto p : arraySpecialZone) delete p;
    if (liberationUnits) {
        for (int i = 0; i < liberationUnitsCount; ++i) delete liberationUnits[i];
        delete[] liberationUnits;
    }
    if (ARVNUnits) {
        for (int i = 0; i < ARVNUnitsCount; ++i) delete ARVNUnits[i];
        delete[] ARVNUnits;
    }
}
string Configuration::str() const {
    ostringstream oss;
    oss << "Configuration[";
    oss << "num_rows=" << num_rows << ",num_cols=" << num_cols << ",";

    auto formatPosList = [](const vector<Position*>& vec) -> string {
        ostringstream s;
        s << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) s << ",";
            s << vec[i]->str();
        }
        s << "]";
        return s.str();
    };

    oss << "arrayForest=" << formatPosList(arrayForest) << ",";
    oss << "arrayRiver=" << formatPosList(arrayRiver) << ",";
    oss << "arrayFortification=" << formatPosList(arrayFortification) << ",";
    oss << "arrayUrban=" << formatPosList(arrayUrban) << ",";
    oss << "arraySpecialZone=" << formatPosList(arraySpecialZone) << ",";

    // Liberation Units
    oss << "liberationUnits=[";
    for (int i = 0; i < liberationUnitsCount; ++i) {
        if (i > 0) oss << ",";
        oss << liberationUnits[i]->str();
    }
    oss << "],";

    // ARVN Units
    oss << "ARVNUnits=[";
    for (int i = 0; i < ARVNUnitsCount; ++i) {
        if (i > 0) oss << ",";
        oss << ARVNUnits[i]->str();
    }
    oss << "],";

    oss << "eventCode=" << eventCode << "]";
    return oss.str();
}

int Configuration::getNumRows() const { return num_rows; }
int Configuration::getNumCols() const { return num_cols; }
const vector<Position*>& Configuration::getArrayForest() const { return arrayForest; }
const vector<Position*>& Configuration::getArrayRiver() const { return arrayRiver; }
const vector<Position*>& Configuration::getArrayFortification() const { return arrayFortification; }
const vector<Position*>& Configuration::getArrayUrban() const { return arrayUrban; }
const vector<Position*>& Configuration::getArraySpecialZone() const { return arraySpecialZone; }
Unit** Configuration::getLiberationUnits() const { return liberationUnits; }
int Configuration::getLiberationUnitsCount() const { return liberationUnitsCount; }
Unit** Configuration::getARVNUnits() const { return ARVNUnits; }
int Configuration::getARVNUnitsCount() const { return ARVNUnitsCount; }
int Configuration::getEventCode() const { return eventCode; }

// ====================== HCMCampaign ==========================
HCMCampaign::HCMCampaign(const string &config_file_path)
    : config(nullptr), battleField(nullptr), liberationArmy(nullptr), arvn(nullptr) {
    config = new Configuration(config_file_path);
    // Optional: khởi tạo luôn army và battlefield ở đây nếu muốn
}
HCMCampaign::~HCMCampaign() {
    delete config;
    delete battleField;
    delete liberationArmy;
    delete arvn;
}
void HCMCampaign::run() {}
string HCMCampaign::printResult() { return ""; }
