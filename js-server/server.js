const { Server } = require("socket.io");
const io = new Server({ /* options */ });
const query = require('./database.js').query;
const callbackCode = require('./database.js').callbackCode;
const ERROR = require('./error.js');
const moment = require('moment');
const request = require('request');
moment.locale('zh-cn');



// 记录在线用户
var userid_to_socket = {};
var socketid_to_userid = {};



io.on("connection", (socket) => {
	console.log("connection: an anonymous user connected");


	// 连接断开
	socket.on("disconnect", () => {
		if (socket.id in socketid_to_userid) {
			query("UPDATE user SET last_logout = ?, is_online = ? WHERE user_id = ?", [moment().format('YYYY-MM-DD HH:mm:ss'), 0, socketid_to_userid[socket.id]]);
			delete userid_to_socket[socketid_to_userid[socket.id]];
			delete socketid_to_userid[socket.id];
			console.log("disconnect: ", socketid_to_userid[socket.id]);
		} else {
			console.log("disconnect: an anonymous user disconnected");
		}
	});


	// ===================================================================================
	// 									用户注册、登录、上线
	// ===================================================================================
	// 注册请求
	socket.on("register", (data, callback) => {
		username = data.username
		nickname = data.nickname
		password = data.password
		console.log("register: " + username + " " + nickname + " " + password);

		query("SELECT * FROM user WHERE username = ?", [username], (err, result) => {
			if (err) {
				callbackCode(callback, ERROR.DATABASE_INTERNAL_ERROR);
				return;
			}
			if (result.length > 0) {	// 已注册
				callbackCode(callback, ERROR.ACCOUNT_EXISTED_ERROR);
			} else {
				query('INSERT INTO user (username, nickname, password) VALUES (?, ?, ?)', [username, nickname, password], (err, result) => {
					if (err) {
						callbackCode(callback, ERROR.DATABASE_INTERNAL_ERROR);
						return;
					}
					console.log("register:", result.insertId);
					callback({
						code: ERROR.NORMAL,
						user_id: result.insertId
					});
				});
			}
		});
	});

	// 登录请求
	socket.on("login", (data, callback) => {
		username = data.username
		password = data.password
		console.log("login:", username, password);

		query('SELECT user_id, password FROM user WHERE username = ?', [username], (err, result) => {
			if (err) {
				callbackCode(callback, ERROR.DATABASE_INTERNAL_ERROR);
				return;
			}
			if (result.length == 0) {	// 用户名不存在
				callbackCode(callback, ERROR.ACCOUNT_NOT_EXIST_ERROR);
			} else if (result[0].password != password) {	// 密码错误
				callbackCode(callback, ERROR.ACCOUNT_PASSWORD_ERROR);
			} else if (result[0].user_id in userid_to_socket) {	// 用户已登录
				callbackCode(callback, ERROR.ACCOUNT_ALREADY_LOGIN_ERROR);
			} else {	// 登录成功
				console.log("login:", result[0].user_id);
				callback({
					code: ERROR.NORMAL,
					user_id: result[0].user_id,
				});
			}
		});
	});

	// 上线请求
	socket.on("online", (data, callback) => {
		user_id = data.user_id;
		console.log("online:", user_id);
		userid_to_socket[user_id] = socket;
		socketid_to_userid[socket.id] = user_id;
		query('SELECT * FROM user WHERE user_id = ?', [user_id], (err, result) => {
			data = result[0]
			delete data['password']
			data.code = ERROR.NORMAL
			console.log("online:", data);
			callback(data);
		});
		// 通知所有在线好友
		query('SELECT friend_id FROM friend WHERE user_id = ?', [user_id], (err, result) => {
			for (var i = 0; i < result.length; i++) {
				friend_id = result[i].friend_id
				if (friend_id in userid_to_socket) {
					userid_to_socket[friend_id].emit("friend online", {
						user_id: user_id
					});
				}
			}
		});
		// 更新数据库
		query('UPDATE user SET is_online = 1 WHERE user_id = ?', [user_id]);
	});


	//=============================================================================================
	// 									读取用户信息（个人、好友、群组、文件、消息）
	//=============================================================================================
	// 获取用户个人信息
	socket.on("get user information", (data, callback) => {
		user_id = data.user_id
		console.log("get user information: " + user_id);
		query('SELECT * FROM user WHERE user_id = ?', [user_id], (err, result) => {
			if (err) {
				callbackCode(callback, ERROR.DATABASE_INTERNAL_ERROR);
				return;
			}
			if (result.length == 0) {	// 用户不存在
				callbackCode(callback, ERROR.ACCOUNT_NOT_EXIST_ERROR);
			} else {
				data = result[0]
				delete data['password']
				data.code = ERROR.NORMAL
				callback(data);
			}
		});
	});

	// 获取好友列表
	socket.on("get friend list", (data, callback) => {
		user_id = data.user_id
		console.log("get friend list: ", user_id);
		query('SELECT * FROM relation WHERE owner = ?', [user_id], (err, result) => {
			console.log("get friend list: ", result);
			callback({ result: result });
		});
	});

	// 获取好友信息列表
	socket.on("get friend info list", (data, callback) => {
		user_id = data.user_id
		console.log("get friend info list: ", user_id);
		query('SELECT * FROM user WHERE user_id IN (SELECT friend FROM relation WHERE owner = ?)', [user_id], (err, result) => {
			console.log("get friend info list", result);
			callback({ result: result });
		});
	});

	// 获取群组信息列表
	socket.on("get group list", (data, callback) => {
		user_id = data.user_id
		console.log("get group list: ", user_id);
		query("SELECT * FROM `group` WHERE group_id IN (SELECT group_id FROM group_member WHERE user_id = ?)", [user_id], (err, result) => {
			console.log("get group list", result);
			callback({ result: result });
		});
	});

	// 获取文件信息列表
	socket.on("get file list", (data, callback) => {
		user_id = data.user_id
		console.log("get file list: ", user_id);
		query('SELECT * FROM file WHERE user_id = ?', [user_id], (err, result) => {
			console.log("get file list", result);
			callback({ result: result });
		});
	});


	// 获取所有消息
	socket.on("get message list", (data, callback) => {
		user_id = data.user_id
		console.log("get messages list: ", user_id);
		query('SELECT * FROM message WHERE from = ? or to = ?', [user_id, user_id], (err, result) => {
			console.log(result);
			callback(result);
		});
	});

	// 获取所有未读消息
	socket.on("get unread message list", (data, callback) => {
		user_id = data.user_id
		console.log("get unread messages list: ", user_id);
		query('SELECT * FROM message WHERE to = ? and is_read = 0', [user_id], (err, result) => {
			console.log(result);
			callback(result);
		});
	});

	// 获取群成员列表
	socket.on("get group members", (data, callback) => {
		group_id = data.group_id
		console.log("get group members: ", group_id);
		query('SELECT * FROM group_member WHERE group_id = ?', [group_id], (err, result) => {
			callback(result);
		});
	});

	// 获取在线好友列表
	socket.on("get online friend list", (data, callback) => {
		user_id = data.user_id
		console.log("get online friend list: ", user_id);
		query('SELECT * FROM relation WHERE owner = ?', [user_id], (err, result) => {
			console.log("get online friend list: ", result);




			//=============================================================================================
			// 									用户在线请求
			//=============================================================================================
			// 发送消息
			socket.on("send message", (data, callback) => {
				from_id = data.from_id
				to_id = data.to_id
				source = data.source
				type = data.type
				payload = data.payload
				time = moment(Date.now()).format('YYYY-MM-DD HH:mm:ss');
				data.time = time
				console.log("send message: ", from_id, to_id);

				// 保存到数据库
				query('INSERT INTO message (from_id, to_id, source, type, payload, time) VALUES (?, ?, ?, ?, ?, ?)', [from_id, to_id, source, type, payload, time], (err, result) => {
					if (err) {
						callbackCode(callback, ERROR.DATABASE_INTERNAL_ERROR);
						return;
					}
					data.msg_id = result.insertId;

					// 返回给发送者
					if (sender != receiver) {
						if (receiver in userid_to_socket) {
							userid_to_socket[receiver].emit("receive message", msg);
							console.log("emit receive message: from " + sender + " to " + receiver);
						}
						else {	// 接收方不在线
							query('UPDATE relation SET unread = unread + 1 WHERE owner = ? AND friend = ?', [receiver, sender], errorHandler);
							console.log("update unread: from " + sender + " to " + receiver);
						}
					}
					console.log("msgid: ", msg.msgid);
					callback({ msgid: msg.msgid, time: time });
				});
			});

			// ==============================好友请求==============================
			// 申请添加好友
			socket.on("request friend", (data, callback) => {
				user_id = data.user_id
				friend_id = data.friend_id
				console.log("request friend: ", user_id, friend_id);
				query('SELECT * FROM relation WHERE owner = ? AND friend = ?', [user_id, friend_id], (err, result) => {
					if (err) {
						callbackCode(callback, ERROR.DATABASE_INTERNAL_ERROR);
						return;
					}
					if (result.length > 0) {
						callbackCode(callback, ERROR.FRIEND_ALREADY_EXIST_ERROR);
					} else {
						query('INSERT INTO relation (owner, friend) VALUES (?, ?)', [user_id, friend_id], (err, result) => {
							errorHandler(err);
							callback(0);
						});
					}
				});
			});

			// 处理好友申请
			socket.on("deal friend apply", (data, callback) => {
				user_id = data.user_id
				friend_id = data.friend_id
				console.log("deal friend apply: ", user_id, friend_id);
				if (data.agree) {	// 同意添加好友
					query('INSERT INTO relation (owner, friend) VALUES (?, ?)', [friend_id, user_id], (err) => {
						if (err) {
							callbackCode(callback, ERROR.DATABASE_INTERNAL_ERROR);
							return;
						}
						callbackCode(callback, ERROR.NORMAL);
					});
				} else {	// 拒绝
					query('DELETE FROM relation WHERE owner = ? AND friend = ?', [user_id, friend_id], (err) => {
						if (err) {
							callbackCode(callback, ERROR.DATABASE_INTERNAL_ERROR);
							return;
						}
						callbackCode(callback, ERROR.NORMAL);
					});
				}
			});

			// 删除好友
			socket.on("delete friend", (data, callback) => {
				user_id = data.user_id
				friend_id = data.friend
				console.log("delete friend: ", user_id, friend_id);
				query('DELETE FROM relation WHERE owner = ? AND friend = ?', [user_id, friend], (err) => {
					errorHandler(err);
					callback(0);
				});
			});


			// ==============================群组请求==============================
			// 创建群组
			socket.on("create group", (data, callback) => {
				user_id = data.user_id
				group_name = data.group_name
				console.log("create group: ", user_id, group_name);
				query('INSERT INTO group (name) VALUES (?)', [group_name], (err, result) => {
					console.log("create group: ", result);
					callback({ group_id: result.insertId });
				});
			});

			// 加入群组


			// ==============================文件请求==============================
			socket.on("upload file", (data, callback) => {
				user_id = data.user_id
				filename = data.filename
				file = daa.file
				request.post("http://localhost:5000/upload file", { formData: { file: file, json: true } }, (err, res, body) => {
					console.log(body);
					url = body.url;
					callback({ url: url });
				});
			});

			// 读取消息
			socket.on("read message", (user_id, friend, callback) => {
				console.log("read message: ", user_id, friend);
				query('UPDATE relation SET unread = 0 WHERE owner = ? AND friend = ?', [user_id, friend], (err, result) => {
					errorHandler(err);
					callback(0);
				});
			});

			// 读取所有消息
			socket.on("read all message", (user_id, callback) => {
				console.log("read all message: ", user_id);
				query('UPDATE relation SET unread = 0 WHERE owner = ?', [user_id], (err, result) => {
					errorHandler(err);
					callback(0);
				});
			});

		});


		io.listen(3000);
