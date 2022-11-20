dbConfig = {
	host: '1.117.242.95',
	user: 'yaim',
	password: 'yaim',
	database: 'yaim'
}

function dbError(err, result) {
	if (err) {
		throw err;
	}
}


module.exports.dbConfig = dbConfig;
module.exports.dbError = dbError;
