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
 * @file user.h
 * @version 0.0
 * @date 4/7/2015
 * @author David McMackins II
 * @brief PATTS user functions.
 */

#ifndef DELWINK_PATTS_USER_H
#define DELWINK_PATTS_USER_H

#define MAX_ID_LEN 12 // max length for a 32-bit INT in characters plus NUL

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Gets the active task for the current user.
 * @param out Unallocated pointer to string for the output.
 * @return Nonzero on error.
 */
int
patts_get_active_task (char **out);

/**
 * @brief Gets the tree of active tasks for the user.
 * @param out Unallocated pointer to string for the output.
 * @return Nonzero on error.
 */
int
patts_get_tree (char **out);

/**
 * @brief Clock into a task.
 * @param type String representation of the typeID of the task.
 * @return Nonzero on error.
 */
int
patts_clockin (const char *type);

/**
 * @brief Clock out of a task and all its subtasks.
 * @param item String representation of the item ID.
 * @return Nonzero on error.
 */
int
patts_clockout (const char *item);

#ifdef __cplusplus
}
#endif

#endif
