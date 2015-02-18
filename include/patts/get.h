/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2014-2015 Delwink, LLC
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

#ifdef __cplusplus
extern "C"
{
#endif

int
patts_get_users (char **out);

int
patts_get_user_byid (char **out, const char *id);

int
patts_get_types (char **out);

int
patts_get_type_byid (char **out, const char *id);

int
patts_get_child_types (char **out, const char *parent_id);

int
patts_get_items (char **out);

int
patts_get_item_byid (char **out, const char *id);

int
patts_get_last_item (size_t *out, const char *user_id);

int
patts_get_items_byuser (char **out, const char *user_id);

int
patts_get_items_byuser_onclock (char **out, const char *user_id);

int
patts_get_child_items (char **out, const char *id);

#ifdef __cplusplus
}
#endif

#endif
