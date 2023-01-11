#include "request.h"
#include <QCryptographicHash>

// 注册登录
void Client::registration(const string& username, const string& nickname, const string& password){

}

void Client::login(const string& username, const string& password, Message m){
    auto ptr = object_message::create();
    std::string md5_password = QCryptographicHash::hash(password.c_str(), QCryptographicHash::Md5).toHex().toStdString();
    ptr->get_map()[std::string("username")] = string_message::create(username);
    ptr->get_map()[std::string("password")] = string_message::create(md5_password);
    this->socket()->emit("login", ptr,  [&](const message::list l) { m = l[0]->get_map(); });
}

// 获取服务器数据
void Client::getUserInformation(const int& user_id){

}
