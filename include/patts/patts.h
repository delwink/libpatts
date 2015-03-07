/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2014-2015 Delwink, LLC
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, version 3 only.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file patts.h
 * @version 0.0
 * @date 3/6/2015
 * @author David McMackins II
 * @brief Functions global to PATTS
 */

#ifndef DELWINK_PATTS_H
#define DELWINK_PATTS_H

#include <sqon.h>

#define PATTS_VERSION "0.0.0"

#define PATTS_COPYRIGHT \
"libpatts - Backend library for PATTS Ain't Time Tracking Software\n"\
"Copyright (C) 2014-2015 Delwink, LLC\n\n"\
"This program is free software: you can redistribute it and/or modify\n"\
"it under the terms of the GNU Affero General Public License as published by\n"\
"the Free Software Foundation, version 3 only.\n\n"\
"This program is distributed in the hope that it will be useful,\n"\
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"\
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"\
"GNU Affero General Public License for more details.\n\n"\
"You should have received a copy of the GNU Affero General Public License\n"\
"along with this program.  If not, see <http://www.gnu.org/licenses/>."

#define PATTS_LOADERROR   -60

#define PATTS_MEMORYERROR SQON_MEMORYERROR-50

#define PATTS_UNEXPECTED  SQON_UNSUPPORTED-50

#define PATTS_OVERFLOW    SQON_OVERFLOW-50

#define PATTS_NOSUCHUSER  SQON_NOPK-50

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initializes connection to the PATTS database.
 * @param db_type Database connection type.
 * @param host Hostname or IP address of the database server.
 * @param user Database username.
 * @param passwd Database authentication password.
 * @param database Database to be used.
 * @return Nonzero on error.
 */
int
patts_init (uint8_t db_type, const char *host, const char *user,
	    const char *passwd, const char *database);

/**
 * @brief Gets the database connection object.
 * @return PATTS database connection object pointer or NULL on failure.
 */
sqon_dbsrv *
patts_get_db (void);

/**
 * @brief Gets the active username.
 * @return String representation of the active username.
 */
const char *
patts_get_user (void);

/**
 * @brief Checks user's admin rights.
 * @return true if user is a PATTS administrator, else false.
 */
bool
patts_have_admin (void);

#ifdef __cplusplus
}
#endif

#endif
