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

#include "admin.h"
#include "get.h"
#include "patts.h"

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
    return set_state(u8"User", id, u8"0");
}


