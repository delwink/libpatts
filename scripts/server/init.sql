CREATE DATABASE IF NOT EXISTS pattsdb;
CREATE TABLE `pattsdb`.`User` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `state` BIT NOT NULL,
  `isAdmin` BIT NOT NULL,
  `firstName` VARCHAR(45) NOT NULL,
  `lastName` VARCHAR(45) NOT NULL,
  `mysqlUser` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`id`));
CREATE TABLE `pattsdb`.`Client` (
  `id` INT SIGNED NOT NULL AUTO_INCREMENT,
  `state` BIT NOT NULL,
  `firstName` VARCHAR(45) NOT NULL,
  `lastName` VARCHAR(45) NOT NULL,
  PRIMARY KEY (`id`));
CREATE TABLE `pattsdb`.`TaskType` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `state` BIT NOT NULL,
  `parentID` INT UNSIGNED,
  `displayName` VARCHAR(45),
  PRIMARY KEY (`id`));
CREATE TABLE `pattsdb`.`TaskItem` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `state` BIT NOT NULL,
  `typeID` INT UNSIGNED NOT NULL,
  `onClock` BIT NOT NULL,
  `userID` INT UNSIGNED NOT NULL,
  `clientID` INT SIGNED NOT NULL,
  `startTime` DATETIME NOT NULL,
  `endTime` DATETIME NULL,
  PRIMARY KEY(`id`));