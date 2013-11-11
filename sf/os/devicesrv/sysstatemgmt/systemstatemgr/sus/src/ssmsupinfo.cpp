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

#include <s32strm.h>
#include <ssm/ssmsupinfo.h>

/**
 Default constructor.
 */
EXPORT_C TSsmSupInfo::TSsmSupInfo() 
	: iFileName(KNullDesC), iNewLOrdinal(1), iIdentity(KNullUid), iSpare(0)
	{
	}

/**
 Constructor that lets you specify a DLL file name only.
 The DLL file name should not include the path or drive. Utility Plugin DLLs
 will only be loaded from the Z: drive.
 
 The optional iIdentity member will be set to KNullUid and iNewLOrdinal will default to 1.
 
 @param aFileName A Utility Plugin  DLL file name without drive or path
 */
EXPORT_C TSsmSupInfo::TSsmSupInfo(const TDesC& aFileName) 
	: iFileName(aFileName), iNewLOrdinal(1), iIdentity(KNullUid), iSpare(0)
	{
	}

/**
 Constructor that lets you specify a DLL file name and ordinal function of 
 the DLL which provides the static MSsmUtility* NewL() function. The DLL file name 
 should not include the path or drive.  Utility Plugin DLLs will only be loaded 
 from the Z: drive.
 
 @param aFileName 		A Utility Plugin DLL file name without drive or path
 @param aNewLOrdinal	The ordinal number of the static MSsmUtility* NewL() function
 */
EXPORT_C TSsmSupInfo::TSsmSupInfo(const TDesC& aFileName, TInt aNewLOrdinal) 
	: iFileName(aFileName), iNewLOrdinal(aNewLOrdinal), iIdentity(KNullUid), iSpare(0)
	{
	}

/**
 Constructor that lets you specify a DLL file name, the UID3 you expect it to have and
 which ordinal function of the DLL provides the static MSsmUtility* NewL() function. 
 The Utility Plugin DLL must match both filename and expected UID.  The DLL file name 
 should not include the path or drive.  Utility Plugin DLLs will only be loaded from 
 the Z: drive.
 
 @param aFileName 		A Utility Plugin DLL file name without drive or path
 @param aNewLOrdinal	The ordinal number of the static MSsmUtility* NewL() function
 @param aIdentityUid	The expected UID3 in the DLL specified by @c aFileName
 */
EXPORT_C TSsmSupInfo::TSsmSupInfo(const TDesC& aFileName, TInt aNewLOrdinal, const TUid& aIdentityUid) 
	: iFileName(aFileName), iNewLOrdinal(aNewLOrdinal), iIdentity(aIdentityUid), iSpare(0)
	{
	}

/**
 Destructor.
 */
EXPORT_C TSsmSupInfo::~TSsmSupInfo() 
	{
	}

/**
 Setter for member data.
 @param aFileName 		A Utility Plugin DLL file name without drive or path
 @param aNewLOrdinal	The ordinal number of the static MSsmUtility* NewL() function
 @param aIdentityUid	The expected UID3 in the DLL specified by @c aFileName, or KNullUid 
 						to skip this optional security check.
 */
EXPORT_C void TSsmSupInfo::SetL(const TDesC& aFileName, TInt aNewLOrdinal, const TUid& aIdentityUid)
	{
	iFileName = aFileName;
	iNewLOrdinal = aNewLOrdinal;
	iIdentity = aIdentityUid;
	}

/**
 @return The Uid stored in this object
 */
EXPORT_C TUid TSsmSupInfo::Identity() const
	{
	return iIdentity;
	}

/**
 @return A const reference to the filename stored in this object
 */
EXPORT_C const TFileName& TSsmSupInfo::FileName() const
	{
	return iFileName;
	}

/**
 @return The ordinal number stored in this object
 */
EXPORT_C TInt TSsmSupInfo::NewLOrdinal() const
	{
	return iNewLOrdinal;
	}

EXPORT_C void TSsmSupInfo::InternalizeL( RReadStream& aReadStream )
	{
	aReadStream >> iFileName;
	aReadStream >> iIdentity;
	aReadStream >> iNewLOrdinal;
	}

EXPORT_C void TSsmSupInfo::ExternalizeL( RWriteStream& aWriteStream ) const
	{
	aWriteStream << iFileName;
	aWriteStream << iIdentity;
	aWriteStream << iNewLOrdinal;
	}

