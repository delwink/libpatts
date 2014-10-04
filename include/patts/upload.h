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

#ifndef DELWINK_PATTS_UPLOAD_H
#define DELWINK_PATTS_UPLOAD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "conn.h"

int patts_upload_clockin(const patts_conn_Connection con);
int patts_upload_clockout(const patts_conn_Connection con);

#ifdef __cplusplus
}
#endif

#endif
