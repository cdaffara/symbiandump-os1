// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <s32strm.h>
#include <ssm/ssmcustomcommandinfo.h>

#include "ssmdebug.h"

/** 
Constructor
*/
EXPORT_C CSsmCustomCommandInfo::CSsmCustomCommandInfo()
	: iOrdinal(1)
	{
	}

/** 
Destructor
*/
EXPORT_C CSsmCustomCommandInfo::~CSsmCustomCommandInfo()
	{
	delete iParams;
	}


/** 
Set command info contents

@param aFileName	A Custom Command DLL file name
@param aOrdinal		The ordinal to call to return a MSsmCustomCommand object
@param aUnloading	Whether to unload the Custom Command DLL immediately when finished or not
@param aRetries		The number of times to try calling the Custom Command if unsuccessful
@param aParams		Optional user data to be passed to the Custom Command
@leave				One of the system-wide error codes.
*/
EXPORT_C void CSsmCustomCommandInfo::SetL(const TDesC& aFileName, TInt32 aOrdinal, TCmdCustomCommandLibUnloading aUnloading, TInt16 aRetries, const TDesC8& aParams)
	{
	iFileName = aFileName;
	iOrdinal = aOrdinal;
	iUnloading = aUnloading;
	iRetries = aRetries;

	delete iParams;
	iParams = NULL;
	if (aParams.Length())
		{
		iParams = aParams.AllocL();
		}
	}

/** 
Clear command info contents and reset to default values
*/
EXPORT_C void CSsmCustomCommandInfo::Clear()
	{
	iFileName = KNullDesC;
	iOrdinal = 1;
	iUnloading = EUnloadOnCommandCompletion;
	iRetries = 0;

	delete iParams;
	iParams = NULL;
	}

/** 
@return The DLL name, or KNullDesC if empty
*/
EXPORT_C const TFileName& CSsmCustomCommandInfo::FileName() const
	{
	return iFileName;
	}

/** 
@return The ordinal called to obtain the MSsmCustomCommand object
*/
EXPORT_C TInt32 CSsmCustomCommandInfo::Ordinal() const
	{
	return iOrdinal;
	}

/** 
@return DLL unloading rule whether to unload the custom command dll after the command has completed or not at all
@see TCmdCustomCommandLibUnloading
*/
EXPORT_C TCmdCustomCommandLibUnloading CSsmCustomCommandInfo::Unloading() const
	{
	return iUnloading;
	}

/** 
@return Number of retries on failure
*/
EXPORT_C TInt16 CSsmCustomCommandInfo::Retries() const
	{
	return iRetries;
	}

/** 
@return Params or KNullDesC8 if empty
*/
EXPORT_C const TPtrC8 CSsmCustomCommandInfo::Params() const
	{
	TPtrC8 params;
	if (iParams)
		{
		params.Set(*iParams);
		}
	else
		{
		params.Set(KNullDesC8);
		}
	return params;
	}

/** 
Overloaded = operator
@return A Custom Command object containing the new values
*/
EXPORT_C CSsmCustomCommandInfo& CSsmCustomCommandInfo::operator=(const CSsmCustomCommandInfo& aInfo)
	{
	// protect against self assignment
	if( &aInfo != this)
		{
		// can't do anything with this if it leaves
		TRAP_IGNORE(SetL(aInfo.FileName(), aInfo.Ordinal(), aInfo.Unloading(), aInfo.Retries(), aInfo.Params()));
		}

	return (*this);
	}

/**
Configures the CSsmCustomCommandInfo object using data contained in a ReadStream
@param aReadStream 	A read stream containing BIC data
@leave				One of the system-wide error codes.
*/
EXPORT_C void CSsmCustomCommandInfo::InternalizeL(RReadStream& aReadStream)
	{
	aReadStream >> iFileName;
	iOrdinal = aReadStream.ReadInt32L();
	iUnloading = static_cast<TCmdCustomCommandLibUnloading>(aReadStream.ReadUint8L());
	iRetries = aReadStream.ReadInt16L();

	HBufC8* paramptr = HBufC8::NewLC(aReadStream, KMaxDLLParams);
	delete iParams;
	iParams = NULL;
	if (paramptr->Length())
		{
		paramptr->ReAllocL(paramptr->Length());
		iParams = paramptr;
		CleanupStack::Pop();
		}
	else
		{
		CleanupStack::PopAndDestroy();
		}
	}

/**
Externalises the CSsmCustomCommandInfo object
@param aWriteStream A write stream to write the object data to
@leave				One of the system-wide error codes.
*/
EXPORT_C void CSsmCustomCommandInfo::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream << iFileName;
  	aWriteStream.WriteInt32L(Ordinal());
  	aWriteStream.WriteUint8L(Unloading());
  	aWriteStream.WriteInt16L(Retries());
	aWriteStream << Params();
	}

