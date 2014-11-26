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

#include "patts.h"
#include "user.h"

int patts_clockin(const char *typeID)
{
    if (typeID == NULL)
        return 1;

    const size_t fieldc = 5;

    const char *fields[] = {u8"state", u8"typeID", u8"onClock", u8"userID",
            u8"startTime"};

    const char *initial_vals[] = {u8"1", typeID, u8"1", patts_get_user(),
            u8"NOW()"};

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
