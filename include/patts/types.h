/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DELWINK_PATTS_TYPES_H
#define DELWINK_PATTS_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <mysql.h>

typedef struct {
    MYSQL_TYPE_LONG id,
    MYSQL_TYPE_TINY state,
    MYSQL_TYPE_BLOB firstName,
    MYSQL_TYPE_BLOB lastName,
    MYSQL_TYPE_BLOB mysqlUser
} patts_User;

typedef struct {
    MYSQL_TYPE_LONG id,
    MYSQL_TYPE_TINY state,
    MYSQL_TYPE_BLOB displayName
} patts_Task;

typedef struct {
    MYSQL_TYPE_LONG id,
    MYSQL_TYPE_TINY state,
    MYSQL_TYPE_BLOB firstName,
    MYSQL_TYPE_BLOB lastName
} patts_Client;

typedef struct {
    MYSQL_TYPE_LONG id,
    MYSQL_TYPE_TIMESTAMP startTime,
    MYSQL_TYPE_TIMESTAMP endTime,
    MYSQL_TYPE_LONG user,
    MYSQL_TYPE_LONG *subtasks,
    MYSQL_TYPE_LONG client
} patts_TaskItem;

#ifdef __cplusplus
}
#endif

#endif
