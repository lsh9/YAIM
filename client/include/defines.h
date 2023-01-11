#ifndef DEFINES_H
#define DEFINES_H

#include <sio_socket.h>

#define kURL "ws://localhost:3000"
#define kURL "ws://47.95.195.78:3000"

#define SERVER "ws://47.95.195.78:3000"
using namespace sio;
using ResponseObject = std::map<std::string, message::ptr>;

#define NORMAL 0
#define DATABASE_INTERNAL_ERROR 1
#define ACCOUNT_EXISTED_ERROR 1001
#define ACCOUNT_NOT_EXIST_ERROR 1002
#define ACCOUNT_PASSWORD_ERROR 1003
#define ACCOUNT_ALREADY_LOGIN_ERROR 1004

#endif	// DEFINES_H
