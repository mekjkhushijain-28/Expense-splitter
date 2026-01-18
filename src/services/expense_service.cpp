#include "services/expense_service.h"
#include <cmath>

std::map<int, double> ExpenseService::calculate_equal_splits(int group_id, double amount) {
    auto group = db.get_group(group_id);
    if (!group.has_value()) {
        throw std::runtime_error("Group not found");
    }
    
    std::map<int, double> splits;
    int num_members = group->member_ids.size();
    double split_amount = std::round(amount / num_members * 100.0) / 100.0;
    
    for (int member_id : group->member_ids) {
        splits[member_id] = split_amount;
    }
    
    return splits;
}

bool ExpenseService::validate_exact_splits(const std::map<int, double>& splits, double amount) {
    double total = 0.0;
    for (const auto& [_, split_amount] : splits) {
        total += split_amount;
    }
    return std::abs(total - amount) < 0.01;
}

Expense ExpenseService::add_expense(int group_id, const std::string& description,
                                   double amount, int paid_by, SplitType split_type,
                                   const std::map<int, double>& custom_splits) {
    // Validate group
    auto group = db.get_group(group_id);
    if (!group.has_value()) {
        throw std::runtime_error("Group not found");
    }
    
    // Validate paying user
    if (!db.get_user(paid_by).has_value()) {
        throw std::runtime_error("Paying user not found");
    }
    
    auto& members = group->member_ids;
    if (std::find(members.begin(), members.end(), paid_by) == members.end()) {
        throw std::runtime_error("Paying user not in group");
    }
    
    // Calculate splits
    std::map<int, double> splits;
    if (split_type == SplitType::EQUAL) {
        splits = calculate_equal_splits(group_id, amount);
    } else {
        if (custom_splits.empty()) {
            throw std::runtime_error("Exact splits required");
        }
        if (!validate_exact_splits(custom_splits, amount)) {
            throw std::runtime_error("Splits don't match total amount");
        }
        splits = custom_splits;
    }
    
    // Create expense
    Expense expense{0, group_id, description, amount, paid_by, split_type, splits};
    return db.create_expense(expense);
}