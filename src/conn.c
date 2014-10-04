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

int patts_conn_open(patts_conn_Connection *con)
{
    con->con = mysql_init(NULL);

    if (mysql_real_connect(con->con, con->host, con->user, con->passwd,
            "pattsdb", 0, NULL, 0) == NULL) {
        mysql_close(con->con);
        return 1;
    }

    return 0;
}

void patts_conn_close(const patts_conn_Connection *con)
{
    mysql_close(con->con);
}

int patts_conn_test(patts_conn_Connection con)
{
    if (patts_conn_open(&con))
        return 1;
    patts_conn_close(&con);
    return 0;
}
