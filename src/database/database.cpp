// src/database/database.cpp
#include "database/database.h"

User Database::create_user(const std::string& name, const std::string& email) {
    User user{user_counter++, name, email};
    users[user.id] = user;
    return user;
}

std::optional<User> Database::get_user(int id) const {
    auto it = users.find(id);
    if (it != users.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<User> Database::get_all_users() const {
    std::vector<User> result;
    for (const auto& [_, user] : users) {
        result.push_back(user);
    }
    return result;
}

Group Database::create_group(const std::string& name, const std::vector<int>& member_ids) {
    Group group{group_counter++, name, member_ids};
    groups[group.id] = group;
    return group;
}

std::optional<Group> Database::get_group(int id) const {
    auto it = groups.find(id);
    if (it != groups.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<Group> Database::get_all_groups() const {
    std::vector<Group> result;
    for (const auto& [_, group] : groups) {
        result.push_back(group);
    }
    return result;
}

bool Database::add_member_to_group(int group_id, int user_id) {
    auto it = groups.find(group_id);
    if (it == groups.end()) return false;
    
    auto& members = it->second.member_ids;
    if (std::find(members.begin(), members.end(), user_id) == members.end()) {
        members.push_back(user_id);
    }
    return true;
}

Expense Database::create_expense(const Expense& expense) {
    Expense new_expense = expense;
    new_expense.id = expense_counter++;
    expenses[new_expense.id] = new_expense;
    return new_expense;
}

std::vector<Expense> Database::get_expenses(std::optional<int> group_id) const {
    std::vector<Expense> result;
    for (const auto& [_, expense] : expenses) {
        if (!group_id.has_value() || expense.group_id == group_id.value()) {
            result.push_back(expense);
        }
    }
    return result;
}

Settlement Database::create_settlement(const Settlement& settlement) {
    Settlement new_settlement = settlement;
    new_settlement.id = settlement_counter++;
    settlements[new_settlement.id] = new_settlement;
    return new_settlement;
}

std::vector<Settlement> Database::get_settlements(std::optional<int> group_id) const {
    std::vector<Settlement> result;
    for (const auto& [_, settlement] : settlements) {
        if (!group_id.has_value() || settlement.group_id == group_id.value()) {
            result.push_back(settlement);
        }
    }
    return result;
}