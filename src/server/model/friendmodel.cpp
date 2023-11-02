#include "friendmodel.hpp"
#include "db.h"

// 添加好友关系
void FriendModel::insert(int userId, int friendId)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values (%d, %d)", userId, friendId);

    MySQL mysql;

    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 返回用户好友列表
vector<User> FriendModel::query(int userId)
{
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.name, a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d", userId);
//  sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d", userid); 

    vector<User> vec;
    MySQL mysql;

    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);

        if (res != nullptr)
        {
            MYSQL_ROW row;

            // 把userid用户中的所有好友信息返回
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
        }
        
        mysql_free_result(res);
        return vec;
    }
    return vec;
}
