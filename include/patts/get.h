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
#define DELWINK_PATTS_GET_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "query.h"

int patts_get_users(struct dbconn con, struct dlist *out);
int patts_get_user_byid(struct dbconn con, struct dlist *out, const char *id);

int patts_get_types(struct dbconn con, struct dlist *out);
int patts_get_type_byid(struct dbconn con, struct dlist *out, const char *id);

int patts_get_items(struct dbconn con, struct dlist *out);
int patts_get_item_byid(struct dbconn con, struct dlist *out, const char *id);
int patts_get_items_byuser(struct dbconn con, struct dlist *out,
        const char *userID);
int patts_get_items_byuser_onclock(struct dbconn con, struct dlist *out,
        const char *userID);

#ifdef __cplusplus
}
#endif

#endif
