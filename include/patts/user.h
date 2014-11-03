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

#ifndef DELWINK_PATTS_USER_H
#define DELWINK_PATTS_USER_H

#include <cquel.h>

#ifdef __cplusplus
extern "C"
{
#endif

int patts_get_active_task(char *out_id, size_t buflen);

int patts_get_tree(const char *userID, struct dlist *out);

int patts_clockin(const char *typeID);

int patts_clockout(const char *itemID);

#ifdef __cplusplus
}
#endif

#endif
