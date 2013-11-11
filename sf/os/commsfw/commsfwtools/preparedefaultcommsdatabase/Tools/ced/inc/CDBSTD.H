// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalComponent
*/

#ifndef __CDBSTD_H__
#define __CDBSTD_H__

//#include <commdb.h>

/**
Database location macros
@internalComponent
*/

#define KMaxFileNameSize					220
_LIT(KDbBaseName, "Cdbv3.dat");
_LIT(KCDrive, "c:");

_LIT(KDbPolicyUIDString, "secure[10004e1d]");
_LIT(KDb, "C:\\private\\100012a5\\DBS_10004e1d_Cdbv3.dat");
_LIT(KDefaultDb, "Z:\\private\\100012a5\\DBS_10004e1d_DefaultCdbv3.dat");
_LIT(KDbFileName, "C:Cdbv3.dat");


#if defined(_UNICODE)
#define _I(x) L ## x
#else
#define _I(x) x
#endif

/**
The _I() strings are passed as parameter s to the CDB_LIT macro
@internalComponent
*/
#define CDB_LIT(name,s) const TLitC<sizeof(s)/2> name={sizeof(s)/2-1,s}

/**
Server name
@internalComponent
*/
#define _SERVER_NAME				_I("CommsDbServer")

/**
Misc table names that don't need to be public
@internalComponent
*/
#define _GLOBAL_SETTINGS			_I("GlobalSettings")
#define _CONNECTION_PREFERENCES		_I("ConnectionPreferences")
#define _ACCESS_TABLE				_I("AccessTypeTable")


/**
Columns for Access Type table
@internalComponent
*/
#define ACCESS_TYPE_TABLE		_S("AccessTypeTable")
#define TABLE_ACC_NAME			_S("TableName")
#define TABLE_ACC_TYPE			_S("AccessType")


/**
DEPRECATED : Agent Table has been removed
@internalComponent
*/

//#define _AGENT						_I("Agent")

/**
Specifies whether a record is hidden from the client.
Field type: TBool.
*/
#define _COMMDB_HIDDEN				_I("Hidden")

/**
Specifies whether a record is read-only.
Field type: TBool.
*/
#define _COMMDB_READ_ONLY			_I("ReadOnly")	
#define _COMMDB_INT_VALUE			_I("IntValue")	//< integer value for global settings
#define _COMMDB_TEXT_VALUE			_I("TextValue")	//< text value for global settings

//
// Columns for Connection Preferences table
//
/** Ranking of this connection preference.
This information is used to specify the order in which attempts at a connection should be
carried out. The highest ranking is 1 and is always attempted. If this fails, a preference
with rank 2 is then attempted, and so on. Ranking of 0 indicates that a connection
preference is only being stored and is not used for connections.

  For incoming connections, CommDb limits the ranking to 0 or 1. For outgoing connection
  preferences, the maximum ranking is the value in CONNECTION_ATTEMPTS field in the Global
  table.
  
	Multiple records with the same ranking (except for 0) and the same direction are not allowed.
	
	  Field type: TUint32. This field may not be left as NULL.
	  @internalComponent
*/
#define _CONNECT_PREF_RANKING		_I("Ranking")

/**
The direction of the connection. The directions are defined by the enum 
TCommDbConnectionDirection.
Field type: TUint32. This field may not be left as NULL.
*/
#define _CONNECT_PREF_DIRECTION		_I("Direction")
/**
The bearers that can be used for this connection. 

  The bearers are defined by the enum TCommDbBearer. Values can be OR'ed together to indicate 
  that either of the bearers could be used.
  Field type: TUint32. This field may not be left as NULL.
*/
#define _CONNECT_PREF_BEARER_SET	_I("BearerSet")

/**
Determines whether or not the user should be prompted or warned with a dialog 
that a connection is going to be made.

  The options are defined by the enum TCommDbDialogPref.
  
	Field type: TUint32. This field may not be left as NULL.
	
	  @see TCommDbDialogPref.
*/
#define _CONNECT_PREF_DIALOG_PREF	_I("DialogPref")

/**
Identifier of a preferred IAP for this connection, if this is an IAP type database.

  For ISP type databases, see the subsequent fields..
  
	Field type: TUint32. This field may be left as NULL.
*/
#define _CONNECT_PREF_IAP			_I("IAP")


/**
Columns for Agent table
@internalComponent
*/

//#define _AGENT_SERVICE_TYPE			_I("AgentServiceType")
//#define _AGENT_NAME					_I("AgentName")
//#define _AGENT_EXT_NAME				_I("AgentExtName")
//#define _AGENT_LAST_SOCKET_ACTIVITY_TIMEOUT	_I("AgentLastSocketActivityTimeout")
//#define _AGENT_LAST_SESSION_CLOSED_TIMEOUT	_I("AgentLastSessionClosedTimeout")
//#define _AGENT_LAST_SOCKET_CLOSED_TIMEOUT	_I("AgentLastSocketClosedTimeout")


/**
SQL query strings
@internalComponent
*/
#define _SQL_START_SQL_QUERY		_I("select * from ")
#define _SQL_WHERE_KEYWORD			_I("where")
#define _SQL_AND_KEYWORD			_I(" and")
#define _SQL_ORDER_KEYWORD			_I("order")
#define _SQL_ORDER_BY_KEYWORD		_I(" order by ")
#define _SQL_WHERE_HIDDEN			_I(" where Hidden=0")
#define _SQL_AND_HIDDEN				_I(" and Hidden=0")
#define _SQL_INT_MATCH				_I("%S=%d")
#define _SQL_WHERE_INT_MATCH		_I(" where %S=%d")
#define _SQL_WHERE_TEXT_MATCH		_I(" where %S='%S'")
#define _SQL_AND_INT_MATCH			_I(" and %S=%d")
#define _SQL_AND_TEXT_MATCH			_I(" and %S='%S'")
#define _SQL_OR_TEXT_MATCH			_I(" or %S='%S'")
#define _SQL_WHERE_INT_GREATERTHAN	_I(" where %S>%d")

/**
SQL query lengths
@internalComponent
*/
//const TInt KTwiceMaxFieldLength=(2*KCommsDbSvrRealMaxFieldLength);
//const TInt KBasicSqlQueryLength=KCommsDbSvrMaxColumnNameLength+29;		// 29 is length of "select * from  where Hidden=0"
//const TInt KSqlQueryWithMatch=KBasicSqlQueryLength+KCommsDbSvrMaxColumnNameLength+KTwiceMaxFieldLength+8;  // 8 is length of "='' and "
//const TInt KSqlQueryWithDoubleMatch=KSqlQueryWithMatch+KTwiceMaxFieldLength+8;  // 8 is length of "='' and "
//const TInt KSqlQueryWithDoubleMatchAndOrderBy=KSqlQueryWithDoubleMatch+KCommsDbSvrMaxColumnNameLength+10;	//10 is the length of " order by "
//const TInt KSqlQueryExtraWhereTextMatchesColumn=KTwiceMaxFieldLength+10; // 10 is length of " where %S='%S'"

/**
These names are maintained for compatibility they are deprecated
and should be replaced in code by CDB_LIT declarations
@internalComponent
*/
#define SERVER_NAME					(const TText*)_SERVER_NAME
#define GLOBAL_SETTINGS				(const TText*)_GLOBAL_SETTINGS
#define CONNECTION_PREFERENCES		(const TText*)_CONNECTION_PREFERENCES
//#define AGENT						(const TText*)_AGENT
/** 
Specifies whether a record is hidden.
Field type: TBool
*/
#define COMMDB_HIDDEN				(const TText*)_COMMDB_HIDDEN

/** 
Specifies whether a record is read only.
Field type: TBool
*/
#define COMMDB_READ_ONLY			(const TText*)_COMMDB_READ_ONLY

#define COMMDB_INT_VALUE			(const TText*)_COMMDB_INT_VALUE
#define COMMDB_TEXT_VALUE			(const TText*)_COMMDB_TEXT_VALUE

/**
Ranking of this connection preference.

  This information is used to specify the order in which attempts at a connection 
  should be carried out. The highest ranking is 1 and is always attempted. If 
  this fails, a preference with rank 2 is then attempted, and so on. A ranking 
  of 0 indicates that a connection preference is only being stored and is not 
  used for connections.
  
	For incoming connections, CommDb limits the ranking to 0 or 1. For outgoing 
	connection preferences, the maximum ranking is the value in CONNECTION_ATTEMPTS 
	field in the Global table.
	
	  Multiple records with the same ranking (except for 0) and the same direction 
	  are not allowed.
	  
		Type: TUint32. Null: no
		
		  @see CONNECTION_ATTEMPTS 
*/
#define CONNECT_PREF_RANKING		(const TText*)_CONNECT_PREF_RANKING

/** 
The direction of the connection. The directions are defined by the enum 
TCommDbConnectionDirection.

  Type: TUint32. Null: no 
  
	@see TCommDbConnectionDirection
*/
#define CONNECT_PREF_DIRECTION		(const TText*)_CONNECT_PREF_DIRECTION

/** 
The bearers that can be used for this connection.

  The bearers are defined by the enum TCommDbBearer. Values can be ORed together 
  to indicate that either of the bearers could be used.
  
	Type: TUint32. Null: no
	
	  @see TCommDbBearer 
*/
#define CONNECT_PREF_BEARER_SET		(const TText*)_CONNECT_PREF_BEARER_SET

/** 
Determines whether or not the user should be should be prompted or warned with 
a dialog that a connection is going to be made. 

  The options are defined by the enum TCommDbDialogPref.
  
	Type: TUint32. Null: no
	
	  @see TCommDbDialogPref 
*/
#define CONNECT_PREF_DIALOG_PREF	(const TText*)_CONNECT_PREF_DIALOG_PREF

/** 
Identifier of the preferred IAP for this connection, if this is an IAP type database.

  For ISP type databases, see the subsequent fields.
  
	Type: TUint32. Null: yes 
*/
#define CONNECT_PREF_IAP			(const TText*)_CONNECT_PREF_IAP

//#define AGENT_SERVICE_TYPE			(const TText*)_AGENT_SERVICE_TYPE
//#define AGENT_NAME					(const TText*)_AGENT_NAME
//#define AGENT_EXT_NAME				(const TText*)_AGENT_EXT_NAME
//#define AGENT_LAST_SOCKET_ACTIVITY_TIMEOUT	(const TText*)_AGENT_LAST_SOCKET_ACTIVITY_TIMEOUT
//#define AGENT_LAST_SESSION_CLOSED_TIMEOUT	(const TText*)_AGENT_LAST_SESSION_CLOSED_TIMEOUT
//#define AGENT_LAST_SOCKET_CLOSED_TIMEOUT	(const TText*)_AGENT_LAST_SOCKET_CLOSED_TIMEOUT
#define SQL_START_SQL_QUERY			(const TText*)_SQL_START_SQL_QUERY
#define SQL_WHERE_KEYWORD			(const TText*)_SQL_WHERE_KEYWORD
#define SQL_AND_KEYWORD				(const TText*)_SQL_AND_KEYWORD
#define SQL_ORDER_KEYWORD			(const TText*)_SQL_ORDER_KEYWORD
#define SQL_WHERE_HIDDEN			(const TText*)_SQL_WHERE_HIDDEN
#define SQL_AND_HIDDEN				(const TText*)_SQL_AND_HIDDEN
#define SQL_INT_MATCH				(const TText*)_SQL_INT_MATCH
#define SQL_WHERE_INT_MATCH			(const TText*)_SQL_WHERE_INT_MATCH
#define SQL_WHERE_TEXT_MATCH		(const TText*)_SQL_WHERE_TEXT_MATCH
#define SQL_AND_INT_MATCH			(const TText*)_SQL_AND_INT_MATCH
#define SQL_AND_TEXT_MATCH			(const TText*)_SQL_AND_TEXT_MATCH
#define SQL_OR_TEXT_MATCH			(const TText*)_SQL_OR_TEXT_MATCH
#define SQL_WHERE_INT_GREATERTHAN	(const TText*)_SQL_WHERE_INT_GREATERTHAN

/**
Reasons for server Panic.
@internalComponent 
*/
enum TCommsDbSvrPanic
	{
	ECommsDbIncorrectNumberOfDbTables,
	ECommsDbInsertNotOpenIdle,
	ECommsDbUpdateNotOpenIdle,
	ECommsDbDeleteNotOpenIdle,
	ECommsDbSvrPanicCommitNotInsertUpdate,
	ECommsDbCancelNotInsertUpdate,
	ECommsDbGotoFirstNotOpenIdle,
	ECommsDbGotoNextNotOpenIdle,
	ECommsDbGotoPreviousNotOpenIdle,
	ECommsDbDeprecated9,
	ECommsDbReadNotOpenIdle,
	ECommsDbDeprecated11,
	ECommsDbWriteNotInsertUpdate,
	ECommsDbWritingIdColumn,
	ECommsDbOverrideValueLengthTooShort,
	ECommsDbColDoesNotExist,
	ECommsDbDirectionUnknown,
	ECommsDbDeprecated17,
	ECommsDbDeprecated18,
	ECommsDbMissingDefaultDatabase,
	ECommsDBOpenNonNullColSet,
	ECommsDbNoAccessTableView
	,
	ECommsDbPublishAndSubscribeError,
	/** Unable to perform the copy of the default commdb, or restore commdb
	operation.
	*/
	ECommdbRestoreFailed
	};

	/** 
	Panics the calling thread. 
	@internalComponent 
*/
GLREF_C void CommsDbPanic(TCommsDbSvrPanic aPanic);

#endif
