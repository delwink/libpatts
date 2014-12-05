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

#ifndef DELWINK_PATTS_GET_H
#define DELWINK_PATTS_GET_H

#include <cquel.h>

#ifdef __cplusplus
extern "C"
{
#endif

int patts_get_users(struct dlist **out);
int patts_get_user_byid(struct dlist **out, const char *id);

int patts_get_types(struct dlist **out);
int patts_get_type_byid(struct dlist **out, const char *id);
int patts_get_child_types(struct dlist **out, const char *parentID);

int patts_get_items(struct dlist **out);
int patts_get_item_byid(struct dlist **out, const char *id);
int patts_get_last_item(size_t *out, const char *userID);
int patts_get_items_byuser(struct dlist **out, const char *userID);
int patts_get_items_byuser_onclock(struct dlist **out, const char *userID);
int patts_get_child_items(struct dlist **out, const char parentID);

#ifdef __cplusplus
}
#endif

#endif
