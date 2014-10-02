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

#ifndef DELWINK_PATTS_EXPORT_H
#define DELWINK_PATTS_EXPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

int patts_export(const patts_TaskItem *tasks, char *out);
int patts_export_to_file(const patts_TaskItem *tasks, char *path);

#ifdef __cplusplus
}
#endif

#endif
