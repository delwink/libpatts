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

#include <mysql.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct dbconn {
    MYSQL *con;
    const char *host;
    const char *user;
    const char *passwd;
    const char *database;
};

struct dbconn patts_new_connection(const char *host, const char *user,
        const char *passwd, const char *database);

int patts_connect(struct dbconn *con);
void patts_close_connection(const struct dbconn *con);
int patts_test_connect(const char *host, const char *user, const char *passwd,
        const char *database);

#ifdef __cplusplus
}
#endif

#endif
