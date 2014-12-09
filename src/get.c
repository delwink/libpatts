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
    if (id == NULL || strlen(id) >= patts_qlen() - strlen(u8"id="))
        return 1;

    char *s = calloc(patts_qlen(), sizeof(char));
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
    if (id == NULL)
        return 1;

    char *s = calloc(patts_qlen(), sizeof(char));
    if (s == NULL)
        return -1;

    int rc = sprintf(s, "%s%s", u8"id=", id);
    if ((size_t)rc >= patts_qlen()) {
        free(s);
        return 2;
    }

    rc = cq_select_all(patts_get_db(), u8"TaskType", out, s);
    free(s);

    return rc;
}

int patts_get_child_types(struct dlist **out, const char *parent_id)
{
    if (parent_id == NULL)
        return 1;

    char *s = calloc(patts_qlen(), sizeof(char));
    if (s == NULL)
        return -1;

    int rc = sprintf(s, "%s%s", u8"parentID=", parent_id);
    if ((size_t)rc >= patts_qlen()) {
        free(s);
        return 2;
    }

    rc = cq_select_all(patts_get_db(), u8"TaskType", out, s);
    free(s);

    return rc;
}

int patts_get_items(struct dlist **out)
{
    return cq_select_all(patts_get_db(), u8"TaskItem", out, u8"");
}

int patts_get_item_byid(struct dlist **out, const char *id)
{
    if (id == NULL)
        return 1;

    char *s = calloc(patts_qlen(), sizeof(char));
    if (s == NULL)
        return -1;

    int rc = sprintf(s, "%s%s", u8"id=", id);
    if ((size_t)rc >= patts_qlen()) {
        free(s);
        return 2;
    }

    rc = cq_select_all(patts_get_db(), u8"TaskItem", out, s);
    free(s);

    return rc;
}

int patts_get_last_item(size_t *out, const char *user_id)
{
    if (out == NULL || user_id == NULL)
        return 1;

    struct dlist *items;
    int rc = patts_get_items_byuser(&items, user_id);
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

int patts_get_items_byuser(struct dlist **out, const char *user_id)
{
    if (user_id == NULL)
        return 1;

    char *s = calloc(patts_qlen(), sizeof(char));
    if (s == NULL)
        return -1;

    int rc = sprintf(s, "%s%s", u8"userID=", user_id);
    if ((size_t)rc >= patts_qlen()) {
        free(s);
        return 2;
    }

    rc = cq_select_all(patts_get_db(), u8"TaskItem", out, s);
    free(s);

    return rc;
}

int patts_get_items_byuser_onclock(struct dlist **out, const char *user_id)
{
    if (user_id == NULL)
        return 1;

    char *s = calloc(patts_qlen(), sizeof(char));
    if (s == NULL)
        return -1;

    int rc = sprintf(s, "%s%s", u8"onClock=1,userID=", user_id);
    if ((size_t)rc >= patts_qlen()) {
        free(s);
        return 2;
    }

    rc = cq_select_all(patts_get_db(), u8"TaskItem", out, s);
    free(s);

    return rc;
}

int patts_get_child_items(struct dlist **out, const char *id)
{
    if (id == NULL)
        return 1;

    char *s = calloc(patts_qlen(), sizeof(char));
    if (s == NULL)
        return -1;

    int rc = sprintf(s, "%s%s%s", 
            u8"typeID,userID,startTime,stopTime FROM TaskItem WHERE ", 
            u8"id=", id);
    if ((size_t)rc >= patts_qlen()) {
        free(s);
        return 2;
    }

    struct dlist *parent;
    rc = cq_select_query(patts_get_db(), &parent, s);
    if (rc) {
        free(s);
        return rc;
    }

    const char *typeID = parent->first->values[0],
            *user_id = parent->first->values[1],
            *startTime = parent->first->values[2],
            *stopTime = parent->first->values[3];

    struct dlist *child_types;
    rc = patts_get_child_types(&child_types, typeID);
    if (rc) {
        cq_free_dlist(parent);
        free(s);
        return rc;
    }

    size_t tid_index;
    rc = cq_field_to_index(child_types, u8"id", &tid_index);
    if (rc) {
        cq_free_dlist(parent);
        cq_free_dlist(child_types);
        free(s);
        return 100;
    }

    char *tids = calloc((patts_fmaxlen() + strlen(u8",") 
            + (strlen(u8"'") * 3)) 
            * cq_dlist_size(child_types), sizeof(char));
    if (tids == NULL) {
        cq_free_dlist(parent);
        cq_free_dlist(child_types);
        free(s);
        return -3;
    }

    for (struct drow *row = child_types->first; row != NULL; row = row->next) {
        strcat(tids, u8"'");
        strcat(tids, row->values[tid_index]);
        strcat(tids, u8"'");
        if (row->next != NULL)
            strcat(tids, u8",");
    }

    rc = sprintf(s, "%s%s%s%s%s%s%s%s%s%s%s", 
            u8"startTime>='", startTime, u8"',",
            u8"stopTime<='", stopTime, u8"',",
            u8"typeID IN (", tids, u8"),",
            u8"userID=", user_id);
    cq_free_dlist(parent);
    cq_free_dlist(child_types);
    free(tids);
    if ((size_t)rc >= patts_qlen()) {
        free(s);
        return 101;
    }

    rc = cq_select_all(patts_get_db(), u8"TaskItem", out, s);
    free(s);

    return rc;
}
