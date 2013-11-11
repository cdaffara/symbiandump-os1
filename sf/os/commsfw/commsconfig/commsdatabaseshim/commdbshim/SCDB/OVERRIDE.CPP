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
// Override settings
// 
//

/**
 @file
 @deprecated since v9.1. Functionality is replaced with commsdat.
*/

#include "OVERRIDE.H"

// CCommDbOverride

CCommDbOverride::CCommDbOverride(CCommDbOverrideSettings::TValueType aType)
	: iType(aType), iTableName(), iColumnName()
/**
Constructor

@param aType The value types that may be stored in as overrides
*/
	{}

void CCommDbOverride::InternalizeL(RReadStream& aStream)
/**
Internalises the override settings from a stream

@param aStream The stream to read from
*/
	{
	iType = (CCommDbOverrideSettings::TValueType) aStream.ReadUint32L();
	aStream >> iTableName;
	aStream >> iColumnName;
	}

void CCommDbOverride::ExternalizeL(RWriteStream& aStream) const
/**
Externalises override settings to a stream.

@param aStream The stream to write to
*/
	{
	aStream.WriteUint32L(iType);
	aStream << iTableName;
	aStream << iColumnName;
	}

TBool CCommDbOverride::MatchOverrides(const CCommDbOverride& aFirst,
									  const CCommDbOverride& aSecond)
/**
Checks if the iColumnName, iTableName fields in the passed objects of CCommDbOverride are equal.

@param aFirst A reference to CCommDbOverride class
@param aSecond A reference to CCommDbOverride class
@return ETrue if Successful else EFalse
*/
	{
	return (aFirst.iColumnName == aSecond.iColumnName &&
			aFirst.iTableName == aSecond.iTableName);
	}

// CCommDbIntOverride

CCommDbIntOverride::CCommDbIntOverride(const TDesC& aTable, const TDesC& aColumn)
	: CCommDbOverride(CCommDbOverrideSettings::EIntValue)
/**
Constructor

@param aTable A reference to a descriptor containing the name Table
@param aColumn A reference to a descriptor containing the name of a column in table aTableName.
*/
	{
	iTableName.Copy(aTable);
	iColumnName.Copy(aColumn);
	}

void CCommDbIntOverride::InternalizeL(RReadStream& aStream)
/**
Internalises the override settings from a stream

@param aStream The stream to read from
*/
	{
	iValue = aStream.ReadUint32L();
	}

void CCommDbIntOverride::ExternalizeL(RWriteStream& aStream) const
/**
Externalises override settings to a stream.

@param aStream The stream to write to
*/
	{
	CCommDbOverride::ExternalizeL(aStream);
	aStream.WriteUint32L(iValue);
	}

// CCommDbBoolOverride

CCommDbBoolOverride::CCommDbBoolOverride(const TDesC& aTable, const TDesC& aColumn)
	: CCommDbOverride(CCommDbOverrideSettings::EBoolValue)
/**
Constructor

@param aTable A reference to a descriptor containing the name Table
@param aColumn A reference to a descriptor containing the name of a column in table aTableName.
*/
	{
	iTableName.Copy(aTable);
	iColumnName.Copy(aColumn);
	}

void CCommDbBoolOverride::InternalizeL(RReadStream& aStream)
/**
Internalises the override settings from a stream

@param aStream The stream to read from
*/
	{
	iValue = (TBool)aStream.ReadUint32L();
	}

void CCommDbBoolOverride::ExternalizeL(RWriteStream& aStream) const
/**
Externalises override settings to a stream.

@param aStream The stream to write to
*/
	{
	CCommDbOverride::ExternalizeL(aStream);
	aStream.WriteUint32L((TBool)iValue);
	}

//CCommDbDes8Override

CCommDbDes8Override::CCommDbDes8Override(const TDesC& aTable, const TDesC& aColumn)
	: CCommDbOverride(CCommDbOverrideSettings::EDes8Value)
/**
Constructor

@param aTable A reference to a descriptor containing the name Table
@param aColumn A reference to a descriptor containing the name of a column in table aTableName.
*/
	{
	iTableName.Copy(aTable);
	iColumnName.Copy(aColumn);
	}

void CCommDbDes8Override::InternalizeL(RReadStream& aStream)
/**
Internalises the override settings from a stream

@param aStream The stream to read from
*/
	{
	aStream >> iValue;
	}

void CCommDbDes8Override::ExternalizeL(RWriteStream& aStream) const
/**
Externalises override settings to a stream.

@param aStream The stream to write to
*/
	{
	CCommDbOverride::ExternalizeL(aStream);
	aStream << iValue;
	}

//CCommDbDes16Override

CCommDbDes16Override::CCommDbDes16Override(const TDesC& aTable, const TDesC& aColumn)
	: CCommDbOverride(CCommDbOverrideSettings::EDes16Value)
/**
Constructor

@param aTable A reference to a descriptor containing the name Table
@param aColumn A reference to a descriptor containing the name of a column in table aTableName.
*/
	{
	iTableName.Copy(aTable);
	iColumnName.Copy(aColumn);
	}

void CCommDbDes16Override::InternalizeL(RReadStream& aStream)
/**
Internalises the override settings from a stream

@param aStream The stream to read from
*/
	{
	aStream >> iValue;
	}

void CCommDbDes16Override::ExternalizeL(RWriteStream& aStream) const
/**
Externalises override settings to a stream.

@param aStream The stream to write to
*/
	{
	CCommDbOverride::ExternalizeL(aStream);
	aStream << iValue;
	}

// CCommDbLongDesOverride

CCommDbLongDesOverride* CCommDbLongDesOverride::NewL(const TDesC& aTable, const TDesC& aColumn)
/**
Allocates and constructs a new communications database override object on the heap.

@param aTable A reference to a descriptor containing the name Table
@param aColumn A reference to a descriptor containing the name of a column in table aTableName.
@return A pointer to a communications database override object.
*/
	{
	CCommDbLongDesOverride* r = new(ELeave) CCommDbLongDesOverride(aTable,aColumn);
	return r;
	}

CCommDbLongDesOverride* CCommDbLongDesOverride::NewL(const TDesC& aTable, const TDesC& aColumn, const TDesC& aValue)
/**
Allocates and constructs a new communications database override object on the heap.

@param aTable A reference to a descriptor containing the name Table
@param aColumn A reference to a descriptor containing the name of a column in table aTableName.
@param aValue A reference to a descriptor passed by the caller.
@return A pointer to a communications database override object.
*/
	{
	CCommDbLongDesOverride* r = new(ELeave) CCommDbLongDesOverride(aTable,aColumn);
	CleanupStack::PushL(r);
	r->ConstructL(aValue);
	CleanupStack::Pop(r);
	return r;
	}

CCommDbLongDesOverride::CCommDbLongDesOverride(const TDesC& aTable, const TDesC& aColumn)
	: CCommDbOverride(CCommDbOverrideSettings::ELongDesValue)
/**
Constructor

@param aTable A reference to a descriptor containing the name Table
@param aColumn A reference to a descriptor containing the name of a column in table aTableName.
*/
	{
	iTableName.Copy(aTable);
	iColumnName.Copy(aColumn);
	}

void CCommDbLongDesOverride::ConstructL(const TDesC& aValue)
	{
	iValue = aValue.AllocL();
	}

CCommDbLongDesOverride::~CCommDbLongDesOverride()
/**
Destructor
*/
	{
	delete iValue;
	}

// Would it be possible to stream iValue directly?
void CCommDbLongDesOverride::InternalizeL(RReadStream& aStream)
/**
Internalises the override settings from a stream

@param aStream The stream to read from
*/
	{
	TUint32 length=aStream.ReadUint32L();
	if (iValue != NULL)
		{
		delete iValue;
		}
	iValue = HBufC::NewL(aStream, length);
	}

void CCommDbLongDesOverride::ExternalizeL(RWriteStream& aStream) const
/**
Externalises override settings to a stream.

@param aStream The stream to write to
*/
	{
	CCommDbOverride::ExternalizeL(aStream);
	aStream.WriteUint32L(iValue->Length());
	aStream << iValue->Des();
	}

//TCommDbIapConnectionPrefOverride

TCommDbIapConnectionPrefOverride::TCommDbIapConnectionPrefOverride() :
/**
Constructor
*/
	iPref()
	{}

void TCommDbIapConnectionPrefOverride::InternalizeL(RReadStream& aStream)
/**
Internalises the override settings from a stream

@param aStream The stream to read from
*/
	{
	iPref.iRanking = aStream.ReadUint32L();
	iPref.iDirection = (TCommDbConnectionDirection)aStream.ReadUint32L();
	iPref.iDialogPref = (TCommDbDialogPref)aStream.ReadUint32L();
	iPref.iBearer.iBearerSet = aStream.ReadUint32L();
	iPref.iBearer.iIapId = aStream.ReadUint32L();
	}

void TCommDbIapConnectionPrefOverride::ExternalizeL(RWriteStream& aStream) const
/**
Externalises override settings to a stream.

@param aStream The stream to write to
*/
	{
	aStream.WriteUint32L(iPref.iRanking);
	aStream.WriteUint32L(iPref.iDirection);
	aStream.WriteUint32L(iPref.iDialogPref);
	aStream.WriteUint32L(iPref.iBearer.iBearerSet);
	aStream.WriteUint32L(iPref.iBearer.iIapId);
	}
