/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DELWINK_PATTS_ADMIN_H
#define DELWINK_PATTS_ADMIN_H

#include <stdint.h>
#include "conn.h"

#ifdef __cplusplus
extern "C"
{
#endif

int patts_admin_create_user(patts_conn_Connection *con,
        char *firstName, char *lastName, char *mysqlUser);
int patts_admin_create_task(patts_conn_Connection *con,
        char *displayName, uint32_t parentID);

int patts_admin_enable_user(patts_conn_Connection *con, uint32_t id);
int patts_admin_enable_task(patts_conn_Connection *con, uint32_t id);

int patts_admin_disable_user(patts_conn_Connection *con, uint32_t id);
int patts_admin_disable_task(patts_conn_Connection *con, uint32_t id);

int patts_admin_grant_admin(patts_conn_Connection *con, uint32_t id);
int patts_admin_revoke_admin(patts_conn_Connection *con, uint32_t id);

#ifdef __cplusplus
}
#endif

#endif
