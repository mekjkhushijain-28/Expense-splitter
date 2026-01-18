#pragma once
#include "../models/models.h"
#include "../database/database.h"
#include <map>

class BalanceService {
private:
    const Database& db;
    
public:
    explicit BalanceService(const Database& database) : db(database) {}
    
    std::map<int, double> calculate_balances(int group_id) const;
    double get_user_balance(int group_id, int user_id) const;
};
