/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef LBS_LOCMONITORDBENGINEDEFS_H
#define LBS_LOCMONITORDBENGINEDEFS_H

const TInt KTempFlushSize = 50;
const TInt KMaxDbSize = 1000;

const TTimeIntervalMicroSeconds32 KInitialPeriod = 200000000;
const TTimeIntervalSeconds KFlushPeriod = 7200; // 2 Hours in seconds


#if defined (LBS_LOCMONITORDB_TEST) && defined (__WINS__)  // Unit test emulator
_LIT(KSecureLocMonDB, "C:\\lastknownlocarea.db");
#elif defined (LBS_LOCMONITORDB_TEST) && !defined (__WINSCW__) // Unit test hw
_LIT(KSecureLocMonDB, "E:\\lastknownlocarea.db");
#else // production code
_LIT(KSecureLocMonDB, "[102869DF]lastknownlocarea.db");
#endif

_LIT(KCreateTable, "CREATE TABLE monitorstore (mcc INTEGER, mnc INTEGER, lac INTEGER, cid INTEGER, date INTEGER, data TEXT);");
_LIT(KCreateLastPosTable, "CREATE TABLE lastpos (rowid INTEGER PRIMARY KEY ASC, date INTEGER, data TEXT);");

_LIT(KCreateIndex4, "CREATE UNIQUE INDEX four_idx1 ON monitorstore(mcc,mnc,lac,cid);");
_LIT(KCreateIndex3, "CREATE INDEX three_idx1 ON monitorstore(mcc,mnc,lac);");

_LIT(KCreateTempTable, "CREATE TEMP TABLE monitorstore_temp (mcc INTEGER, mnc INTEGER, lac INTEGER, cid INTEGER, date INTEGER, data TEXT);");
_LIT(KCreateLastPosTempTable, "CREATE TEMP TABLE lastpos_temp (date INTEGER, data TEXT);");
_LIT(KCreateTempIndex4, "CREATE UNIQUE INDEX four_temp_idx1 ON monitorstore_temp(mcc,mnc,lac,cid);");

_LIT(KUpsertRow, "INSERT OR REPLACE INTO monitorstore_temp (mcc, mnc, lac, cid, date, data) VALUES (:mcc, :mnc, :lac, :cid, :timestamp, :data);");
_LIT(KUpsertLastPosRow, "INSERT OR REPLACE INTO lastpos_temp (rowid, date, data) VALUES (1, :timestamp, :data);");
//_LIT(KUpsertRowActual, "INSERT OR REPLACE INTO monitorstore (mcc, mnc, lac, cid, date, data) VALUES (:mcc, :mnc, :lac, :cid, :timestamp, :data);");
//_LIT(KUpsertLastPosRowActual, "INSERT OR REPLACE INTO lastpos (rowid, date, data) VALUES (1, :timestamp, :data);");

_LIT(KSelectRow4, "SELECT data FROM monitorstore WHERE mcc = :mcc AND mnc = :mnc AND lac = :lac AND cid = :cid;");
_LIT(KSelectRow3, "SELECT data FROM monitorstore WHERE mcc = :mcc AND mnc = :mnc AND lac = :lac ORDER BY date DESC LIMIT 1;");
_LIT(KSelectRow2, "SELECT data FROM monitorstore WHERE mcc = :mcc AND mnc = :mnc ORDER BY date DESC LIMIT 1;");
_LIT(KSelectRow1, "SELECT data FROM monitorstore WHERE mcc = :mcc ORDER BY date DESC LIMIT 1;");
_LIT(KSelectRowLatest, "SELECT data FROM lastpos ORDER BY date DESC LIMIT 1;");

_LIT(KSelectTempRow4, "SELECT data FROM monitorstore_temp WHERE mcc = :mcc AND mnc = :mnc AND lac = :lac AND cid = :cid;");
_LIT(KSelectTempRow3, "SELECT data FROM monitorstore_temp WHERE mcc = :mcc AND mnc = :mnc AND lac = :lac ORDER BY date DESC LIMIT 1;");
_LIT(KSelectTempRow2, "SELECT data FROM monitorstore_temp WHERE mcc = :mcc AND mnc = :mnc ORDER BY date DESC LIMIT 1;");
_LIT(KSelectTempRow1, "SELECT data FROM monitorstore_temp WHERE mcc = :mcc ORDER BY date DESC LIMIT 1;");
_LIT(KSelectTempRowLatest, "SELECT data FROM lastpos_temp ORDER BY date DESC LIMIT 1;");

_LIT(KCount, "SELECT COUNT(*) FROM monitorstore;");
_LIT(KPrune, "DELETE FROM monitorstore WHERE ROWID IN (SELECT rowid FROM monitorstore ORDER BY date LIMIT 100)");

_LIT(KBegin, "BEGIN;");
_LIT(KCommit, "COMMIT;");
_LIT(KCopy, "INSERT OR REPLACE INTO monitorstore SELECT * FROM monitorstore_temp;");
_LIT(KLastPosCopy, "INSERT OR REPLACE INTO lastpos SELECT rowid, date, data FROM lastpos_temp limit 1;");
_LIT(KClear, "DELETE FROM monitorstore_temp;");
_LIT(KLastPosClear, "DELETE FROM lastpos_temp;");

_LIT(KMcc, ":mcc");
_LIT(KMnc, ":mnc");
_LIT(KLac, ":lac");
_LIT(KCid, ":cid");
_LIT(KData, ":data");
_LIT(KStamp, ":timestamp");

_LIT(KDataColumn, "data");
_LIT(KCountColumn, "COUNT(*)");

#endif
