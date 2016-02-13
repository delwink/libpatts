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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "get.h"
#include "internal.h"
#include "patts.h"
#include "user.h"

int
patts_get_active_task (char **out)
{
  const char *fmt = "SELECT * FROM TaskItem "
    "WHERE state=1 AND stopTime IS NULL AND userID='%s' "
    "ORDER BY id DESC LIMIT 1";
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += USERNAME_LEN * 2;
  char query[qlen];

  snprintf (query, qlen, fmt, patts_get_user_esc ());
  return patts_query (query, out, "id");
}

int
patts_get_tree (char **out)
{
  const char *fmt = "SELECT id,typeID,userID,startTime FROM TaskItem "
    "WHERE state=1 AND stopTime IS NULL AND userID='%s'";
  size_t qlen = 1;

  qlen += strlen (fmt) - 2;
  qlen += USERNAME_LEN * 2;
  char query[qlen];

  snprintf (query, qlen, fmt, patts_get_user_esc ());
  return patts_query (query, out, "id");
}

int
patts_clockin (const char *type)
{
  int rc;
  const char *fmt = "%s,'%s'";

  char *esc_type;
  rc = patts_escape (type, &esc_type, false);
  if (rc)
    return rc;

  size_t len = 1;
  len += strlen (fmt) - 4;
  len += MAX_ID_LEN * 2;
  len += USERNAME_LEN * 2;
  char args[len];

  snprintf (args, len, fmt, esc_type, patts_get_user_esc ());
  patts_free (esc_type);

  rc = call_procedure ("clockIn", args);
  if (1210 == rc)
    rc = PATTS_UNAVAILABLE; // server said so

  return rc;
}

int
patts_clockout (const char *item)
{
  int rc;
  char *esc_item;

  rc = patts_escape (item, &esc_item, false);
  if (rc)
    return rc;

  rc = call_procedure ("clockOut", esc_item);
  patts_free (esc_item);

  return rc;
}
