#ifndef REQUEST_H
#define REQUEST_H

#include "sio_client.h"

using namespace sio;
using std::string;
using Message = std::map<std::string, message::ptr>&;
using MessageList = std::vector<message::ptr>&;

class Client : public client{

    // 注册登录
    void registration(const string& username, const string& nickname, const string& password);
    void login(const string& username, const string& password, Message m);

    // 获取服务器数据
    void getUserInformation(const int& user_id);
};


#endif // REQUEST_H
