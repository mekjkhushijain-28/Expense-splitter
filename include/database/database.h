#pragma once
#include "../models/models.h"
#include <map>
#include <memory>
#include <optional>

class Database {
private:
    std::map<int, User> users;
    std::map<int, Group> groups;
    std::map<int, Expense> expenses;
    std::map<int, Settlement> settlements;
    
    int user_counter = 1;
    int group_counter = 1;
    int expense_counter = 1;
    int settlement_counter = 1;

public:
    // User operations
    User create_user(const std::string& name, const std::string& email);
    std::optional<User> get_user(int id) const;
    std::vector<User> get_all_users() const;
    
    // Group operations
    Group create_group(const std::string& name, const std::vector<int>& member_ids);
    std::optional<Group> get_group(int id) const;
    std::vector<Group> get_all_groups() const;
    bool add_member_to_group(int group_id, int user_id);
    
    // Expense operations
    Expense create_expense(const Expense& expense);
    std::vector<Expense> get_expenses(std::optional<int> group_id = std::nullopt) const;
    
    // Settlement operations
    Settlement create_settlement(const Settlement& settlement);
    std::vector<Settlement> get_settlements(std::optional<int> group_id = std::nullopt) const;
};