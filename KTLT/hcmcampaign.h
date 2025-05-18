#ifndef _H_HCM_CAMPAIGN_H_
#define _H_HCM_CAMPAIGN_H_

#include "main.h"

enum VehicleType { TRUCK, MORTAR, ANTIAIRCRAFT, ARMOREDCAR, APC, ARTILLERY, TANK };
enum InfantryType { SNIPER, ANTIAIRCRAFTSQUAD, MORTARSQUAD, ENGINEER, SPECIALFORCES, REGULARINFANTRY };

class Position {
private:
    int r, c;
public:
    Position(int r = 0, int c = 0);
    Position(const string &str_pos);
    int getRow() const;
    int getCol() const;
    void setRow(int r);
    void setCol(int c);
    string str() const;
};

class Unit {
protected:
    int quantity, weight;
    Position pos;
public:
    Unit(int quantity, int weight, Position pos);
    virtual ~Unit();
    virtual int getAttackScore() = 0;
    Position getCurrentPosition() const;
    virtual string str() const = 0;
    int getQuantity() const;
    int getWeight() const;
    void setQuantity(int q);
    void setWeight(int w);
};

class Vehicle : public Unit {
private:
    VehicleType vehicleType;
public:
    Vehicle(int quantity, int weight, const Position pos, VehicleType vehicleType);
    int getAttackScore();
    string str() const;
    VehicleType getVehicleType() const;
};

class Infantry : public Unit {
private:
    InfantryType infantryType;
public:
    Infantry(int quantity, int weight, const Position pos, InfantryType infantryType);
    int getAttackScore();
    string str() const;
    InfantryType getInfantryType() const;
};

class UnitList {
public:
    struct Node {
        Unit* data;
        Node* next;
        Node(Unit* u) : data(u), next(nullptr) {}
    };
private:
    int capacity;
    Node* head;
    int count_vehicle, count_infantry;
public:
    UnitList(int capacity);
    ~UnitList();
    bool insert(Unit *unit);
    bool isContain(VehicleType vehicleType);
    bool isContain(InfantryType infantryType);
    string str() const;
    int getCountVehicle() const;
    int getCountInfantry() const;
    int getTotalCount() const;
    Unit* getUnitAt(int idx) const;
    void removeIfAttackScoreLE5();
    friend class Army;
};

class BattleField {
private:
    int n_rows, n_cols;
public:
    BattleField(int n_rows, int n_cols, vector<Position *> arrayForest,
                vector<Position *> arrayRiver, vector<Position *> arrayFortification,
                vector<Position *> arrayUrban, vector<Position *> arraySpecialZone);
    ~BattleField();
    string str() const;
    int getRows() const;
    int getCols() const;
};

class Army {
protected:
    int LF, EXP;
    string name;
    UnitList *unitList;
    BattleField *battleField;
public:
    Army(Unit **unitArray, int size, string name, BattleField *battleField);
    virtual ~Army();
    virtual void fight(Army *enemy, bool defense = false) = 0;
    virtual string str() const = 0;
    int getLF() const;
    int getEXP() const;
    string getName() const;
    UnitList* getUnitList() const;
    void updateLF_EXP();
};

class LiberationArmy : public Army {
public:
    LiberationArmy(Unit **unitArray, int size, string name, BattleField *battleField);
    void fight(Army *enemy, bool defense = false);
    string str() const;
};

class ARVN : public Army {
public:
    ARVN(Unit **unitArray, int size, string name, BattleField *battleField);
    void fight(Army *enemy, bool defense = false);
    string str() const;
};

class Configuration {
private:
    int num_rows, num_cols;
    vector<Position*> arrayForest, arrayRiver, arrayFortification, arrayUrban, arraySpecialZone;
    Unit** liberationUnits;
    int liberationUnitsCount;
    Unit** ARVNUnits;
    int ARVNUnitsCount;
    int eventCode;
public:
    Configuration(const string& filepath);
    ~Configuration();
    string str() const;
    int getNumRows() const;
    int getNumCols() const;
    const vector<Position*>& getArrayForest() const;
    const vector<Position*>& getArrayRiver() const;
    const vector<Position*>& getArrayFortification() const;
    const vector<Position*>& getArrayUrban() const;
    const vector<Position*>& getArraySpecialZone() const;
    Unit** getLiberationUnits() const;
    int getLiberationUnitsCount() const;
    Unit** getARVNUnits() const;
    int getARVNUnitsCount() const;
    int getEventCode() const;
};

class HCMCampaign {
private:
    Configuration *config;
    BattleField *battleField;
    LiberationArmy *liberationArmy;
    ARVN *arvn;
public:
    HCMCampaign(const string &config_file_path);
    ~HCMCampaign();
    void run();
    string printResult();
};

#endif
