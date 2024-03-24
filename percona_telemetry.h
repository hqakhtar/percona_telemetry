/*-------------------------------------------------------------------------
 *
 * percona_telemetry.h
 *      Collect telemetry information for the database cluster.
 *
 * Portions Copyright © 2018-2024, Percona LLC and/or its affiliates
 *
 * Portions Copyright (c) 1996-2024, PostgreSQL Global Development Group
 *
 * Portions Copyright (c) 1994, The Regents of the University of California
 *
 * IDENTIFICATION
 *    contrib/percona_telemetry/percona_telemetry.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef __PG_PERCONA_TELEMETRY_H__
#define __PG_PERCONA_TELEMETRY_H__

#include "miscadmin.h"
#include "access/xact.h"
#include "storage/ipc.h"


/* Struct to store pg_settings data */
typedef struct PTSettingsInfo
{
    char *name;
    char *unit;
    char *settings;
    char *reset_val;
    char *boot_val;
} PTSetttingsInfo;

/* Struct to keep track of databases telemetry data */
typedef struct PTDatabaseInfo
{
    Oid  datid;
    char datname[NAMEDATALEN];
    int64 datsize;
} PTDatabaseInfo;

/* Struct to keep track of extensions of a database */
typedef struct PTExtensionInfo
{
    char     extname[NAMEDATALEN];
    PTDatabaseInfo *db_data;
} PTExtensionInfo;

/*
 * Shared state to telemetry. We don't need any locks as we run only one
 * background worker at a time which may update this in case of an error.
 */
typedef struct PTSharedState
{
    int error_code;
    bool write_in_progress;
    char dbtemp_filepath[MAXPGPATH];
    char dbinfo_filepath[MAXPGPATH];
    PTDatabaseInfo dbinfo;
    int json_file_indent;
    TimestampTz last_file_processed;
    bool waiting_for_agent;
    bool first_db_entry;
    bool last_db_entry;
} PTSharedState;

/* Defining error codes */
#define PT_SUCCESS          0
#define PT_DB_ERROR         1
#define PT_FILE_ERROR       2
#define PT_JSON_ERROR       3

/* Must use to exit a background worker process. */
#define PT_WORKER_EXIT(e_code)                  \
{                                               \
    if (IsTransactionBlock())                   \
        CommitTransactionCommand();             \
    if (e_code != PT_SUCCESS)                   \
        ereport(LOG, (errmsg("percona_telemetry bgworker exiting with error_code = %d", e_code)));    \
    proc_exit(0);                               \
}

#endif