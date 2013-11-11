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

#include <f32file.h>
#include <ssm/ssmstate.h>
#include "ssmstatepolicyframe.h"
#include "ssmstatepolicyresolver.h"
#include "ssmdebug.h"
#include "ssmserverpanic.h"

_LIT(KRomDriveLetter, "Z:");

/**
*/
CSsmStatePolicyResolver::CSsmStatePolicyResolver()
	{
	}

/**
*/
CSsmStatePolicyResolver::~CSsmStatePolicyResolver()
	{
	delete iPolicy;
	iLibrary.Close(); //Always ok to call Close()
	}

/**
*/
void CSsmStatePolicyResolver::ReleasePolicyResolver()
 	{
 	delete this;
 	}

/**
*/
CSsmStatePolicyResolver* CSsmStatePolicyResolver::NewL()
	{
	return new (ELeave) CSsmStatePolicyResolver();
	}

/**
*/
CSsmStatePolicyResolver* CSsmStatePolicyResolver::NewLC()
	{
	CSsmStatePolicyResolver* self = CSsmStatePolicyResolver::NewL();
	CleanupStack::PushL(self);
	return self;
	}

/**
*/
void CSsmStatePolicyResolver::GetStatePolicyL(TSsmState aState)
	{
	//const TInt KMaxStatePolicyFileName =  KRomDriveLetter().Length() + KSsmStatePolicyFilenamePrefix().Length() +	4 /*MainState in hex*/ + KSsmStatePolicyFilenamePostfix().Length();
	TBuf</*KMaxStatePolicyFileName*/32> libraryFilename;
	GetFileNameForState(aState, libraryFilename);

	if( NeedsLoading(libraryFilename) )
		{
		RLibrary nextLibrary;
		LoadLibraryLC(nextLibrary, libraryFilename);
		CSsmStatePolicyFrame* nextPolicy = CreatePolicyLC(nextLibrary);
		nextPolicy->SetStatePolicyId(aState.MainState());

		DEBUGPRINT2(_L("Switching to System State Policy DLL %S"), &libraryFilename);

		delete iPolicy;
		iPolicy = NULL;
		iPolicy = nextPolicy;
		CleanupStack::Pop(nextPolicy);

		iLibrary.Close();
		iLibrary = nextLibrary;
		CleanupStack::Pop(&nextLibrary);
		}
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
*/
CSsmStatePolicyFrame* CSsmStatePolicyResolver::Policy()
  	{
  	return iPolicy;
  	}

/**
@return ETrue if the requested library is not the same as the one we have already loaded
*/
TBool CSsmStatePolicyResolver::NeedsLoading(const TParsePtrC& aLibraryFilename) const
	{
	if (KNullHandle == iLibrary.Handle())
		{
		return ETrue;
		}

	TParsePtrC currentLibrary(iLibrary.FileName());
	if (aLibraryFilename.NameAndExt() != currentLibrary.NameAndExt())
		{
		return ETrue;
		}

	return EFalse;
	}

/**
*/
void CSsmStatePolicyResolver::GetFileNameForState(const TSsmState& aState, TDes& aLibraryFilename) const
	{
	aLibraryFilename.Zero();
	aLibraryFilename.Append(KRomDriveLetter);
	aLibraryFilename.Append(KSsmStatePolicyFilenamePrefix);
	aLibraryFilename.AppendNumFixedWidth(aState.MainState(), EHex, 4);
	aLibraryFilename.Append(KSsmStatePolicyFilenamePostfix);
	}

/**
@leave KErrNotSupported If UID2 of the DLL is not KStatePolicyDllTypeUidValue
@leave KErrNotFound If the requested policy DLL file is missing.
@leave KErrCorrupt Or any other system wide error code that the fileserver can raise.
*/
void CSsmStatePolicyResolver::LoadLibraryLC(RLibrary& aLibrary, const TDesC& aLibraryFilename) const
	{
	CleanupClosePushL(aLibrary);
	const TInt fileErr = aLibrary.Load(aLibraryFilename);
	if (fileErr != KErrNone)
		{
		DEBUGPRINT3(_L("Failed to load library file %S, file error-code: %d"), &aLibraryFilename, fileErr);
		User::Leave(fileErr);
		}
	if (aLibrary.Type()[1] != KSsmStatePolicyDllTypeUid)
		{
		DEBUGPRINT4(_L("Wrong type (uid2) in state policy library dll %S. Expected %x found %x"),
					&aLibraryFilename, KSsmStatePolicyDllTypeUid, aLibrary.Type()[1]);
		User::Leave(KErrNotSupported);
		}
	}

/**
*/
CSsmStatePolicyFrame* CSsmStatePolicyResolver::CreatePolicyLC(const RLibrary& aLibrary) const
	{
	__ASSERT_DEBUG(KNullHandle != aLibrary.Handle(), PanicNow(KPanicSysStateMgr, ESsmStateResolverError1));

	CSsmStatePolicyFrame* frame = NULL;
	TRAPD(err, frame = CSsmStatePolicyFrame::NewL(aLibrary.Lookup(1)));
	CleanupStack::PushL(frame);
#ifdef _DEBUG
	if(KErrNone != err)
		{
		TFileName name = aLibrary.FileName();
		DEBUGPRINT3(_L("Error %d when calling first function in State Policy DLL %S."), err, &name);
		}
#endif
	SSMLOGLEAVEIFERROR(err);
	return frame;
	}
