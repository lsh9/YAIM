var mysql = require('mysql');

var pool = mysql.createPool({
	host: 'localhost',
	user: 'yaim',
	password: 'yaim',
	database: 'yaim'
});


function callbackCode(callback, code, msg = {}) {
	msg.code = code;
	callback(msg);
};


function query(sql, params, callback) {
	pool.getConnection(function (err, conn) {
		if (err) {
			if (callback) {
				callback(err, null, null);
			}
		} else {
			conn.query(sql, params, function (qerr, vals, fields) {
				conn.release();
				if (callback) {
					callback(qerr, vals, fields);
				}
			});
		}
	});
};


async function asyncQuery(sql, values) {
	return new Promise((resolve, reject) => {
		pool.getConnection(function (err, connection) {
			if (err) {
				reject(err)
			} else {
				connection.query(sql, values, (err, rows) => {

					if (err) {
						reject(err)
					} else {
						resolve(rows)
					}
					connection.release()
				})
			}
		})
	})
}


module.exports = {
	query,
	asyncQuery,
	callbackCode
}

