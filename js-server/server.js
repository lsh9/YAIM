const { Server } = require("socket.io");
const io = new Server({ /* options */ });
const mysql = require('mysql');
const dbConfig = require('./database.js').dbConfig;
const dbError = require('./database.js').dbError;

const moment = require('moment');
moment.locale('zh-cn');
const username_to_socket = {};
const socketid_to_username = {};



io.on("connection", (socket) => {
	console.log("connection: " + "a user connected");

	// 连接数据库
	const db = mysql.createConnection(dbConfig);
	db.connect();

	// 连接断开
	socket.on("disconnect", () => {
		try {
			db.query("UPDATE user SET last_logout = ? WHERE username = ?", [moment().format('YYYY-MM-DD HH:mm:ss'), socketid_to_username[socket.id]], dbError);
			db.end();
			delete username_to_socket[socketid_to_username[socket.id]];
			delete socketid_to_username[socket.id];
		} catch (error) {
			console.log(error);
		}
		finally {
			console.log("disconnect: " + "a user disconnected");
		}
	});


	// 注册
	socket.on("sign up", (username, password, callback) => {
		console.log("sign up: " + username + " " + password);

		db.query("SELEct * FROM user WHERE username = ?", [username], (err, result) => {
			if (err) {
				console.log(err);
				callback(2);
			}
			if (result.length > 0) {
				callback(1);
			} else {
				db.query('INSERT INTO user (username, password) VALUES (?, ?)', [username, password], (err, result) => {
					if (err) {
						callback(2);
						throw err;
					}
					console.log(result);
				});
				callback(0);
			}
		});
	});

	// 登录
	socket.on("sign in", (username, password, callback) => {
		console.log(username, password);

		db.query('SELECT * FROM user WHERE username = ? AND password = ?', [username, password], (err, result) => {
			if (err) {
				callback(100);
				throw err;
			}
			if (result.length == 0) {	// 用户名或密码错误
				callback(2);
			} else if (username in username_to_socket) {	// 用户已登录
				callback(1);
			} else {	// 登录成功
				console.log("sign in: " + "success");
				callback(0);
			}
		});
	});

	// 上线
	socket.on("online", (username) => {
		username_to_socket[username] = socket;
		socketid_to_username[socket.id] = username;
		console.log("online: " + username);
	});

	// 下线
	socket.on("offline", (username) => {
		delete username_to_socket[username];
		console.log("offline: " + username);
	});

	// 客户端请求数据库同步到本地

	// 获取好友列表
	socket.on("get friends list", (username, callback) => {
		console.log("get friends list: ", username);
		db.query('SELECT * FROM relation WHERE owner = ?', [username], (err, result) => {
			if (err) {
				throw err;
			}
			callback(result);
		});
	});

	// 从服务器获取用户最近100条消息
	socket.on("get messages list", (username, callback) => {
		console.log("get messages list: ", username);
		db.query('SELECT * FROM message WHERE receiver = ? or sender = ? ORDER BY time ASC LIMIT 100', [username, username], (err, result) => {
			if (err) {
				throw err;
			}
			console.log(result);
			callback(result);
		});
	});



	// 用户点对点通信
	socket.on("send message", (msg, callback) => {
		sender = msg.sender;
		receiver = msg.receiver;
		content = msg.content;
		time = moment(Date.now()).format('YYYY-MM-DD HH:mm:ss');
		msg.time = time
		console.log("send: " + sender + " send " + content + " to " + receiver);

		// 保存到数据库
		db.query('INSERT INTO message (sender, receiver, content, time) VALUES (?, ?, ?, ?)', [sender, receiver, content, time], (err, result) => {
			if (err) {
				throw err;
			}
			msg.msgid = result.insertId;

			// 返回给发送者
			if (sender != receiver) {
				if (receiver in username_to_socket) {
					username_to_socket[receiver].emit("receive message", msg);
					console.log("emit receive message: from " + sender + " to " + receiver);
				}
				else {	// 接收方不在线
					db.query('UPDATE relation SET unread = unread + 1 WHERE owner = ? AND friend = ?', [receiver, sender], dbError);
					console.log("update unread: from" + sender + " to " + receiver);
				}
			}
			console.log("msgid: ", msg.msgid);
			callback({ msgid: msg.msgid, time: time });
		});
	});


	//============================================================
	// 申请添加好友
	socket.on("add friend", (username, friend, callback) => {
		console.log("add friend: ", username, friend);
		db.query('SELECT * FROM relation WHERE owner = ? AND friend = ?', [username, friend], (err, result) => {
			if (err) {
				throw err;
			}
			if (result.length > 0) {
				callback(1);
			} else {
				db.query('INSERT INTO relation (owner, friend) VALUES (?, ?)', [username, friend], (err, result) => {
					if (err) {
						throw err;
					}
					callback(0);
				});
			}
		});
	});

	// 同意添加好友
	socket.on("agree add friend", (username, friend, callback) => {
		console.log("agree add friend: ", username, friend);
		db.query('UPDATE relation SET status = 1 WHERE owner = ? AND friend = ?', [friend, username], (err, result) => {
			if (err) {
				throw err;
			}
			callback(0);
		});
	}
	);

	// 拒绝添加好友
	socket.on("refuse add friend", (username, friend, callback) => {
		console.log("refuse add friend: ", username, friend);
		db.query('DELETE FROM relation WHERE owner = ? AND friend = ?', [friend, username], (err, result) => {
			if (err) {
				throw err;
			}
			callback(0);
		});
	}
	);

	// 删除好友
	socket.on("delete friend", (username, friend, callback) => {
		console.log("delete friend: ", username, friend);
		db.query('DELETE FROM relation WHERE owner = ? AND friend = ?', [username, friend], (err, result) => {
			if (err) {
				throw err;
			}
			callback(0);
		});
	}
	);

	// 读取消息
	socket.on("read message", (username, friend, callback) => {
		console.log("read message: ", username, friend);
		db.query('UPDATE relation SET unread = 0 WHERE owner = ? AND friend = ?', [username, friend], (err, result) => {
			if (err) {
				throw err;
			}
			callback(0);
		});
	});

	// 读取所有消息
	socket.on("read all message", (username, callback) => {
		console.log("read all message: ", username);
		db.query('UPDATE relation SET unread = 0 WHERE owner = ?', [username], (err, result) => {
			if (err) {
				throw err;
			}
			callback(0);
		});
	});

	// // 用户下线
	// socket.on("disconnect", () => {
	// 	console.log("disconnect: ", socket.username);
	// 	delete username_to_socket[socket.username];
	// });

	// 错误处理
	socket.on("error", (err) => {
		console.log("socket error: ", err);
	});


});


io.listen(3000);
