/*
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

#include "patts.h"
#include "user.h"
#include "get.h"

int patts_get_active_task(char *out_id, size_t buflen)
{
    if (out_id == NULL)
        return 1;

    struct dlist *tasks;
    int rc = patts_get_items_byuser_onclock(&tasks, patts_get_user());
    if (rc)
        return 100;

    size_t index;
    rc = cq_field_to_index(tasks, u8"id", &index);
    if (rc) {
        cq_free_dlist(tasks);
        return 101;
    }

    if (strlen(tasks->last->values[index]) >= buflen) {
        cq_free_dlist(tasks);
        return -1;
    }

    strcpy(out_id, tasks->last->values[index]);

    cq_free_dlist(tasks);
    return 0;
}

int patts_clockin(const char *typeID)
{
    if (typeID == NULL)
        return 1;

    const size_t fieldc = 5;

    char *fields[] = {
        u8"state",
        u8"typeID",
        u8"onClock",
        u8"userID",
        u8"startTime"
    };

    const char *initial_vals[] = {
        u8"1",
        typeID,
        u8"1",
        patts_get_user(),
        u8"\\NOW()"
    };

    struct dlist *newitem = cq_new_dlist(fieldc, fields, NULL);
    if (newitem == NULL)
        return -1;

    struct drow *iteminfo = cq_new_drow(fieldc);
    if (iteminfo == NULL) {
        cq_free_dlist(newitem);
        return -2;
    }

    int rc = cq_drow_set(iteminfo, initial_vals);
    if (rc) {
        cq_free_dlist(newitem);
        cq_free_drow(iteminfo);
        return 100;
    }

    cq_dlist_add(newitem, iteminfo);

    rc = cq_insert(patts_get_db(), u8"TaskItem", newitem);
    cq_free_dlist(newitem);

    return rc;
}
