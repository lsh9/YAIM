var mysql = require('mysql');
var pool = mysql.createPool({
	host: 'localhost',
	user: 'yaim',
	password: 'yaim',
	database: 'yaim'
});
pool.getConnection(function (err, conn) {
	conn.query("show tables", (qerr, result) => {
		conn.release();
		console.log(result);
	});
});