#include "api/api_server.h"

void ApiServer::setup_routes() {
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        json response = {{"message", "Expense Splitting System API"}};
        res.set_content(response.dump(), "application/json");
    });

    server.Post("/users", [this](const httplib::Request& req, httplib::Response& res) {handle_create_user(req, res);});
    server.Get("/users", [this](const httplib::Request& req, httplib::Response& res) {handle_get_users(req, res);});
    server.Get("/users/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {handle_get_user(req, res);});
    server.Post("/groups", [this](const httplib::Request& req, httplib::Response& res) {handle_create_group(req, res);});
    server.Get("/groups", [this](const httplib::Request& req, httplib::Response& res) {handle_get_groups(req, res);});
    server.Get("/groups/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {handle_get_group(req, res);});
    server.Post("/groups/(\\d+)/members", [this](const httplib::Request& req, httplib::Response& res) {handle_add_member(req, res);});
    server.Post("/expenses", [this](const httplib::Request& req, httplib::Response& res) {handle_add_expense(req, res);});
    server.Get("/expenses", [this](const httplib::Request& req, httplib::Response& res) {handle_get_expenses(req, res);});
    server.Get("/balances/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {handle_get_balances(req, res);});
    server.Get("/balances/(\\d+)/user/(\\d+)", [this](const httplib::Request& req, httplib::Response& res) {handle_get_user_balance(req, res);});
    server.Post("/settlements", [this](const httplib::Request& req, httplib::Response& res) {handle_settle_payment(req, res);});
    server.Get("/settlements", [this](const httplib::Request& req, httplib::Response& res) {handle_get_settlements(req, res);});
}

void ApiServer::handle_create_user(const httplib::Request& req, httplib::Response& res) {
    try {
        auto j = json::parse(req.body);
        auto user = db.create_user(j["name"], j["email"]);
        
        json response = {
            {"id", user.id},
            {"name", user.name},
            {"email", user.email}
        };
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 400;
        json error = {{"error", e.what()}};
        res.set_content(error.dump(), "application/json");
    }
}

void ApiServer::handle_get_users(const httplib::Request&, httplib::Response& res) {
    auto users = db.get_all_users();
    json j_array = json::array();
    
    for (const auto& user : users) {
        j_array.push_back({
            {"id", user.id},
            {"name", user.name},
            {"email", user.email}
        });
    }
    
    res.set_content(j_array.dump(), "application/json");
}

void ApiServer::handle_get_user(const httplib::Request& req, httplib::Response& res) {
    int id = std::stoi(req.matches[1]);
    auto user = db.get_user(id);
    
    if (user.has_value()) {
        json response = {
            {"id", user->id},
            {"name", user->name},
            {"email", user->email}
        };
        res.set_content(response.dump(), "application/json");
    } else {
        res.status = 404;
        json error = {{"error", "User not found"}};
        res.set_content(error.dump(), "application/json");
    }
}

void ApiServer::handle_create_group(const httplib::Request& req, httplib::Response& res) {
    try {
        auto j = json::parse(req.body);
        std::vector<int> member_ids = j["member_ids"].get<std::vector<int>>();
        
        for (int member_id : member_ids) {
            if (!db.get_user(member_id).has_value()) {
                res.status = 404;
                json error = {{"error", "User " + std::to_string(member_id) + " not found"}};
                res.set_content(error.dump(), "application/json");
                return;
            }
        }
        
        auto group = db.create_group(j["name"], member_ids);
        
        json response = {
            {"id", group.id},
            {"name", group.name},
            {"member_ids", group.member_ids}
        };
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 400;
        json error = {{"error", e.what()}};
        res.set_content(error.dump(), "application/json");
    }
}

void ApiServer::handle_get_groups(const httplib::Request&, httplib::Response& res) {
    auto groups = db.get_all_groups();
    json j_array = json::array();
    
    for (const auto& group : groups) {
        j_array.push_back({
            {"id", group.id},
            {"name", group.name},
            {"member_ids", group.member_ids}
        });
    }
    
    res.set_content(j_array.dump(), "application/json");
}

void ApiServer::handle_get_group(const httplib::Request& req, httplib::Response& res) {
    int id = std::stoi(req.matches[1]);
    auto group = db.get_group(id);
    
    if (group.has_value()) {
        json response = {
            {"id", group->id},
            {"name", group->name},
            {"member_ids", group->member_ids}
        };
        res.set_content(response.dump(), "application/json");
    } else {
        res.status = 404;
        json error = {{"error", "Group not found"}};
        res.set_content(error.dump(), "application/json");
    }
}

void ApiServer::handle_add_member(const httplib::Request& req, httplib::Response& res) {
    try {
        int group_id = std::stoi(req.matches[1]);
        auto j = json::parse(req.body);
        int user_id = j["user_id"];
        
        if (!db.get_user(user_id).has_value()) {
            res.status = 404;
            json error = {{"error", "User not found"}};
            res.set_content(error.dump(), "application/json");
            return;
        }
        
        if (db.add_member_to_group(group_id, user_id)) {
            json response = {{"message", "Member added successfully"}};
            res.set_content(response.dump(), "application/json");
        } else {
            res.status = 404;
            json error = {{"error", "Group not found"}};
            res.set_content(error.dump(), "application/json");
        }
    } catch (const std::exception& e) {
        res.status = 400;
        json error = {{"error", e.what()}};
        res.set_content(error.dump(), "application/json");
    }
}

void ApiServer::handle_add_expense(const httplib::Request& req, httplib::Response& res) {
    try {
        auto j = json::parse(req.body);
        
        int group_id = j["group_id"];
        std::string description = j["description"];
        double amount = j["amount"];
        int paid_by = j["paid_by"];
        std::string split_type_str = j["split_type"];
        
        SplitType split_type = (split_type_str == "equal") ? SplitType::EQUAL : SplitType::EXACT;
        
        std::map<int, double> custom_splits;
        if (j.contains("splits")) {
            custom_splits = j["splits"].get<std::map<int, double>>();
        }
        
        auto expense = expense_service.add_expense(group_id, description, amount, paid_by, split_type, custom_splits);
        
        json response = {
            {"id", expense.id},
            {"group_id", expense.group_id},
            {"description", expense.description},
            {"amount", expense.amount},
            {"paid_by", expense.paid_by},
            {"split_type", (expense.split_type == SplitType::EQUAL) ? "equal" : "exact"},
            {"splits", expense.splits}
        };
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 400;
        json error = {{"error", e.what()}};
        res.set_content(error.dump(), "application/json");
    }
}

void ApiServer::handle_get_expenses(const httplib::Request& req, httplib::Response& res) {
    std::optional<int> group_id = std::nullopt;
    if (req.has_param("group_id")) {
        group_id = std::stoi(req.get_param_value("group_id"));
    }
    
    auto expenses = db.get_expenses(group_id);
    json j_array = json::array();
    
    for (const auto& expense : expenses) {
        j_array.push_back({
            {"id", expense.id},
            {"group_id", expense.group_id},
            {"description", expense.description},
            {"amount", expense.amount},
            {"paid_by", expense.paid_by},
            {"split_type", (expense.split_type == SplitType::EQUAL) ? "equal" : "exact"},
            {"splits", expense.splits}
        });
    }
    res.set_content(j_array.dump(), "application/json");
}

void ApiServer::handle_get_balances(const httplib::Request& req, httplib::Response& res) {
    try {
        int group_id = std::stoi(req.matches[1]);
        
        if (!db.get_group(group_id).has_value()) {
            res.status = 404;
            json error = {{"error", "Group not found"}};
            res.set_content(error.dump(), "application/json");
            return;
        }
        
        auto balances = balance_service.calculate_balances(group_id);
        json response = json::object();
        
        for (const auto& [user_id, balance] : balances) {
            auto user = db.get_user(user_id);
            if (user.has_value()) {
                response[user->name] = std::round(balance * 100.0) / 100.0;
            }
        }
        
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 400;
        json error = {{"error", e.what()}};
        res.set_content(error.dump(), "application/json");
    }
}

void ApiServer::handle_get_user_balance(const httplib::Request& req, httplib::Response& res) {
    try {
        int group_id = std::stoi(req.matches[1]);
        int user_id = std::stoi(req.matches[2]);
        
        if (!db.get_group(group_id).has_value()) {
            res.status = 404;
            json error = {{"error", "Group not found"}};
            res.set_content(error.dump(), "application/json");
            return;
        }
        
        auto user = db.get_user(user_id);
        if (!user.has_value()) {
            res.status = 404;
            json error = {{"error", "User not found"}};
            res.set_content(error.dump(), "application/json");
            return;
        }
        
        double balance = balance_service.get_user_balance(group_id, user_id);
        
        json response = {
            {"user", user->name},
            {"balance", std::round(balance * 100.0) / 100.0},
            {"status", balance < 0 ? "owes" : (balance > 0 ? "gets back" : "settled")}
        };
        
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 400;
        json error = {{"error", e.what()}};
        res.set_content(error.dump(), "application/json");
    }
}

void ApiServer::handle_settle_payment(const httplib::Request& req, httplib::Response& res) {
    try {
        auto j = json::parse(req.body);
        
        int group_id = j["group_id"];
        int from_user = j["from_user"];
        int to_user = j["to_user"];
        double amount = j["amount"];
        
        if (!db.get_group(group_id).has_value()) {
            res.status = 404;
            json error = {{"error", "Group not found"}};
            res.set_content(error.dump(), "application/json");
            return;
        }
        
        if (!db.get_user(from_user).has_value() || !db.get_user(to_user).has_value()) {
            res.status = 404;
            json error = {{"error", "User not found"}};
            res.set_content(error.dump(), "application/json");
            return;
        }
        
        Settlement settlement{0, group_id, from_user, to_user, amount};
        auto result = db.create_settlement(settlement);
        
        json response = {
            {"id", result.id},
            {"group_id", result.group_id},
            {"from_user", result.from_user},
            {"to_user", result.to_user},
            {"amount", result.amount}
        };
        
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 400;
        json error = {{"error", e.what()}};
        res.set_content(error.dump(), "application/json");
    }
}

void ApiServer::handle_get_settlements(const httplib::Request& req, httplib::Response& res) {
    std::optional<int> group_id = std::nullopt;
    if (req.has_param("group_id")) {
        group_id = std::stoi(req.get_param_value("group_id"));
    }
    
    auto settlements = db.get_settlements(group_id);
    json j_array = json::array();
    
    for (const auto& settlement : settlements) {
        j_array.push_back({
            {"id", settlement.id},
            {"group_id", settlement.group_id},
            {"from_user", settlement.from_user},
            {"to_user", settlement.to_user},
            {"amount", settlement.amount}
        });
    }
    
    res.set_content(j_array.dump(), "application/json");
}

void ApiServer::run(const std::string& host, int port) {
    std::cout << "Starting Expense Splitting System API on " << host << ":" << port << std::endl;
    server.listen(host, port);
}