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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

#include "patts.h"
#include "user.h"
#include "get.h"
#include "internal.h"

#define MAX_ID_LEN 10 /* maximum length for a 32-bit INT in characters */

int
patts_get_active_task (char **out)
{
  int rc;
  const char *fmt = "SELECT * FROM TaskItem "
    "WHERE state=1 AND onClock=1 AND userID='%s' "
    "ORDER BY id DESC LIMIT 1";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (patts_get_user ());

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, patts_get_user ());

  rc = sqon_query (patts_get_db (), query, out, NULL);
  sqon_free (query);

  return rc;
}

int
patts_get_tree (char **out)
{
  int rc;
  const char *fmt = "SELECT id,typeID,userID,startTime FROM TaskItem "
    "WHERE state=1 AND onClock=1 AND userID='%s'";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += strlen (patts_get_user ());

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, patts_get_user ());

  rc = sqon_query (patts_get_db (), query, out, "id");
  sqon_free (query);

  return rc;
}

int
patts_clockin (const char *type)
{
  int rc;
  const char *fmt = "1,1,NOW(),%s,'%s'";
  char *args;
  size_t len = 1;

  len += strlen (fmt) - 4;
  len += strlen (type);
  len += strlen (patts_get_user ());

  args = sqon_malloc (len * sizeof (char));
  if (NULL == args)
    return PATTS_MEMORYERROR;

  snprintf (args, len, fmt, type, patts_get_user ());

  rc = call_procedure ("clockIn", args);
  sqon_free (args);

  return rc;
}

int
patts_clockout (const char *item)
{
  int rc;
  char *esc_item;
  size_t len = strlen (item) * 2 + 1;

  esc_item = sqon_malloc (len * sizeof (char));
  if (NULL == esc_item)
    return PATTS_MEMORYERROR;

  rc = sqon_escape (patts_get_db (), item, esc_item, len, false);
  if (rc)
    {
      sqon_free (esc_item);
      return rc;
    }

  rc =  call_procedure ("clockOut", esc_item);
  sqon_free (esc_item);

  return rc;
}
