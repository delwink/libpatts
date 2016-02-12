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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

#include "internal.h"
#include "patts.h"

static sqon_DatabaseServer *PATTSDB;
static bool HAVE_ADMIN = false;
static char user_id[USERNAME_LEN + 1];
static char esc_user_id[USERNAME_LEN * 2 + 1];

void *
patts_malloc (size_t n)
{
  return sqon_malloc (n);
}

void
patts_free (void *v)
{
  sqon_free (v);
}

void
patts_set_alloc_funcs (void *(*new_malloc) (size_t n),
		       void (*new_free) (void *v))
{
  sqon_set_alloc_funcs (new_malloc, new_free);
}

int
patts_init (uint8_t db_type, const char *host, const char *user,
	    const char *passwd, const char *database, const char *port)
{
  int rc = 0;
  const char *fmt = "SELECT isAdmin FROM User WHERE dbUser='%s'";
  char *user_info, *esc_user;
  const char *isAdmin;
  json_t *list, *user_cols;
  size_t qlen = 1;

  if (strlen (user) >= 8)
    return PATTS_OVERFLOW;

  sqon_init ();
  PATTSDB = sqon_new_connection (db_type, host, user, passwd, database, port);

  rc = patts_escape (user, &esc_user, false);
  if (rc)
    return rc;

  strcpy (user_id, user);
  strcpy (esc_user_id, esc_user);

  qlen += strlen (fmt) - 2;
  qlen += USERNAME_LEN * 2;
  char query[qlen];

  snprintf (query, qlen, fmt, esc_user);
  sqon_free (esc_user);

  rc = patts_query (query, &user_info, NULL);
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

  if (strcmp (isAdmin, "\001") == 0 || strcmp (isAdmin, "1") == 0)
    HAVE_ADMIN = true;
  else if (strcmp (isAdmin, "") == 0 || strcmp (isAdmin, "0") == 0)
    HAVE_ADMIN = false;
  else
    rc = PATTS_UNEXPECTED;

  json_decref (list);

  return rc;
}

void
patts_cleanup ()
{
  sqon_free_connection (PATTSDB);
}

int
patts_connect ()
{
  return sqon_connect (PATTSDB);
}

void
patts_close ()
{
  sqon_close (PATTSDB);
}

int
patts_query (const char *query, char **out, const char *primary_key)
{
  return sqon_query (PATTSDB, query, out, primary_key);
}

int
patts_get_primary_key (const char *table, char **out)
{
  return sqon_get_primary_key (PATTSDB, table, out);
}

int
patts_escape (const char *in, char **out, bool quote)
{
  return sqon_escape (PATTSDB, in, out, quote);
}

const char *
patts_get_user ()
{
  return user_id;
}

const char *
patts_get_user_esc ()
{
  return esc_user_id;
}

bool
patts_have_admin ()
{
  return HAVE_ADMIN;
}

int
patts_get_db_version (uint32_t *out)
{
  int rc;
  char *result;
  json_t *meta_header, *metadata;

  rc = patts_query ("SELECT version FROM Meta LIMIT 1", &result, NULL);
  if (rc)
    return rc;

  meta_header = json_loads (result, 0, NULL);
  sqon_free (result);
  if (NULL == meta_header)
    return PATTS_UNEXPECTED;

  metadata = json_array_get (meta_header, 0);

  const char *v = json_string_value (json_object_get (metadata, "version"));
  sscanf (v, "%" SCNu32, out);
  json_decref (meta_header);

  return 0;
}
