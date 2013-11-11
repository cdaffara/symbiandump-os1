// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Override functionality header 
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
 @internalComponent
*/

#ifndef OVERRIDE_H
#define OVERRIDE_H

#include <s32strm.h>
#include <commdb.h>
#include <cdbpreftable.h>
#include <cdbover.h>

NONSHARABLE_CLASS(CCommDbOverride) : public CBase
/**
@internalComponent
*/
	{
public:
	CCommDbOverride(CCommDbOverrideSettings::TValueType aType);
	virtual void InternalizeL(RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;
	static TBool MatchOverrides(const CCommDbOverride& aFirst, const CCommDbOverride& aSecond);

public:
	CCommDbOverrideSettings::TValueType iType;
	TBuf<KCommsDbSvrMaxColumnNameLength> iTableName;
	TBuf<KCommsDbSvrMaxColumnNameLength> iColumnName;
	};

NONSHARABLE_CLASS(CCommDbIntOverride) : public CCommDbOverride
/**
@internalComponent
*/
	{
public:
	CCommDbIntOverride(const TDesC& aTable, const TDesC& aColumn);
	virtual void InternalizeL(RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;

public:
	TInt iValue;
	};

NONSHARABLE_CLASS(CCommDbBoolOverride) : public CCommDbOverride
/**
@internalComponent
*/
	{
public:
	CCommDbBoolOverride(const TDesC& aTable, const TDesC& aColumn);
	virtual void InternalizeL(RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;

public:
	TBool iValue;
	};

NONSHARABLE_CLASS(CCommDbDes8Override) : public CCommDbOverride
/**
@internalComponent
*/
	{
public:
	CCommDbDes8Override(const TDesC& aTable, const TDesC& aColumn);
	virtual void InternalizeL(RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;

public:
	TBuf8<KCommsDbSvrRealMaxFieldLength> iValue;
	};

NONSHARABLE_CLASS(CCommDbDes16Override) : public CCommDbOverride
/**
@internalComponent
*/
	{
public:
	CCommDbDes16Override(const TDesC& aTable, const TDesC& aColumn);
	virtual void InternalizeL(RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;

public:
	TBuf16<KCommsDbSvrRealMaxFieldLength> iValue;
	};

class CCommDbLongDesOverride;
typedef CCommDbLongDesOverride* CCommDbLongDesOverridePtr;

NONSHARABLE_CLASS(CCommDbLongDesOverride) : public CCommDbOverride
/**
@internalComponent
*/
	{
public:
	static CCommDbLongDesOverride* NewL(const TDesC& aTable, const TDesC& aColumn);
	static CCommDbLongDesOverride* NewL(const TDesC& aTable, const TDesC& aColumn, const TDesC& aValue);
	CCommDbLongDesOverride();
	virtual ~CCommDbLongDesOverride();
	virtual void InternalizeL(RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;

private:
	CCommDbLongDesOverride(const TDesC& aTable, const TDesC& aColumn);
	void ConstructL(const TDesC& aValue);

public:
	HBufC* iValue;
	};

NONSHARABLE_CLASS(TCommDbIapConnectionPrefOverride)
/**
@internalComponent
*/
	{
public:
	TCommDbIapConnectionPrefOverride();
	void InternalizeL(RReadStream& aStream);
	virtual void ExternalizeL(RWriteStream& aStream) const;

public:
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref iPref;
	};

#endif
