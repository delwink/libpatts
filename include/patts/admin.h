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

#ifndef DELWINK_PATTS_H
#define DELWINK_PATTS_H

#include <cquel.h>

#ifdef __cplusplus
extern "C"
{
#endif

int patts_create_user(const struct dlist *info, const char *host,
        const char *passwd);
int patts_create_task(const struct dlist *info);

int patts_enable_user(const char *id);
int patts_enable_task(const char *id);

int patts_disable_user(const char *id);
int patts_disable_task(const char *id);

int patts_grant_admin(const char *id);
int patts_revoke_admin(const char *id);

#ifdef __cplusplus
}
#endif

#endif
