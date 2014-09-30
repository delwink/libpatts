
function create_user($username, $password, $firstname, $lastname)
	{
	Create user '$username'@'%' identified by '$password';

	grant insert, select, update on pattsdb.* to '$username'@'%';

	insert into pattsdb.User(State,MySQLUser,FirstName,LastName) values(1,'$username','$firstname','$lastname');

	flush privileges;

	}