#include "services/balance_service.h"
#include <cmath>

std::map<int, double> BalanceService::calculate_balances(int group_id) const {
    std::map<int, double> balances;
    
    auto group = db.get_group(group_id);
    if (!group.has_value()) return balances;
    
    // Initialize balances
    for (int member_id : group->member_ids) {
        balances[member_id] = 0.0;
    }
    
    // Process expenses
    auto expenses = db.get_expenses(group_id);
    for (const auto& expense : expenses) {
        // First, everyone owes their split amount
        for (const auto& [user_id, amount] : expense.splits) {
            balances[user_id] -= amount;
        }
        
        // Then, the person who paid gets the full amount back
        balances[expense.paid_by] += expense.amount;
    }
    
    // Process settlements
    auto settlements = db.get_settlements(group_id);
    for (const auto& settlement : settlements) {
        balances[settlement.from_user] += settlement.amount;
        balances[settlement.to_user] -= settlement.amount;
    }
    
    return balances;
}

double BalanceService::get_user_balance(int group_id, int user_id) const {
    auto balances = calculate_balances(group_id);
    auto it = balances.find(user_id);
    return it != balances.end() ? it->second : 0.0;
}
