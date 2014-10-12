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

#include "get.h"
#include "conn.h"

int patts_list_user_init(patts_UserList *list, size_t maxlen)
{
    list->users = calloc(maxlen, sizeof(patts_User));
    if (list->users == NULL)
        return -1;
    list->len = 0;
    list->maxlen = maxlen;
    return 0;
}

int patts_list_user_cat(patts_UserList *dest, const patts_UserList *src)
{
    const size_t newlen = dest->len + src->len;
    const size_t oldlen = dest->len;

    if (newlen > dest->maxlen)
        return 1;

    for (size_t i = oldlen; i < newlen; ++i) {
        dest->users[i] = src->users[i-oldlen];
    }

    dest->len = newlen;
    return 0;
}

void patts_list_user_close(patts_UserList *list)
{
    free(list->users);
    list->len = 0;
    list->maxlen = 0;
}

int patts_list_type_init(patts_TaskTypeList *list, size_t maxlen)
{
    list->types = calloc(maxlen, sizeof(patts_TaskType));
    if (list->types == NULL)
        return -1;
    list->len = 0;
    list->maxlen = maxlen;
    return 0;
}

int patts_list_type_cat(patts_TaskTypeList *dest,
        const patts_TaskTypeList *src)
{
    const size_t newlen = dest->len + src->len;
    const size_t oldlen = dest->len;

    if (newlen > dest->maxlen)
        return 1;

    for (size_t i = oldlen; i < newlen; ++i) {
        dest->types[i] = src->types[i-oldlen];
    }

    dest->len = newlen;
    return 0;
}

void patts_list_type_close(patts_TaskTypeList *list)
{
    free(list->types);
    list->len = 0;
    list->maxlen = 0;
}

int patts_list_item_init(patts_TaskItemList *list, size_t maxlen)
{
    list->items = calloc(maxlen, sizeof(patts_TaskItem));
    if (list->items == NULL)
        return -1;
    list->len = 0;
    list->maxlen = maxlen;
    return 0;
}

int patts_list_item_cat(patts_TaskItemList *dest,
        const patts_TaskItemList *src)
{
    const size_t newlen = dest->len + src->len;
    const size_t oldlen = dest->len;

    if (newlen > dest->maxlen)
        return 1;

    for (size_t i = oldlen; i < newlen; ++i) {
        dest->items[i] = src->items[i-oldlen];
    }

    dest->len = newlen;
    return 0;
}

void patts_list_item_close(patts_TaskItemList *list)
{
    free(list->items);
    list->len = 0;
    list->maxlen = 0;
}

int patts_get_users(patts_conn_Connection con, patts_UserList *list)
{
    int rc;

    rc = patts_conn_open(&con);
    if (rc)
        return 200;

    rc = mysql_query(con.con, "SELECT * FROM User;");
    if (rc) {
        patts_conn_close(&con);
        return 201;
    }

    MYSQL_RES *selection = mysql_store_result(con.con);
    if (selection == NULL) {
        patts_conn_close(&con);
        return -1;
    }

    size_t nfields = mysql_num_fields(selection);
    if (nfields == 0) { /* nothing to add to the list; job well done */
        patts_conn_close(&con);
        mysql_free_result(selection);
        return 0;
    }
    if (nfields > list->maxlen) { /* impending buffer overflow */
        patts_conn_close(&con);
        mysql_free_result(selection);
        return 1;
    }

    MYSQL_ROW row;
    int i = 0;
    while ((row = mysql_fetch_row(selection))) {
        list->users[i].id = (uint32_t) atoi((char *)row[0]);
        list->users[i].state = (bool) atoi((char *)row[1]);
        list->users[i].isAdmin = (bool) atoi((char *)row[2]);
        list->users[i].firstName = (char *)row[3];
        list->users[i].lastName = (char *)row[4];
        list->users[i].mysqlUser = (char *)row[5];
        ++i;
    }

    list->len = i+1;

    patts_conn_close(&con);
    mysql_free_result(selection);
    return 0;
}

int patts_get_user_byid(patts_conn_Connection con, patts_User *user,
        uint32_t id)
{
    int rc;
    char *query;
    const size_t qlen = 64;
    const char *fmt = "SELECT * FROM User WHERE id=%d";

    query = calloc(qlen, sizeof(char));
    if (query == NULL)
        return -1;

    rc = snprintf(query, qlen, fmt, id);
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

    MYSQL_RES *selection = mysql_store_result(con.con);
    if (selection == NULL) {
        patts_conn_close(&con);
        return -1;
    }

    size_t nfields = mysql_num_fields(selection);
    if (nfields == 0) { /* no such user id */
        patts_conn_close(&con);
        mysql_free_result(selection);
        return 1;
    }
    if (nfields > 1) { /* duplicate users; panic */
        patts_conn_close(&con);
        mysql_free_result(selection);
        return 2;
    }

    MYSQL_ROW row = mysql_fetch_row(selection);

    user->id = (uint32_t) atoi((char *)row[0]);
    user->state = (bool) atoi((char *)row[1]);
    user->isAdmin = (bool) atoi((char *)row[2]);
    user->firstName = (char *)row[3];
    user->lastName = (char *)row[4];
    user->mysqlUser = (char *)row[5];

    patts_conn_close(&con);
    mysql_free_result(selection);
    return 0;
}
