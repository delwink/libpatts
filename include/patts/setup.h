/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2015-2016 Delwink, LLC
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
 * @file setup.h
 * @version 0.1
 * @date 02/12/2016
 * @author David McMackins II
 * @brief PATTS database setup utilities.
 */

#ifndef DELWINK_PATTS_SETUP_H
#define DELWINK_PATTS_SETUP_H

#include <stdint.h>

#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

__BEGIN_DECLS

/**
 * @brief Creates and sets up a new PATTS database.
 * @param db_type Database connection type.
 * @param host Hostname or IP address of the database server.
 * @param user Database server username.
 * @param passwd Database server authentication password.
 * @param database Database to be created.
 * @param port String representation of the port number.
 * @return Nonzero on error.
 */
int
patts_setup (uint8_t db_type, const char *host, const char *user,
	     const char *passwd, const char *database, const char *port);

/**
 * @brief Upgrades an existing PATTS database.
 * @param db_type Database connection type.
 * @param host Hostname or IP address of the database server.
 * @param user Database server username.
 * @param passwd Database server authentication password.
 * @param database Database to be upgraded.
 * @param port String representation of the port number.
 * @return Nonzero on error.
 */
int
patts_upgrade_db (uint8_t db_type, const char *host, const char *user,
		  const char *passwd, const char *database, const char *port);

/**
 * @brief Compares the current database schema to the version supported by this
 * interface.
 * @param out Pointer to output value which is positive when an update is
 * needed, negative when the library is out of date, and zero when up-to-date.
 * @return Nonzero on error.
 */
int
patts_version_check (int64_t *out);

__END_DECLS

#endif
