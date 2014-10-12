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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mysql.h>

#include "admin.h"

int patts_admin_create_user(patts_conn_Connection con, const char *firstName, 
        const char *lastName, const char *mysqlUser, const char *passwd)
{
    int rc;
    char *query;
    const size_t qlen = 256;
    const char *q1fmt = "GRANT SELECT ON * TO '%s'@'%%' "
                        "IDENTIFIED BY '%s';";
    const char *q2fmt = "GRANT INSERT,UPDATE ON TaskType TO '%s'@'%%' "
                        "IDENTIFIED BY '%s';";
    const char *q3fmt = "INSERT INTO User(state, isAdmin, firstName, lastName, "
                        "mysqlUser) VALUES (1,0,'%s','%s','%s');";

    if (strlen(firstName) > 45)
        return 1;
    if (strlen(lastName) > 45)
        return 2;
    if (strlen(mysqlUser) >= 64)
        return 3;
    if (strlen(passwd) >= 64)
        return 4;

    query = calloc(qlen, sizeof(char));
    if (query == NULL)
        return -1;

    rc = snprintf(query, qlen, q1fmt, mysqlUser, passwd);
    if ((size_t)rc >= qlen) {
        free(query);
        return 101;
    }

    rc = patts_conn_open(&con);
    if (rc) {
        free(query);
        return 200;
    }

    rc = mysql_query(con.con, query);
    if (rc) {
        free(query);
        patts_conn_close(&con);
        return 201;
    }

    rc = snprintf(query, qlen, q2fmt, mysqlUser, passwd);
    if ((size_t)rc >= qlen) {
        free(query);
        patts_conn_close(&con);
        return 102;
    }

    rc = mysql_query(con.con, query);
    if (rc) {
        free(query);
        patts_conn_close(&con);
        return 202;
    }

    rc = snprintf(query, qlen, q3fmt, firstName, lastName, mysqlUser);
    if ((size_t)rc >= qlen) {
        free(query);
        patts_conn_close(&con);
        return 103;
    }

    rc = mysql_query(con.con, query);
    if (rc) {
        free(query);
        patts_conn_close(&con);
        return 203;
    }

    free(query);
    patts_conn_close(&con);
    return 0;
}

int patts_admin_create_task(patts_conn_Connection con,
        const char *displayName, uint32_t parentID)
{
    int rc;
    char *query;
    const size_t qlen = 256;
    const char *fmt = "INSERT INTO TaskType(state, parentID, displayName) "
                        "VALUES(1,%d,'%s');";

    if (strlen(displayName) > 45)
        return 1;

    query = calloc(qlen, sizeof(char));
    if (query == NULL)
        return -1;

    rc = snprintf(query, qlen, fmt, parentID, displayName);
    if ((size_t)rc >= qlen) {
        free(query);
        return 101;
    }

    rc = patts_conn_open(&con);
    if (rc) {
        free(query);
        return 200;
    }

    rc = mysql_query(con.con, query);
    if (rc) {
        free(query);
        patts_conn_close(&con);
        return 201;
    }

    free(query);
    patts_conn_close(&con);
    return 0;
}
