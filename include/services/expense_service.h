#pragma once
#include "../models/models.h"
#include "../database/database.h"
#include <stdexcept>

class ExpenseService {
private:
    Database& db;
    
    std::map<int, double> calculate_equal_splits(int group_id, double amount);
    bool validate_exact_splits(const std::map<int, double>& splits, double amount);
    
public:
    explicit ExpenseService(Database& database) : db(database) {}
    
    Expense add_expense(int group_id, const std::string& description, 
                       double amount, int paid_by, SplitType split_type,
                       const std::map<int, double>& custom_splits = {});
};