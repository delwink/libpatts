SET character_set_client = utf8;

CREATE DATABASE IF NOT EXISTS pattsdb;

USE pattsdb;

CREATE TABLE IF NOT EXISTS Meta
(
	`version` INT UNSIGNED;
);

INSERT INTO Meta VALUES(1);

CREATE TABLE IF NOT EXISTS User
(
	`dbUser` VARCHAR(8) CHARACTER SET utf8 NOT NULL,
 	`state` BIT NOT NULL,
	`isAdmin` BIT NOT NULL,
	`firstName` VARCHAR(45) CHARACTER SET utf8 NOT NULL,
	`middleName` VARCHAR(45) CHARACTER SET utf8 NOT NULL,
	`lastName` VARCHAR(45) CHARACTER SET utf8 NOT NULL,
	PRIMARY KEY (`dbUser`)
);

CREATE TABLE IF NOT EXISTS TaskType
(
	`id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
	`state` BIT NOT NULL,
	`parentID` INT UNSIGNED NOT NULL,
	`displayName` VARCHAR(45) CHARACTER SET utf8 NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE IF NOT EXISTS TaskItem
(
	`id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
	`state` BIT NOT NULL,
	`onClock` BIT NOT NULL,
	`typeID` INT UNSIGNED NOT NULL,
	`userID` VARCHAR(8) NOT NULL,
	`startTime` DATETIME NOT NULL,
	`stopTime` DATETIME NULL,
	PRIMARY KEY(`id`)
);

DELIMITER //

CREATE PROCEDURE clockIn (taskID INT UNSIGNED, username VARCHAR(8))
BEGIN
	INSERT INTO TaskItem(state,onClock,startTime,typeID,userID)
	       VALUES(1,1,NOW(),taskID,username);
END//

CREATE PROCEDURE clockOut (taskID INT UNSIGNED)
BEGIN
	DECLARE @username VARCHAR(8);
	SELECT @username = userID FROM TaskItem WHERE id=taskID;

	UPDATE TaskItem SET onClock=0,stopTime=NOW()
	       WHERE id=taskID AND userID=@username;
END//

CREATE PROCEDURE createUser (
       newUser VARCHAR(8),
       host VARCHAR(45),
       passwd VARCHAR(45)
       )
BEGIN
	GRANT SELECT ON * TO newUser@host IDENTIFIED BY passwd;
	INSERT INTO User(state,isAdmin,dbUser,firstName,middleName,lastName)
	       VALUES(1,0,newUser,'','','');
	FLUSH PRIVILEGES;
END//

CREATE PROCEDURE grantAdmin (
       id VARCHAR(8),
       host VARCHAR(45),
       passwd VARCHAR(45)
       )
BEGIN
	GRANT EXECUTE ON PROCEDURE createUser TO id@host IDENTIFIED BY passwd;
	GRANT EXECUTE ON PROCEDURE grantAdmin TO id@host IDENTIFIED BY passwd;
	GRANT EXECUTE ON PROCEDURE revokeAdmin TO id@host IDENTIFIED BY passwd;
	GRANT INSERT,UPDATE ON TaskType TO id@host IDENTIFIED BY passwd;
	GRANT UPDATE ON User TO id@host IDENTIFIED BY passwd;
	UPDATE User SET isAdmin=1 WHERE dbUser=id;
	FLUSH PRIVILEGES;
END//

CREATE PROCEDURE revokeAdmin (
       id VARCHAR(8),
       host VARCHAR(45)
       )
BEGIN
	REVOKE EXECUTE ON PROCEDURE createUser FROM id@host;
	REVOKE EXECUTE ON PROCEDURE grantAdmin FROM id@host;
	REVOKE EXECUTE ON PROCEDURE revokeAdmin FROM id@host;
	REVOKE INSERT,UPDATE ON TaskType FROM id@host;
	REVOKE UPDATE ON User FROM id@host;
	UPDATE User SET isAdmin=0 WHERE dbUser=id;
	FLUSH PRIVILEGES;
END//

DELIMITER ;

CALL createUser('patts', '%', 'patts');
CALL grantAdmin('patts', '%', 'patts');
