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

#ifndef MMFDRMPLUGINSERVERPROXY_H
#define MMFDRMPLUGINSERVERPROXY_H

#include <e32base.h>
#include <f32file.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include <caf/caf.h>
using namespace ContentAccess;



NONSHARABLE_CLASS( RMMFDRMPluginServerProxy ): public RMmfSessionBase
/**
*@internalTechnology
*/
	{
public:

	IMPORT_C TInt Open();
	IMPORT_C TInt LaunchControllerServer(TUint aMaxHeapSize, TBool aUseSharedHeap,
										TThreadId& aControllerThreadId, TUint aStackSize);
	IMPORT_C TInt GetControllerSessionHandle();
	IMPORT_C TInt PanicControllerThread(TThreadId aTid, const TDesC& aCategory,TInt aReason);
	IMPORT_C TInt KillControllerThread(TThreadId aTid, TInt aReason);
	IMPORT_C TInt SetThreadPriority(TThreadId aTid, TThreadPriority aPriority);
	
	// Data content functions are designed to be used by CMMFUtilityFileInfo
	IMPORT_C void OpenDataContentL(const TDesC& aFilePath, const TDesC8& aInitData);
	// File handle must be shared through the call RFs::ShareProtected(); 
	// otherwise KErrBadHandle will return
	IMPORT_C void OpenDataContentL(const RFile& aFile, const TDesC8& aInitData);
	IMPORT_C TInt EvaluateDataContentIntent(TIntent aIntent);
	IMPORT_C TBool GetDataContentMimeTypeL(TDes8& aMimeType);
	IMPORT_C void GetDataContentFileHeaderL(TDes8& aHeaderData, TInt aMaxLength);
	};

#endif
