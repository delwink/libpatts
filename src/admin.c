/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2014 Delwink, LLC
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation.
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
#include <stdlib.h>
#include <string.h>

#include "admin.h"
#include "get.h"
#include "patts.h"

int patts_create_user(struct dlist *info, const char *host,
        const char *passwd)
{
    int rc;
    struct dlist *newid = NULL;

    size_t index;
    rc = cq_field_to_index(info, u8"mysqlUser", &index);
    if (rc)
        return 1;

    const char *args[] = {
        info->first->values[index],
        host,
        passwd
    };

    rc = cq_select_func_arr(patts_get_db(), u8"createUser", args, 1, &newid);
    if (rc)
        return 2;

    rc = cq_field_to_index(info, u8"id", &index);
    if (rc) {
        cq_free_dlist(newid);
        return 3;
    }

    /* set info's id column to match return value of createUser */
    strcpy(info->first->values[index], newid->first->values[0]);
    cq_free_dlist(newid);

    return cq_update(patts_get_db(), u8"User", info);
}

int patts_create_task(struct dlist *info)
{
    int rc;
    size_t index;

    rc = cq_field_to_index(info, u8"id", &index);
    if (!rc) { /* need to remove primary key from insertion */
        rc = cq_dlist_remove_field_at(info, index);
        if (rc) /* something is very very wrong */
            return 1;
    }

    return cq_insert(patts_get_db(), u8"TaskType", info);
}

static int set_state(const char *table, const char *id, const char *state)
{
    if (id == NULL)
        return 1;
    if (patts_fmaxlen() < 2) /* impending buffer overflow */
        return -1;

    struct dlist *list;
    int rc = patts_get_user_byid(&list, id);
    if (rc)
        return rc;
    if (cq_dlist_size(list) != 1) {
        cq_free_dlist(list);
    }

    size_t index;
    rc = cq_field_to_index(list, u8"state", &index);
    if (rc) {
        cq_free_dlist(list);
        return rc;
    }

    strcpy(list->first->values[index], state);

    rc = cq_update(patts_get_db(), table, list);
    cq_free_dlist(list);

    return rc;
}

int patts_delete_user(const char *id)
{
    int rc;
    char *query;
    struct dlist *user = NULL;
    const char *fmt = "mysqlUser FROM User WHERE id=%s";

    query = calloc(patts_qlen(), sizeof(char));
    if (NULL == query)
        return -10;

    rc = snprintf(query, patts_qlen(), fmt, id);
    if (patts_qlen() <= (size_t) rc) {
        free(query);
        return 120;
    }

    rc = cq_select_query(patts_get_db(), &user, query);
    free(query);
    if (rc)
        return 121;

    rc = cq_revoke(patts_get_db(), u8"*", u8"*", user->first->values[0], u8"%",
            u8"");
    cq_free_dlist(user);
    if (rc)
        return 210;

    return set_state(u8"User", id, u8"0");
}

int patts_delete_task(const char *id)
{
    return set_state(u8"TaskType", id, u8"0");
}

int patts_grant_admin(const char *id)
{
    const char *func = u8"grantAdmin";
    const char *args[] = { id };

    return cq_proc_arr(patts_get_db(), func, args, 1);
}

int patts_revoke_admin(const char *id)
{
    const char *func = u8"revokeAdmin";
    const char *args[] = { id };

    return cq_proc_arr(patts_get_db(), func, args, 1);
}
