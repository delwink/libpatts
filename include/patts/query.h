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

#ifndef DELWINK_PATTS_QUERY_H
#define DELWINK_PATTS_QUERY_H

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

#include "conn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct drow;

struct drow {
    char **values;

    struct drow *next;
    struct drow *prev;
};

int patts_new_drow(struct drow *row);
void patts_free_drow(struct drow *row);
int patts_drow_set(struct drow *row, char **values);

struct dlist {
    size_t fieldc;
    char **fieldnames;

    struct drow *first;
    struct drow *last;
};

int patts_new_dlist(struct dlist *list);
void patts_free_dlist(struct dlist *list);
int patts_dlist_add(struct dlist *list, struct drow *row);
void patts_dlist_remove(struct dlist *list, size_t index);

int patts_insert(struct dbconn con, const char *table, struct dlist *list);
int patts_update(struct dbconn con, const char *table, struct dlist *list);

int patts_select(struct dbconn con, const char *table, struct dlist *out,
        const char *conditions);

#ifdef __cplusplus
}
#endif

#endif
