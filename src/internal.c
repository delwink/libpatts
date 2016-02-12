/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2014-2016 Delwink, LLC
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

#include "internal.h"
#include "patts.h"

int
call_procedure (const char *proc, const char *args)
{
  const char *fmt = "CALL %s(%s)";
  size_t qlen = 1;

  qlen += strlen (fmt) - 4;
  qlen += strlen (proc);
  qlen += strlen (args);
  char query[qlen];

  snprintf (query, qlen, fmt, proc, args);
  return patts_query (query, NULL, NULL);
}
