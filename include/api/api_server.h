#pragma once
#include "../database/database.h"
#include "../services/expense_service.h"
#include "../services/balance_service.h"
#include <httplib.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ApiServer {
private:
    Database db;
    ExpenseService expense_service;
    BalanceService balance_service;
    httplib::Server server;
    
    void setup_routes();
    
    // Route handlers
    void handle_create_user(const httplib::Request& req, httplib::Response& res);
    void handle_get_users(const httplib::Request& req, httplib::Response& res);
    void handle_get_user(const httplib::Request& req, httplib::Response& res);
    
    void handle_create_group(const httplib::Request& req, httplib::Response& res);
    void handle_get_groups(const httplib::Request& req, httplib::Response& res);
    void handle_get_group(const httplib::Request& req, httplib::Response& res);
    void handle_add_member(const httplib::Request& req, httplib::Response& res);
    
    void handle_add_expense(const httplib::Request& req, httplib::Response& res);
    void handle_get_expenses(const httplib::Request& req, httplib::Response& res);
    
    void handle_get_balances(const httplib::Request& req, httplib::Response& res);
    void handle_get_user_balance(const httplib::Request& req, httplib::Response& res);
    
    void handle_settle_payment(const httplib::Request& req, httplib::Response& res);
    void handle_get_settlements(const httplib::Request& req, httplib::Response& res);
    
public:
    ApiServer() : expense_service(db), balance_service(db) {
        setup_routes();
    }
    
    void run(const std::string& host = "0.0.0.0", int port = 8000);
};
