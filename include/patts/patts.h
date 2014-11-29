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

#include <stdbool.h>
#include <cquel.h>

#ifdef __cplusplus
extern "C"
{
#endif

int patts_init(size_t qlen, size_t fmaxlen, const char *host, const char *user,
        const char *passwd, const char *database);

size_t patts_fmaxlen(void);

struct dbconn patts_get_db(void);

const char *patts_get_user(void);

bool patts_have_admin(void);

const char *patts_interface(void);

const char *patts_version(void);

#ifdef __cplusplus
}
#endif

#endif
