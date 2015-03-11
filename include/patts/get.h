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
 * @file get.h
 * @version 0.0
 * @date 3/10/2015
 * @brief Functions for getting information from the PATTS database.
 */

#ifndef DELWINK_PATTS_GET_H
#define DELWINK_PATTS_GET_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Gets all PATTS users.
 * @param out Unallocated pointer to string for the output.
 * @return Nonzero on error.
 */
int
patts_get_users (char **out);

/**
 * @brief Gets a PATTS user by username.
 * @param out Unallocated pointer to string for the output.
 * @param id Username for which to search.
 * @return Nonzero on error.
 */
int
patts_get_user_byid (char **out, const char *id);

/**
 * @brief Gets all PATTS task types.
 * @param out Unallocated pointer to string for the output.
 * @return Nonzero on error.
 */
int
patts_get_types (char **out);

/**
 * @brief Gets a PATTS task type by its ID number.
 * @param out Unallocated pointer to string for the output.
 * @param id String representation of the ID number for which to search.
 * @return Nonzero on error.
 */
int
patts_get_type_byid (char **out, const char *id);

/**
 * @brief Gets all the child types of a particular PATTS task type.
 * @param out Unallocated pointer to string for the output.
 * @param parent_id The ID number of the parent type.
 * @return Nonzero on error.
 */
int
patts_get_child_types (char **out, const char *parent_id);

/**
 * @brief Gets all PATTS tasks.
 * @param out Unallocated pointer to string for the output.
 * @return Nonzero on error.
 */
int
patts_get_items (char **out);

/**
 * @brief Gets a PATTS task by ID number.
 * @param out Unallocated pointer to string for the output.
 * @param id String representation of the ID number for which to search.
 * @return Nonzero on error.
 */
int
patts_get_item_byid (char **out, const char *id);

/**
 * @brief Gets the last PATTS task ID number for a given user.
 * @param out Address of output variable.
 * @param user_id Username with which to find the task ID.
 * @return Nonzero on error.
 */
int
patts_get_last_item (size_t *out, const char *user_id);

/**
 * @brief Gets all PATTS items for a given user.
 * @param out Unallocated pointer to string for the output.
 * @param user_id Username for which to search.
 * @return Nonzero on error.
 */
int
patts_get_items_byuser (char **out, const char *user_id);

/**
 * @brief Gets all PATTS items for a given user that are currently clocked in.
 * @param out Unallocated pointer to string for the output.
 * @param user_id Username for which to search.
 * @return Nonzero on error.
 */
int
patts_get_items_byuser_onclock (char **out, const char *user_id);

/**
 * @brief Gets all child items for a PATTS task (calculated based on data).
 * @param out Unallocated pointer to string for the output.
 * @param id String representation of the ID number of the parent task.
 * @return Nonzero on error.
 */
int
patts_get_child_items (char **out, const char *id);

#ifdef __cplusplus
}
#endif

#endif
