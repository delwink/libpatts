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

#ifndef DELWINK_PATTS_GET_H
#define DELWINK_PATTH_GET_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "conn.h"

typedef struct {
    patts_User *users;
    size_t len;
    size_t maxlen;
} patts_UserList;
int patts_list_user_init(patts_UserList *list, size_t maxlen);
int patts_list_user_cat(patts_UserList *dest, const patts_UserList *src);
void patts_list_user_close(patts_UserList *list);

typedef struct {
    patts_TaskType *types;
    size_t len;
    size_t maxlen;
} patts_TaskTypeList;
int patts_list_type_init(patts_TaskTypeList *list, size_t maxlen);
int patts_list_type_cat(patts_TaskTypeList *dest, 
        const patts_TaskTypeList *src);
void patts_list_type_close(patts_TaskTypeList *list);

typedef struct {
    patts_TaskItem *items;
    size_t len;
    size_t maxlen;
} patts_TaskItemList;
int patts_list_item_init(patts_TaskItemList *list, size_t maxlen);
int patts_list_item_cat(patts_TaskItemList *dest,
        const patts_TaskItemList *src);
void patts_list_item_close(patts_TaskItemList *list);

int patts_get_users(patts_conn_Connection con, patts_UserList *list);
int patts_get_user_byid(patts_conn_Connection con, patts_User *user,
        uint32_t id);

int patts_get_types(patts_conn_Connection con, patts_TaskTypeList *list);
int patts_get_type_byid(patts_conn_Connection con, patts_TaskType *type,
        uint32_t id);

int patts_get_items(patts_conn_Connection con, patts_TaskItemList *list);
int patts_get_item_byid(patts_conn_Connection con, patts_TaskItem *item,
        uint32_t id);
int patts_get_items_byuser(patts_conn_Connection con,
        patts_TaskItemList *list, uint32_t userID);
int patts_get_items_byuser_onclock(patts_conn_Connection con,
        patts_TaskItemList *list, uint32_t userID);

#ifdef __cplusplus
}
#endif

#endif
