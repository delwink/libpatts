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

#include <my_global.h>

#include "conn.h"

struct dbconn patts_new_connection(const char *host, const char *user,
        const char *passwd, const char *database)
{
    struct dbconn out = {
        .host = host,
        .user = user,
        .passwd = passwd,
        .database = database
    };
    return out;
}

int patts_connect(struct dbconn *con)
{
    con->con = mysql_init(NULL);

    if (mysql_real_connect(con->con, con->host, con->user, con->passwd,
            con->database, 0, NULL, CLIENT_MULTI_STATEMENTS) == NULL) {
        return 1;
    }

    return 0;
}

void patts_close_connection(struct dbconn *con)
{
    mysql_close(con->con);
}

int patts_test_connect(const char *host, const char *user, const char *passwd,
        const char *database)
{
    int rc;
    struct dbconn con = patts_new_connection(host, user, passwd, database);

    rc = patts_connect(&con);
    patts_close_connection(&con);
    return rc;
}
