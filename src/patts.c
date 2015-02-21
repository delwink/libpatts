/*
 *  libpatts - Backend library for PATTS Ain't Time Tracking Software
 *  Copyright (C) 2014-2015 Delwink, LLC
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <stdlib.h>
#include <jansson.h>
#include "patts.h"

static sqon_dbsrv *PATTSDB;
static bool HAVE_ADMIN = false;
static char user_id[8];

int
patts_init (uint8_t db_type, const char *host, const char *user,
	    const char *passwd, const char *database)
{
  int rc = 0;
  const char *fmt = "SELECT isAdmin FROM User WHERE id=%s";
  char *query, *user_info;
  const char *isAdmin;
  json_t *list, *user_cols;
  size_t qlen = 1;

  if (strlen (user) >= 8)
    return PATTS_OVERFLOW;

  strcpy (user_id, user);

  sqon_init ();

  sqon_dbsrv db = sqon_new_connection (db_type, host, user, passwd, database);
  PATTSDB = &db;

  qlen += strlen (fmt) - 2;
  qlen += strlen (user);

  query = sqon_malloc (qlen * sizeof (char));
  if (NULL == query)
    return PATTS_MEMORYERROR;

  snprintf (query, qlen, fmt, user);

  rc = sqon_query (patts_get_db (), query, &user_info, NULL);
  sqon_free (query);

  if (rc)
    return rc;

  list = json_loads (user_info, 0, NULL);
  sqon_free (user_info);

  if (NULL == list)
    return PATTS_LOADERROR;
  if (0 == json_array_size (list))
    return PATTS_NOSUCHUSER;

  user_cols = json_array_get (list, 0);

  isAdmin = json_string_value (json_object_get (user_cols, "isAdmin"));

  if (!strcmp (isAdmin, "\\u0001") || !strcmp (isAdmin, "1"))
    HAVE_ADMIN = true;
  else if (!strcmp (isAdmin, "") || !strcmp (isAdmin, "0"))
    HAVE_ADMIN = false;
  else
    rc = PATTS_UNEXPECTED;

  json_decref (list);

  return rc;
}

sqon_dbsrv *
patts_get_db ()
{
  return PATTSDB;
}

const char *
patts_get_user ()
{
  return user_id;
}

bool
patts_have_admin ()
{
  return HAVE_ADMIN;
}
