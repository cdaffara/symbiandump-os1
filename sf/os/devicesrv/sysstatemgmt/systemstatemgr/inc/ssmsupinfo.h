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

#ifndef __SSMSUPINFO_H__
#define __SSMSUPINFO_H__

#include <e32cmn.h>
#include <e32def.h>

class RReadStream;
class RWriteStream;

/**
 A parameter class intended to hold the information needed to identify utility 
 plugins which are to be loaded into the Utility Server. These DLLs  must have 
 @c KSsmUtilityPluginDllTypeUid as their second UID and must have a function 
 that returns an @c MSsmUtility object.
 
 The first member of this class, filename, is mandatory to identify the plugin DLL. 
 The second member is supposed to point at the DLL ordinal function which 
 instantiates and returns an @c MSsmUtility object. The prototype for this function
 must be MSsmUtility* NewL(void)
 The third member can be used for an optional validation of the third UID of the DLL.
 
 @see MSsmUtility
 @see SSM_START_SSM_UTILITY_PLUGIN
 @publishedPartner
 @released
 */
NONSHARABLE_CLASS(TSsmSupInfo)
	{
public:
	IMPORT_C TSsmSupInfo();
	IMPORT_C TSsmSupInfo(const TDesC& aFileName);
	IMPORT_C TSsmSupInfo(const TDesC& aFileName, TInt aNewLOrdinal);
	IMPORT_C TSsmSupInfo(const TDesC& aFileName, TInt aNewLOrdinal, const TUid& aIdentityUid);
	IMPORT_C ~TSsmSupInfo();
	IMPORT_C void SetL(const TDesC& aFileName, TInt aNewLOrdinal, const TUid& aIdentityUid);
	IMPORT_C const TFileName& FileName() const;
	IMPORT_C TInt NewLOrdinal() const;
	IMPORT_C TUid Identity() const;
	IMPORT_C void InternalizeL( RReadStream& aReadStream );
	IMPORT_C void ExternalizeL( RWriteStream& aWriteStream ) const;
	
private:
	TFileName iFileName;	//Filename without drive or path
	TInt32 iNewLOrdinal;	//The ordinal position of the function: MSsmUtility* NewL(void)
	TUid iIdentity;			//Third UID, for an optional security-check
	TInt iSpare;
	};


#endif	// __SSMSUPINFO_H__
