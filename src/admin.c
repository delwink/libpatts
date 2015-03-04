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
#include <sqon.h>

#include "admin.h"
#include "patts.h"
#include "internal.h"

int
patts_create_user (const char *id, const char *host, const char *passwd)
{
  int rc;
  const char *fmt = "'%s','%s','%s'";
  char *args;
  size_t len = 1;

  len += strlen (fmt) - 6;
  len += strlen (id);
  len += strlen (host);
  len += strlen (passwd);

  args = sqon_malloc (len * sizeof (char));
  if (NULL == args)
    return PATTS_MEMORYERROR;

  snprintf (args, len, fmt, id, host, passwd);

  rc = call_procedure ("createUser", args);
  sqon_free (args);

  return rc;
}

int
patts_create_task (const char *parentID, const char *displayName)
{
  int rc;
  const char *fmt = "INSERT INTO TaskType(parentID,displayName) "
    "VALUES(%s,'%s')";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 4;
  qlen += strlen (parentID);
  qlen += strlen (displayName);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, parentID, displayName);

  rc = sqon_query (patts_get_db (), query, NULL, NULL);
  sqon_free (query);

  return rc;
}

static int
set_state (const char *id, const char *table, const char *state)
{
  int rc;
  const char *fmt = "UPDATE %s SET state=%s WHERE id=%s";
  char *query;
  size_t qlen = 1;

  qlen += strlen (fmt) - 6;
  qlen += strlen (id);
  qlen += strlen (table);
  qlen += strlen (state);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, table, state, id);

  rc = sqon_query (patts_get_db (), query, NULL, NULL);
  sqon_free (query);

  return rc;
}

int
patts_delete_user (const char *id)
{
  return set_state (id, "User", "0");
}

int
patts_delete_task (const char *id)
{
  return set_state (id, "TaskType", "0");
}

int
patts_grant_admin (const char *id)
{
  return call_procedure ("grantAdmin", id);
}

int
patts_revoke_admin (const char *id)
{
  return call_procedure ("revokeAdmin", id);
}
