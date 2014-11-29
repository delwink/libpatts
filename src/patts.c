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

#include <string.h>
#include <stdlib.h>
#include "patts.h"

struct dbconn PATTSDB;
bool HAVE_ADMIN = false;
char userID[10];
size_t FMAXLEN = 0;

int patts_init(size_t qlen, size_t fmaxlen, const char *host, const char *user,
        const char *passwd, const char *database)
{
    cq_init(qlen, fmaxlen);
    FMAXLEN = fmaxlen;

    PATTSDB = cq_new_connection(host, user, passwd, database);

    char *condition = calloc(qlen, sizeof(char));
    if (condition == NULL) {
        return -2;
    }

    strcat(condition, u8"mysqlUser=");
    strcat(condition, user);

    struct dlist *puser = NULL;
    if (cq_select_all(PATTSDB, u8"User", &puser, condition)) {
        free(condition);
        return 1;
    }
    free(condition);

    if (cq_dlist_size(puser) != 1) {
        cq_free_dlist(puser);
        return 2;
    }

    size_t index;
    if (cq_field_to_index(puser, u8"id", &index)) {
        cq_free_dlist(puser);
        return 3;
    }

    strcpy(userID, puser->first->values[index]);

    if (cq_field_to_index(puser, u8"isAdmin", &index)) {
        cq_free_dlist(puser);
        return 4;
    }

    if (puser->first->values[index]) {
        HAVE_ADMIN = true;
    } else {
        HAVE_ADMIN = false;
    }

    cq_free_dlist(puser);
    return 0;
}

size_t patts_fmaxlen()
{
    return FMAXLEN;
}

struct dbconn patts_get_db()
{
    return PATTSDB;
}

const char *patts_get_user()
{
    return userID;
}

bool patts_have_admin()
{
    return HAVE_ADMIN;
}

const char *patts_interface()
{
    return u8"0.0";
}

const char *patts_version()
{
    return u8"0.0";
}
