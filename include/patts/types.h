/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DELWINK_PATTS_TYPES_H
#define DELWINK_PATTS_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define DATETIME_LEN 19

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    uint32_t id;
    bool state;
    bool isAdmin;
    char *firstName;
    char *lastName;
    char *mysqlUser;
} patts_User;

typedef struct {
    uint32_t id;
    bool state;
    uint32_t parentID;
    char *displayName;
} patts_TaskType;

typedef struct {
    uint32_t id;
    bool state;
    bool onClock;
    uint32_t userID;
    char *startTime;
    char *stopTime;
} patts_TaskItem;

#ifdef __cplusplus
}
#endif

#endif
