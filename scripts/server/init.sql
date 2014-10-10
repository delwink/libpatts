CREATE DATABASE IF NOT EXISTS pattsdb;

CREATE TABLE `pattsdb`.`User`
(
	`id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
 	`state` BIT NOT NULL,
	`isAdmin` BIT NOT NULL,
	`firstName` VARCHAR(45) NOT NULL,
	`lastName` VARCHAR(45) NOT NULL,
	`mysqlUser` VARCHAR(45) NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `pattsdb`.`TaskType` 
(
	`id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
	`state` BIT NOT NULL,
	`parentID` INT UNSIGNED NOT NULL,
	`displayName` VARCHAR(45) NOT NULL,
	PRIMARY KEY (`id`)
);

CREATE TABLE `pattsdb`.`TaskItem` 
(
	`id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
	`state` BIT NOT NULL,
	`typeID` INT UNSIGNED NOT NULL,
	`onClock` BIT NOT NULL,
	`userID` INT UNSIGNED NOT NULL,
	`startTime` DATETIME NOT NULL,
	`stopTime` DATETIME NULL,
	PRIMARY KEY(`id`)
);

CREATE USER 'patts'@'%' IDENTIFIED BY 'patts';

GRANT ALL PRIVILEGES ON *.* TO 'patts'@'%' WITH GRANT OPTION;

FLUSH PRIVILEGES;

INSERT INTO pattsdb.User(state, isAdmin, firstName, lastName, mysqlUser) VALUES(1,1,'Patts','Admin','patts');

