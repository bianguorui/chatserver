#ifndef GROUP_H
#define GROUP_H

#include "groupuser.hpp"
#include <string>
#include <vector>
using namespace std;

// User表的ORM类
class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->m_id = id;
        this->m_name = name;
        this->m_desc = desc;
    }

    void setId(int id) { this->m_id = id; }
    void setName(string name) { this->m_name = name; }
    void setDesc(string desc) { this->m_desc = desc; }

    int getId() { return this->m_id; }
    string getName() { return this->m_name; }
    string getDesc() { return this->m_desc; }
    vector<GroupUser> &getUsers() { return this->m_users; }

private:
    int m_id;
    string m_name;
    string m_desc;
    vector<GroupUser> m_users;
};
#endif