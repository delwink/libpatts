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

#ifndef DELWINK_PATTS_CONN_H
#define DELWINK_PATTS_CONN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <mysql.h>

typedef struct {
    MYSQL *con;
    char *host;
    char *user;
    char *passwd;
} patts_conn_Connection;

int patts_conn_open(const patts_conn_Connection con);
int patts_conn_close(const patts_conn_Connection con);
int patts_conn_test(const patts_conn_Connection con);

#ifdef __cplusplus
}
#endif

#endif
