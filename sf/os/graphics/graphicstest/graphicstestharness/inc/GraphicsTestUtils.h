// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __GRAPHICSTESTUTILS_H__
#define __GRAPHICSTESTUTILS_H__

#include <e32std.h>

typedef TUint8 TWipeItems;

_LIT(KTestServerName,"!GraphicsTestUtilsServer");
_LIT(KTestServerImg,"GraphicsTestUtilsServer");		// EXE name


enum TSmlProfileTransportType
	{
	ELocalTransport,
	ENetworkTransport,
	};

class RSmlTestUtils : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C void SetDevIdL(const TDesC8& aDevId);
	IMPORT_C void GetDevIdL(TDes8& aDevId);
	IMPORT_C TInt WipeDataStoreL(TWipeItems aItemsToClean);
	IMPORT_C TInt RenameDirectoryL(const TDesC16& aSrc,const TDesC16& aDest);
	IMPORT_C TInt CreateDirectoryL(const TDesC& aPath);
	IMPORT_C TInt DeleteDirectoryL(const TDesC& aPath);
	IMPORT_C TInt CreateFileL(const TDesC& aPath);
	IMPORT_C TInt DeleteFileUsingWildcardL(const TDesC& aPath);
	IMPORT_C TInt DeleteFileL(const TDesC& aPath);
	IMPORT_C TInt CopyFileL(const TDesC& aSrc,const TDesC& aDest);
	IMPORT_C TInt ReplaceFileL(const TDesC& aName,TUint aFileMode);
	IMPORT_C TInt IsFilePresent(const TDesC& aName, TBool &aPresent);
	IMPORT_C TInt GetAtt( const TDesC &aName, TUint &aAttValue );
	IMPORT_C TInt SetAtt( const TDesC &aName, TUint aSetAttMask, TUint aClearAttMask );
	IMPORT_C void SetHomeTimeL(const TTime aTime);
	IMPORT_C TInt SetReadOnly(const TDesC& aName,TUint aSetAttMask);
	IMPORT_C TInt CopyDirectoryL(const TDesC& aSource,const TDesC& aTarget);
	IMPORT_C TInt ChangeFilePermissionL(const TDesC& aPath);

	};

enum TSmlTestUtilsService //Services provided by the SmlTestUtils server
	{
	EChangeDevId,
	EGetDevId,
	EWipeDataStore,
	ECreateDir,
	ERenameDir,
	EDeleteDir,
	ECreateFile,
	EDeleteFile,
	EDeleteFileUsingWildcard,
	ECreateDSJob,
	EStopDSJob,
	ECreateDSProfile,
	ESetHomeTime,
	ECopyFile,
	EReplaceFile,
	EIsFilePresent,
	ESetReadOnly,
	EGetAttributes,
	ESetAttributes,
	ECopyDirectory,
	EChangeFilePermission,

	};
	
enum TStoreType //Types of store that can be deleted using WipeDataStoreL()
	{
	ECenRep		= 0x01,		
	ELocalSDS	= 0x02,
	EDMTree		= 0x04,
	ECenRepFiles = 0x08,// Deletes all the cen rep data files. This is a lengthy and volatile operation
					    // and should be used with caution. Note that this is not covered by "EAll"
					    // and should be called explicitly
	ESDS		= ELocalSDS | ECenRep,
	EAll		= 0xff
	};

#endif
