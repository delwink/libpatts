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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "patts.h"
#include "get.h"

int patts_get_users(struct dlist **out)
{
    return cq_select_all(patts_get_db(), u8"User", out, u8"");
}

int patts_get_user_byid(struct dlist **out, const char *id)
{
    if (id == NULL || strlen(id) >= patts_fmaxlen() - strlen(u8"id="))
        return 1;

    char *s = calloc(patts_fmaxlen(), sizeof(char));
    if (s == NULL)
        return -1;

    strcat(s, u8"id=");
    strcat(s, id);

    int rc = cq_select_all(patts_get_db(), u8"User", out, s);
    free(s);

    return rc;
}

int patts_get_types(struct dlist **out)
{
    return cq_select_all(patts_get_db(), u8"TaskType", out, u8"");
}

int patts_get_type_byid(struct dlist **out, const char *id)
{
    if (id == NULL || strlen(id) >= patts_fmaxlen() - strlen(u8"id="))
        return 1;

    char *s = calloc(patts_fmaxlen(), sizeof(char));
    if (s == NULL)
        return -1;

    strcat(s, u8"id=");
    strcat(s, id);

    int rc = cq_select_all(patts_get_db(), u8"TaskType", out, s);
    free(s);

    return rc;
}

int patts_get_items(struct dlist **out)
{
    return cq_select_all(patts_get_db(), u8"TaskItem", out, u8"");
}

int patts_get_item_byid(struct dlist **out, const char *id)
{
    if (id == NULL || strlen(id) >= patts_fmaxlen() - strlen(u8"id="))
        return 1;

    char *s = calloc(patts_fmaxlen(), sizeof(char));
    if (s == NULL)
        return -1;

    strcat(s, u8"id=");
    strcat(s, id);

    int rc = cq_select_all(patts_get_db(), u8"TaskItem", out, s);
    free(s);

    return rc;
}

int patts_get_last_item(size_t *out, const char *userID)
{
    if (out == NULL || userID == NULL)
        return 1;

    struct dlist *items;
    int rc = patts_get_items_byuser(&items, userID);
    if (rc)
        return 100;

    size_t index;
    rc = cq_field_to_index(items, u8"id", &index);
    if (rc) {
        cq_free_dlist(items);
        return 101;
    }

    rc = sscanf(items->last->values[index], "%zu", out);
    cq_free_dlist(items);
    if (rc != 1)
        return 102;

    return 0;
}

int patts_get_items_byuser(struct dlist **out, const char *userID)
{
    if (userID == NULL
            || strlen(userID) >= patts_fmaxlen() - strlen(u8"userID="))
        return 1;

    char *s = calloc(patts_fmaxlen(), sizeof(char));
    if (s == NULL)
        return -1;

    strcat(s, u8"userID=");
    strcat(s, userID);

    int rc = cq_select_all(patts_get_db(), u8"TaskItem", out, s);
    free(s);

    return rc;
}

int patts_get_items_byuser_onclock(struct dlist **out, const char *userID)
{
    if (userID == NULL
            || (strlen(userID) >= patts_fmaxlen() 
                + strlen(u8"onClock=1,userID=")))
        return 1;

    char *s = calloc(patts_fmaxlen(), sizeof(char));
    if (s == NULL)
        return -1;

    strcat(s, u8"onClock=1,userID=");
    strcat(s, userID);

    int rc = cq_select_all(patts_get_db(), u8"TaskItem", out, s);
    free(s);

    return rc;
}
