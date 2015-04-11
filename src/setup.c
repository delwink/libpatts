/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2015 Delwink, LLC
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, version 3 only.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <sqon.h>

#include "patts.h"
#include "setup.h"

#define LATEST_DB_VERSION "1"

int
patts_setup (uint8_t db_type, const char *host, const char *user,
	     const char *passwd, const char *database)
{
  int rc;
  sqon_dbsrv srv;
  const char *fmt;
  char *query, *esc_database;
  size_t qlen = 1;
  size_t lens[] = {
    strlen (host) * 2 + 1,
    strlen (user) * 2 + 1,
    strlen (passwd) * 2 + 1,
    strlen (database) * 2 + 1
  };

  esc_database = sqon_malloc (lens[3] * sizeof (char));
  if (NULL == esc_database)
    return PATTS_MEMORYERROR;

  sqon_init ();

  srv = sqon_new_connection (db_type, host, user, passwd, NULL);

  rc = sqon_escape (&srv, database, esc_database, lens[3], false);
  if (rc)
    {
      sqon_free (esc_database);
      return rc;
    }

  switch (db_type)
    {
    case SQON_DBCONN_MYSQL:
      fmt = "CREATE DATABASE %s";
      qlen += strlen (fmt) - 2;
      qlen += strlen (esc_database);

      query = sqon_malloc (qlen * sizeof (char));
      if (NULL == query)
	{
	  rc = PATTS_MEMORYERROR;
	  break;
	}

      snprintf (query, qlen, fmt, esc_database);

      rc = sqon_query (&srv, query, NULL, NULL);
      sqon_free (query);
      break;

    default:
      rc = PATTS_UNEXPECTED;
      break;
    }

  sqon_free (esc_database);

  if (rc)
    return rc;

  srv = sqon_new_connection (db_type, host, user, passwd, database);

  char *queries[] = {
    "CREATE TABLE Meta(version INT UNSIGNED)",
    "INSERT INTO Meta VALUES(" LATEST_DB_VERSION ")",

    "CREATE TABLE User"
    "("
    "dbUser VARCHAR(8) CHARACTER SET utf8 NOT NULL,"
    "state BIT NOT NULL,"
    "isAdmin BIT NOT NULL,"
    "firstName VARCHAR(45) CHARACTER SET utf8 NOT NULL,"
    "middleName VARCHAR(45) CHARACTER SET utf8 NOT NULL,"
    "lastName VARCHAR(45) CHARACTER SET utf8 NOT NULL,"
    "PRIMARY KEY (dbUser)"
    ")",

    "CREATE TABLE TaskType"
    "("
    "id INT UNSIGNED NOT NULL AUTO_INCREMENT,"
    "state BIT NOT NULL,"
    "parentID INT UNSIGNED NOT NULL,"
    "displayName VARCHAR(45) CHARACTER SET utf8 NOT NULL,"
    "PRIMARY KEY (id)"
    ")",

    "CREATE TABLE TaskItem"
    "("
    "id INT UNSIGNED NOT NULL AUTO_INCREMENT,"
    "state BIT NOT NULL,"
    "onClock BIT NOT NULL,"
    "typeID INT UNSIGNED NOT NULL,"
    "userID VARCHAR(8) NOT NULL,"
    "startTime DATETIME NOT NULL,"
    "stopTime DATETIME NULL,"
    "PRIMARY KEY (id)"
    ")",

    "CREATE PROCEDURE clockIn (taskID INT UNSIGNED, username VARCHAR(8)) "
    "BEGIN "
    "INSERT INTO TaskItem(state,onClock,startTime,typeID,userID) "
    "VALUES(1,1,CURRENT_TIMESTAMP,taskID,username);"
    "END",

    "CREATE PROCEDURE clockOut (taskID INT UNSIGNED) "
    "BEGIN "
    "DECLARE username VARCHAR(8);"
    "SELECT username = userID FROM TaskItem WHERE id=taskID;"
    "UPDATE TaskItem SET onClock=0,stopTime=CURRENT_TIMESTAMP "
    "WHERE id=taskID AND userID=username AND onClock=1;"
    "END",

    "CREATE PROCEDURE grantPermission (perm VARCHAR(30), "
    "target VARCHAR(30), id VARCHAR(8), host VARCHAR(45), passwd VARCHAR(45)) "
    "BEGIN "
    "SET @setPermissionCmd = CONCAT('GRANT ', perm, ' ON ', target, "
    "' TO ''', id, '''@''', host, ''' IDENTIFIED BY ''', passwd, ''';');"
    "PREPARE setPermissionStmt FROM @setPermissionCmd;"
    "EXECUTE setPermissionStmt;"
    "DEALLOCATE PREPARE setPermissionStmt;"
    "FLUSH PRIVILEGES;"
    "END",

    "CREATE PROCEDURE revokePermission (perm VARCHAR(30), "
    "target VARCHAR(30), id VARCHAR(8), host VARCHAR(45)) "
    "BEGIN "
    "SET @setPermissionCmd = CONCAT('REVOKE ', perm, ' ON ', target, "
    "' FROM ''', id, '''@''', host, ''';');"
    "PREPARE setPermissionStmt FROM @setPermissionCmd;"
    "EXECUTE setPermissionStmt;"
    "DEALLOCATE PREPARE setPermissionStmt;"
    "FLUSH PRIVILEGES;"
    "END",

    "CREATE PROCEDURE createUser (id VARCHAR(8), host VARCHAR(45), "
    "passwd VARCHAR(45)) "
    "BEGIN "
    "CALL grantPermission('SELECT', '*', id, host, passwd);"
    "INSERT INTO User(state,isAdmin,dbUser,firstName,middleName,lastName) "
    "VALUES(1,0,id,'','','');"
    "END",

    "CREATE PROCEDURE grantAdmin (id VARCHAR(8), host VARCHAR(45),"
    "passwd VARCHAR(45)) "
    "BEGIN "
    "SET max_sp_recursion_depth=255;"
    "CALL grantPermission('EXECUTE', 'PROCEDURE createUser', id, host,"
    "passwd);"
    "CALL grantPermission('EXECUTE', 'PROCEDURE grantAdmin', id, host,"
    "passwd);"
    "CALL grantPermission('EXECUTE', 'PROCEDURE revokeAdmin', id, host,"
    "passwd);"
    "CALL grantPermission('INSERT,UPDATE', 'TaskType', id, host, passwd);"
    "CALL grantPermission('UPDATE', 'User', id, host, passwd);"
    "UPDATE User SET isAdmin=1 WHERE dbUser=id;"
    "FLUSH PRIVILEGES;"
    "END",

    "CREATE PROCEDURE revokeAdmin (id VARCHAR(8), host VARCHAR(45)) "
    "BEGIN "
    "SET max_sp_recursion_depth=255;"
    "CALL revokePermission('EXECUTE', 'PROCEDURE createUser', id, host);"
    "CALL revokePermission('EXECUTE', 'PROCEDURE grantAdmin', id, host);"
    "CALL revokePermission('EXECUTE', 'PROCEDURE revokeAdmin', id, host);"
    "CALL revokePermission('INSERT,UPDATE', 'TaskType', id, host);"
    "CALL revokePermission('UPDATE', 'User', id, host);"
    "UPDATE User SET isAdmin=0 WHERE dbUser=id;"
    "FLUSH PRIVILEGES;"
    "END",

    "CALL createUser('patts', '%', 'patts')",

    "CALL grantAdmin('patts', '%', 'patts')",

    "UPDATE User SET firstName='Admin',middleName='User',lastName='Account' "
    "WHERE dbUser='patts'"
  };

  const size_t num_queries = 15;

  rc = sqon_connect (&srv);
  if (rc)
    return rc;

  size_t i;
  for (i = 0; i < num_queries; ++i)
    {
      rc = sqon_query (&srv, queries[i], NULL, NULL);

      if (rc)
	break;
    }

  sqon_close (&srv);

  return rc;
}
