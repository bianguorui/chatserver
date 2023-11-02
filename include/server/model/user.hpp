#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

// User表的ORM类
class User
{
public:
    User(int id = -1, string name = "", string password = "", string state = "offline")
    {
        this->m_id = id;
        this->m_name = name;
        this->m_password = password;
        this->m_state = state;
    }

    void setId(int id) { this->m_id = id; }
    void setName(string name) { this->m_name = name; }
    void setPassword(string password) { this->m_password = password; }
    void setState(string state) { this->m_state = state; }

    int getId() { return this->m_id; }
    string getName() { return this->m_name; }
    string getPassword() { return this->m_password; }
    string getState() { return this->m_state; }
private:
    int m_id;
    string m_name;
    string m_password;
    string m_state;
};
#endif