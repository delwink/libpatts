CREATE USER '$username'@'%' identified by '$password';
GRANT INSERT, SELECT, UPDATE ON pattsdb.* TO '$username'@'%';
INSERT INTO pattsdb.User(state,isAdmin,MysqlUser,firstName,lastName) VALUES(1,$adminbit,'$username','$firstname','$lastname');
FLUSH PRIVILEGES;