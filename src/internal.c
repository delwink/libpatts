/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2014-2015 Delwink, LLC
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, version 3 only.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sqon.h>

#include "patts.h"
#include "internal.h"

int
call_procedure (const char *proc, const char *args)
{
  int rc;
  const char *fmt = "CALL %s(%s)";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 4;
  qlen += strlen (proc);
  qlen += strlen (args);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, proc, args);

  rc = sqon_query (patts_get_db (), query, NULL, NULL);
  sqon_free (query);

  return rc;
}
