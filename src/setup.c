/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2015-2016 Delwink, LLC
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

#include <inttypes.h>
#include <jansson.h>
#include <sqon.h>
#include <stdio.h>
#include <string.h>

#include "internal.h"
#include "patts.h"
#include "setup.h"

#define LATEST_DB_VERSION_NUM 5

#define TRY(Q) { rc = sqon_query (srv, (Q), NULL, NULL); if (rc) goto end; }

int
patts_setup (uint8_t db_type, const char *host, const char *user,
	     const char *passwd, const char *database, const char *port)
{
  int rc;
  sqon_DatabaseServer *srv;
  const char *fmt;
  char *esc_database;
  size_t qlen = 1;

  sqon_init ();

  srv = sqon_new_connection (db_type, host, user, passwd, NULL, port);
  if (NULL == srv)
    return PATTS_MEMORYERROR;

  rc = sqon_escape (srv, database, &esc_database, false);
  if (rc)
    return rc;

  switch (db_type)
    {
    case SQON_DBCONN_MYSQL:
      {
	fmt = "CREATE DATABASE %s";
	qlen += strlen (fmt) - 2;
	qlen += strlen (esc_database);
	char query[qlen];

	snprintf (query, qlen, fmt, esc_database);
	rc = sqon_query (srv, query, NULL, NULL);
      }
      break;

    default:
      rc = PATTS_UNEXPECTED;
      break;
    }

  sqon_free (esc_database);
  sqon_free_connection (srv);

  if (rc)
    return rc;

  srv = sqon_new_connection (db_type, host, user, passwd, database, port);
  if (NULL == srv)
    return PATTS_MEMORYERROR;

  char *queries[] = {
    "CREATE TABLE Meta(version INT UNSIGNED)",

    "INSERT INTO Meta VALUES(0)",

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
    "id SERIAL,"
    "state BIT NOT NULL,"
    "parentID INT UNSIGNED NOT NULL,"
    "displayName VARCHAR(45) CHARACTER SET utf8 NOT NULL,"
    "PRIMARY KEY (id)"
    ")",

    "CREATE TABLE TaskItem"
    "("
    "id SERIAL,"
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
    "UPDATE TaskItem SET onClock=0,stopTime=CURRENT_TIMESTAMP "
    "WHERE id=taskID AND onClock=1;"
    "END",

    "CREATE PROCEDURE grantPermissionSuffix (perm VARCHAR(30), "
    "target VARCHAR(30), id VARCHAR(8), host VARCHAR(45), suffix VARCHAR(45)) "
    "BEGIN "
    "SET @setPermissionCmd = CONCAT('GRANT ', perm, ' ON ', target, "
    "' TO ''', id, '''@''', host, ''' ', suffix, ';');"
    "PREPARE setPermissionStmt FROM @setPermissionCmd;"
    "EXECUTE setPermissionStmt;"
    "DEALLOCATE PREPARE setPermissionStmt;"
    "FLUSH PRIVILEGES;"
    "END",

    "CREATE PROCEDURE grantPermissionPasswd (perm VARCHAR(30), "
    "target VARCHAR(30), id VARCHAR(8), host VARCHAR(45), passwd VARCHAR(45)) "
    "BEGIN "
    "CALL grantPermissionSuffix(perm, target, id, host, "
    "CONCAT('IDENTIFIED BY ''', passwd, ''''));"
    "END",

    "CREATE PROCEDURE grantPermission(perm VARCHAR(30), "
    "target VARCHAR(30), id VARCHAR(8), host VARCHAR(45)) "
    "BEGIN "
    "CALL grantPermissionSuffix(perm, target, id, host, '');"
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
    "CALL grantPermissionPasswd('SELECT', '*', id, host, passwd);"
    "CALL grantPermissionPasswd('EXECUTE', 'PROCEDURE clockIn', id, host, "
    "passwd);"
    "CALL grantPermissionPasswd('EXECUTE', 'PROCEDURE clockOut', id, host, "
    "passwd);"
    "INSERT INTO User(state,isAdmin,dbUser,firstName,middleName,lastName) "
    "VALUES(1,0,id,'','','');"
    "END",

    "CREATE PROCEDURE grantAdmin (id VARCHAR(8), host VARCHAR(45)) "
    "BEGIN "
    "SET max_sp_recursion_depth=255;"
    "CALL grantPermission('EXECUTE', 'PROCEDURE createUser', id, host);"
    "CALL grantPermission('EXECUTE', 'PROCEDURE grantAdmin', id, host);"
    "CALL grantPermission('EXECUTE', 'PROCEDURE revokeAdmin', id, host);"
    "CALL grantPermission('INSERT,UPDATE', 'TaskType', id, host);"
    "CALL grantPermission('UPDATE', 'User', id, host);"
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

    "UPDATE Meta SET version=1",

    "CALL createUser('patts', '%', 'patts')",

    "CALL grantAdmin('patts', '%')",

    "UPDATE User SET firstName='Admin',middleName='User',lastName='Account' "
    "WHERE dbUser='patts'"
  };

  const size_t num_queries = 18;

  rc = sqon_connect (srv);
  if (rc)
    return rc;

  for (size_t i = 0; i < num_queries; ++i)
    {
      rc = sqon_query (srv, queries[i], NULL, NULL);
      if (rc)
	break;
    }

  sqon_free_connection (srv);
  if (rc)
    return rc;

  return patts_upgrade_db (db_type, host, user, passwd, database, port);
}

static int
refresh_users (sqon_DatabaseServer *srv)
{
  int rc;

  char *result_str;
  rc = sqon_query (srv, "SELECT dbUser,isAdmin FROM User WHERE state=1",
		   &result_str, NULL);
  if (rc)
    return rc;

  json_t *users = json_loads (result_str, 0, NULL);
  sqon_free (result_str);
  if (NULL == users)
    return PATTS_MEMORYERROR;

  json_t *value;
  size_t i;
  json_array_foreach (users, i, value)
    {
      const char *fmt = "CALL grantUserPerms('%s', '%%')";
      size_t qlen = strlen (fmt) + USERNAME_LEN * 2 - 2;
      char query[qlen];

      char *esc_user;
      json_t *user = json_object_get (value, "dbUser");
      rc = sqon_escape (srv, json_string_value (user), &esc_user, false);
      if (rc)
	break;

      snprintf (query, qlen, fmt, esc_user);
      rc = sqon_query (srv, query, NULL, NULL);
      if (rc)
	{
	  sqon_free (esc_user);
	  break;
	}

      if (json_string_value (json_object_get (value, "isAdmin"))[0] == 1)
	{
	  fmt = "CALL grantAdmin('%s', '%%')";
	  snprintf (query, qlen, fmt, esc_user);
	  rc = sqon_query (srv, query, NULL, NULL);
	}

      sqon_free (esc_user);
      if (rc)
	break;
    }

  json_decref (users);
  return rc;
}

int
patts_upgrade_db (uint8_t db_type, const char *host, const char *user,
		  const char *passwd, const char *database, const char *port)
{
  int rc;
  sqon_DatabaseServer *srv;

  srv = sqon_new_connection (db_type, host, user, passwd, database, port);
  if (NULL == srv)
    return PATTS_MEMORYERROR;

  rc = sqon_connect (srv);
  if (rc)
    goto end;

  char *results;
  rc = sqon_query (srv, "SELECT version FROM Meta LIMIT 1", &results, NULL);
  if (rc)
    goto end;

  uint32_t version;
  rc = sscanf (results, "[{\"version\": \"%" SCNu32 "\"}]", &version);
  sqon_free (results);
  if (rc != 1)
    {
      rc = PATTS_UNEXPECTED;
      goto end;
    }

  switch (version)
    {
    case 1:
      TRY ("UPDATE Meta SET version=0");

      TRY ("DROP PROCEDURE clockIn");

      TRY ("CREATE PROCEDURE clockIn (taskID INT UNSIGNED,"
	   "username VARCHAR(16)) "
	   "BEGIN "
	   "SELECT typeID INTO @cType FROM TaskItem WHERE stopTime IS NULL "
	   "AND userID=username ORDER BY id DESC LIMIT 1;"
	   "SELECT parentID INTO @cParent FROM TaskType WHERE id=taskID;"
	   "IF @cParent IS NOT NULL "
	   "AND (@cType IS NULL AND @cParent=0) OR @cType=@cParent THEN "
	   "INSERT INTO TaskItem(state,startTime,typeID,userID) "
	   "VALUES(1,CURRENT_TIMESTAMP,taskID,username);"
	   "ELSE "
	   "SIGNAL SQLSTATE 'HY000' SET MYSQL_ERRNO = 1210;"
	   "END IF;"
	   "END");

      TRY ("DROP PROCEDURE clockOut");

      TRY ("CREATE PROCEDURE clockOut (taskID INT UNSIGNED) "
	   "BEGIN "
	   "SELECT startTime,stopTime,userID INTO @tStart,@tStop,@tUser "
	   "FROM TaskItem WHERE id=taskID;"
	   "IF @tStop IS NULL THEN "
	   "UPDATE TaskItem SET stopTime=CURRENT_TIMESTAMP "
	   "WHERE startTime>=@tStart AND stopTime IS NULL AND userID=@tUser;"
	   "END IF;"
	   "END");

      TRY ("CREATE PROCEDURE changePermissionSuffix (type VARCHAR(7), "
	   "perm VARCHAR(30), target VARCHAR(30), id VARCHAR(16), "
	   "host VARCHAR(45), suffix VARCHAR(45)) "
	   "BEGIN "
	   "IF type='GRANT' THEN "
	   "SET @prep = ' TO ''';"
	   "ELSE "
	   "SET @prep = ' FROM ''';"
	   "END IF;"
	   "SET @cmd = CONCAT(type, ' ', perm, ' ON ', target, @prep, id, "
	   "'''@''', host, ''' ', suffix, ';');"
	   "PREPARE stmt FROM @cmd;"
	   "EXECUTE stmt;"
	   "DEALLOCATE PREPARE stmt;"
	   "FLUSH PRIVILEGES;"
	   "END");

      TRY ("DROP PROCEDURE grantPermissionSuffix");

      TRY ("DROP PROCEDURE grantPermissionPasswd");

      TRY ("CREATE PROCEDURE grantPermissionPasswd (perm VARCHAR(30), "
	   "target VARCHAR(30), id VARCHAR(16), host VARCHAR(45), "
	   "passwd VARCHAR(45)) "
	   "BEGIN "
	   "CALL changePermissionSuffix('GRANT', perm, target, id, host, "
	   "CONCAT('IDENTIFIED BY ''', passwd, ''''));"
	   "END");

      TRY ("DROP PROCEDURE grantPermission");

      TRY ("CREATE PROCEDURE grantPermission (perm VARCHAR(30), "
	   "target VARCHAR(30), id VARCHAR(16), host VARCHAR(45)) "
	   "BEGIN "
	   "CALL changePermissionSuffix('GRANT', perm, target, id, host, '');"
	   "END");

      TRY ("DROP PROCEDURE revokePermission");

      TRY ("CREATE PROCEDURE revokePermission (perm VARCHAR(30), "
	   "target VARCHAR(30), id VARCHAR(16), host VARCHAR(45)) "
	   "BEGIN "
	   "CALL changePermissionSuffix('REVOKE', perm, target, id, host, '');"
	   "END");

      TRY ("CREATE PROCEDURE grantUserPermsPasswd (id VARCHAR(16), "
	   "host VARCHAR(45), passwd VARCHAR(45)) "
	   "BEGIN "
	   "CALL grantPermissionPasswd('SELECT', '*', id, host, passwd);"
	   "CALL grantPermissionPasswd('EXECUTE', 'PROCEDURE clockIn', id, "
	   "host, passwd);"
	   "CALL grantPermissionPasswd('EXECUTE', 'PROCEDURE clockOut', id, "
	   "host, passwd);"
	   "END");

      TRY ("CREATE PROCEDURE grantUserPerms (id VARCHAR(16), "
	   "host VARCHAR(45)) "
	   "BEGIN "
	   "CALL grantPermission('SELECT', '*', id, host);"
	   "CALL grantPermission('EXECUTE', 'PROCEDURE clockIn', id, host);"
	   "CALL grantPermission('EXECUTE', 'PROCEDURE clockOut', id, host);"
	   "END");

      TRY ("CREATE PROCEDURE revokeUserPerms (id VARCHAR(16), "
	   "host VARCHAR(45)) "
	   "BEGIN "
	   "CALL revokePermission('SELECT', '*', id, host);"
	   "CALL revokePermission('EXECUTE', 'PROCEDURE clockIn', id, host);"
	   "CALL revokePermission('EXECUTE', 'PROCEDURE clockOut', id, host);"
	   "END");

      TRY ("DROP PROCEDURE createUser");

      TRY ("DROP PROCEDURE grantAdmin");

      TRY ("CREATE PROCEDURE grantAdmin (id VARCHAR(16), host VARCHAR(45)) "
	   "BEGIN "
	   "SET max_sp_recursion_depth=255;"
	   "CALL grantPermission('EXECUTE', 'PROCEDURE createUser', id, host);"
	   "CALL grantPermission('EXECUTE', 'PROCEDURE deleteUser', id, host);"
	   "CALL grantPermission('EXECUTE', 'PROCEDURE grantAdmin', id, host);"
	   "CALL grantPermission('EXECUTE', 'PROCEDURE revokeAdmin', id, "
	   "host);"
	   "CALL grantPermission('INSERT,UPDATE', 'TaskType', id, host);"
	   "CALL grantPermission('UPDATE', 'User', id, host);"
	   "UPDATE User SET isAdmin=1 WHERE dbUser=id;"
	   "END");

      TRY ("DROP PROCEDURE revokeAdmin");

      TRY ("CREATE PROCEDURE revokeAdmin (id VARCHAR(16), host VARCHAR(45)) "
	   "BEGIN "
	   "SET max_sp_recursion_depth=255;"
	   "CALL revokePermission('EXECUTE', 'PROCEDURE createUser', id, "
	   "host);"
	   "CALL revokePermission('EXECUTE', 'PROCEDURE deleteUser', id, "
	   "host);"
	   "CALL revokePermission('EXECUTE', 'PROCEDURE grantAdmin', id, "
	   "host);"
	   "CALL revokePermission('EXECUTE', 'PROCEDURE revokeAdmin', id, "
	   "host);"
	   "CALL revokePermission('INSERT,UPDATE', 'TaskType', id, host);"
	   "CALL revokePermission('UPDATE', 'User', id, host);"
	   "UPDATE User SET isAdmin=0 WHERE dbUser=id;"
	   "END");

      TRY ("CREATE PROCEDURE createUser (id VARCHAR(16), "
	   "host VARCHAR(45), passwd VARCHAR(45)) "
	   "BEGIN "
	   "CALL grantUserPermsPasswd(id, host, passwd);"
	   "IF id NOT IN (SELECT dbUser FROM User) THEN "
	   "INSERT INTO User(state,isAdmin,dbUser,firstName,middleName,"
	   "lastName) VALUES(1,0,id,'','','');"
	   "ELSE "
	   "UPDATE User SET state=1 WHERE dbUser=id;"
	   "END IF;"
	   "END");

      TRY ("CREATE PROCEDURE deleteUser (id VARCHAR(16)) "
	   "BEGIN "
	   "CALL revokeAdmin(id, '%');"
	   "CALL revokeUserPerms(id, '%');"
	   "UPDATE User SET state=0 WHERE dbUser=id;"
	   "END");

      TRY ("ALTER TABLE TaskItem DROP COLUMN onClock");

      TRY ("ALTER TABLE User MODIFY dbUser VARCHAR(16)");

      TRY ("UPDATE Meta SET version=2");

    case 2:
      TRY ("UPDATE Meta SET version=0");

      TRY ("DROP PROCEDURE clockIn");

      TRY ("CREATE PROCEDURE clockIn (taskID INT UNSIGNED,"
	   "username VARCHAR(16)) "
	   "BEGIN "
	   "SELECT typeID INTO @cType FROM TaskItem WHERE stopTime IS NULL "
	   "AND userID=username AND state=1 ORDER BY id DESC LIMIT 1;"
	   "SELECT parentID INTO @cParent FROM TaskType WHERE id=taskID "
	   "AND state=1;"
	   "IF @cParent IS NOT NULL "
	   "AND (@cType IS NULL AND @cParent=0) OR @cType=@cParent THEN "
	   "INSERT INTO TaskItem(state,startTime,typeID,userID) "
	   "VALUES(1,CURRENT_TIMESTAMP,taskID,username);"
	   "ELSE "
	   "SIGNAL SQLSTATE 'HY000' SET MYSQL_ERRNO = 1210;"
	   "END IF;"
	   "END");

      TRY ("DROP PROCEDURE deleteUser");

      TRY ("CREATE PROCEDURE deleteUser (id VARCHAR(16)) "
	   "BEGIN "
	   "IF id IN (SELECT dbUser FROM User WHERE isAdmin=1) THEN "
	   "CALL revokeAdmin(id, '%');"
	   "END IF;"
	   "CALL revokeUserPerms(id, '%');"
	   "UPDATE User SET state=0 WHERE dbUser=id;"
	   "END");

    case 3:
      TRY ("UPDATE Meta SET version=0");

      TRY ("ALTER TABLE TaskItem MODIFY userID VARCHAR(16)");

    case 4:
      TRY ("UPDATE Meta SET version=0");

      TRY ("DROP PROCEDURE clockIn");

      TRY ("CREATE PROCEDURE clockIn (taskID INT UNSIGNED) "
	   "BEGIN "
	   "SELECT SUBSTRING_INDEX(USER(), '@', 1) INTO @user;"
	   "SELECT typeID INTO @cType FROM TaskItem WHERE stopTime IS NULL "
	   "AND userID=@user AND state=1 ORDER BY id DESC LIMIT 1;"
	   "SELECT parentID INTO @cParent FROM TaskType WHERE id=taskID "
	   "AND state=1;"
	   "IF @cParent IS NOT NULL "
	   "AND (@cType IS NULL AND @cParent=0) OR @cType=@cParent THEN "
	   "INSERT INTO TaskItem(state,startTime,typeID,userID) "
	   "VALUES(1,CURRENT_TIMESTAMP,taskID,@user);"
	   "ELSE "
	   "SIGNAL SQLSTATE 'HY000' SET MYSQL_ERRNO = 1210;"
	   "END IF;"
	   "END");

      TRY ("UPDATE Meta SET version=5");

      rc = refresh_users (srv);
      if (rc)
	goto end;

    case LATEST_DB_VERSION_NUM:
      rc = 0;
      break;

    default:
      rc = PATTS_UNEXPECTED;
      goto end;
    }

 end:
  sqon_free_connection (srv);
  return rc;
}

int
patts_version_check (int64_t *out)
{
  int rc;
  uint32_t installed_version;

  rc = patts_get_db_version (&installed_version);
  if (rc)
    return rc;

  *out = LATEST_DB_VERSION_NUM - installed_version;
  return 0;
}
