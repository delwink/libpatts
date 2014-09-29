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

int patts_admin_create_user(char *fullName, unsigned int mysqlUser);
int patts_admin_create_task(char *displayName);
int patts_admin_create_client(char *fullName);

int patts_admin_enable_user(unsigned int id);
int patts_admin_enable_task(unsigned int id);
int patts_admin_enable_client(unsigned int id);

int patts_admin_disable_user(unsigned int id);
int patts_admin_disable_task(unsigned int id);
int patts_admin_disable_client(unsigned int id);

#endif
