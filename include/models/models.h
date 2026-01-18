#pragma once
#include <string>
#include <vector>
#include <map>

enum class SplitType {
    EQUAL,
    EXACT
};

struct User {
    int id;
    std::string name;
    std::string email;
};

struct Group {
    int id;
    std::string name;
    std::vector<int> member_ids;
};

struct Expense {
    int id;
    int group_id;
    std::string description;
    double amount;
    int paid_by;
    SplitType split_type;
    std::map<int, double> splits;
};

struct Settlement {
    int id;
    int group_id;
    int from_user;
    int to_user;
    double amount;
};
