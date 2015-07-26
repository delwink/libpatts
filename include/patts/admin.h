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
 * @file admin.h
 * @version 0.0
 * @date 07/26/2015
 * @author David McMackins II
 * @brief Functions for PATTS administrators.
 */

#ifndef DELWINK_PATTS_ADMIN_H
#define DELWINK_PATTS_ADMIN_H

#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

__BEGIN_DECLS

/**
 * @brief Creates a new user on the database and applies proper permissions for
 * a new user.
 * @param id A username for the new user.
 * @param host The hostname (or wildcard string) from which this user will be
 * allowed to connect.
 * @param passwd A password for the new user.
 * @return Nonzero on error.
 */
int
patts_create_user (const char *id, const char *host, const char *passwd);

/**
 * @brief Creates a new PATTS task type.
 * @param parent_id The ID number of the parent type ("0" if top-level).
 * @param display_name The display name of the new type.
 * @return Nonzero on error.
 */
int
patts_create_task (const char *parent_id, const char *display_name);

/**
 * @brief Deactivates a user in the database.
 * @param id The username of the user to be deactivated.
 * @return Nonzero on error.
 */
int
patts_delete_user (const char *id);

/**
 * @brief Deactivates a task type.
 * @param id String represenation of the ID number of the task to be
 * deactivated.
 * @return Nonzero on error.
 */
int
patts_delete_task (const char *id);

/**
 * @brief Grants admin permissions to a PATTS user.
 * @param id Username of the new admin user.
 * @param host Hostname for which to grant the user privileges.
 * @return Nonzero on error.
 */
int
patts_grant_admin (const char *id, const char *host);

/**
 * @brief Revokes admin permissions from a PATTS user.
 * @param id Username of the admin user to be demoted.
 * @param host Hostname for which to deny the user privileges.
 * @return Nonzero on error.
 */
int
patts_revoke_admin (const char *id, const char *host);

__END_DECLS

#endif
