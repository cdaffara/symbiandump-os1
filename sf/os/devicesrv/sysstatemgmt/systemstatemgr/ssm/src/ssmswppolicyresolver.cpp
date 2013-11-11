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
#include "ssmswppolicyframe.h"
#include "ssmswppolicyresolver.h"
#include "ssmdebug.h"
#include "ssmserverpanic.h"
#include "ssmswppolicymap.h"

_LIT(KRomDriveLetter, "Z:");
_LIT(KDllFilenamePostfix, ".dll");

/**
Empty constructor
*/
CSsmSwpPolicyResolver::CSsmSwpPolicyResolver()
	{
	}

/**
Destructor cleans up internal resources
*/
CSsmSwpPolicyResolver::~CSsmSwpPolicyResolver()
	{
	delete iSwpPolicyMap;
	}

/**
Static Factory
*/
CSsmSwpPolicyResolver* CSsmSwpPolicyResolver::NewL()
	{
	CSsmSwpPolicyResolver* self = new (ELeave) CSsmSwpPolicyResolver();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Construct policy map object
*/
void CSsmSwpPolicyResolver::ConstructL()
	{
	iSwpPolicyMap = CSsmSwpPolicyMap::NewL();
	}


/**
Register an Swp and its policy file with the map

@param aSwp the swp value to register
@param aFile The file name to associate with the swp value
@leave KErrAlreadyExists if the swp value already exists
@leave KErrArgument if aFilename is larger than KMaxFileName
*/
void CSsmSwpPolicyResolver::RegisterSwpMappingL(TUint aSwpKey, const TDesC& aFilename)
	{
	SSMLOGLEAVEIFNULL(iSwpPolicyMap);
	DEBUGPRINT3(_L("CSsmSwpPolicyResolver::RegisterSwpMappingL swp %d file %S"), aSwpKey, &aFilename);
	if(aFilename.Length() > KMaxFileName)
		{
		DEBUGPRINT1(_L("Filename too long"));
		SSMLOGLEAVE(KErrArgument); //lint !e527 Suppress Unreachable. Lint is just confused by macro, warning goes away when code in macro gets expanded
		}
	iSwpPolicyMap->AddL(aSwpKey, aFilename);
	}  //lint !e1746 Suppress parameter 'aSwpKey' could be made const reference

/**
 * Used only for testing purposes
 * DeRegister an Swp and its policy file with the map

 * @param aSwpKey the swp value to deregister
 * @param aFilename The file name to associate with the swp value
 * @leave KErrNotFound if the swp value doesn't exists
 * @leave KErrArgument if aFilename is longer than KMaxFileName
*/
#ifdef _DEBUG
void CSsmSwpPolicyResolver::DeRegisterSwpMappingL(TUint aSwpKey, const TDesC& aFilename)
	{
	SSMLOGLEAVEIFNULL(iSwpPolicyMap);
	DEBUGPRINT3(_L("CSsmSwpPolicyResolver::DeRegisterSwpMappingL swp %d file %S"), aSwpKey, &aFilename);
	if(aFilename.Length() > KMaxFileName)
		{
		DEBUGPRINT1(_L("Filename too long"));
		SSMLOGLEAVE(KErrArgument); //lint !e527 Suppress Unreachable. Lint is just confused by macro, warning goes away when code in macro gets expanded
		}
	iSwpPolicyMap->DeleteSwpMapL(aSwpKey);
	}
#endif

/**
Construct a policy frame object based on the SWP value supplied

@param aSwp The swp to resolve
@return A policy frame containing a valid policy for the swp value
@leave KErrNotFound If the swp is not registered
@leave Any error returned from LoadLibraryLC
*/
CSsmSwpPolicyFrame* CSsmSwpPolicyResolver::GetSwpPolicyL(const TSsmSwp& aSwp)
	{
	TBuf<KMaxFileName + 2> libraryFilename; //KMaxFileName + KRomDriveLetter().Length()
	GetFileNameForSwpL(aSwp, libraryFilename);

	RLibrary library;
	LoadLibraryLC(library, libraryFilename);

	// make a process-owned copy of the handle as it has to be used by SsmSwpPolicyServer, 
	// which runs in a separate thread
	User::LeaveIfError(library.Duplicate(RThread(), EOwnerProcess));
	
	CSsmSwpPolicyFrame* policy = CreatePolicyLC(library);
	CleanupStack::Pop(policy);
	policy->SetLibrary(library); // takes ownership of open library handle
	CleanupStack::Pop(&library);
	policy->SetSwpKey(aSwp.Key());

	DEBUGPRINT2(_L("Created Swp Policy DLL %S"), &libraryFilename);

	return policy;
	}

/**
Construct a descriptor containing the composite file name from the DLL to load

@param aSwp The swp to create the file name for
@param aLibraryFilename reference to object in which file name is returned
@leave KErrNotFound If the swp is not registered
*/
void CSsmSwpPolicyResolver::GetFileNameForSwpL(const TSsmSwp& aSwp, TDes& aLibraryFilename) const
	{
	aLibraryFilename.Zero();
	aLibraryFilename.Append(KRomDriveLetter);
	aLibraryFilename.Append(iSwpPolicyMap->FilenameL(aSwp.Key()));
	const TInt postfixLength = KDllFilenamePostfix().Length();
	if(KDllFilenamePostfix().CompareF(aLibraryFilename.Right(postfixLength)))
		{
		aLibraryFilename.Append(KDllFilenamePostfix);
		}
	}

/**
Attempt to load the library with the supplied name

@param aLibrary Library object used to load the DLL
@param aLibraryFilename file name of library to load
@leave KErrNotSupported If UID2 of the DLL is not KStatePolicyDllTypeUidValue
@leave KErrNotFound If the requested policy DLL file is missing.
@leave KErrCorrupt Or any other system wide error code that the fileserver can raise.
*/
void CSsmSwpPolicyResolver::LoadLibraryLC(RLibrary& aLibrary, const TDesC& aLibraryFilename) const
	{
	CleanupClosePushL(aLibrary);
	const TInt fileErr = aLibrary.Load(aLibraryFilename);
	if (fileErr != KErrNone)
		{
		DEBUGPRINT3(_L("Failed to load library file %S, file error-code: %d"), &aLibraryFilename, fileErr);
		User::Leave(fileErr);
		}
	if (aLibrary.Type()[1] != KSsmSwpPolicyDllTypeUid)
		{
		DEBUGPRINT4(_L("Wrong type (uid2) in swp policy library dll %S. Expected %x found %x"),
					&aLibraryFilename, KSsmSwpPolicyDllTypeUid, aLibrary.Type()[1]);
		User::Leave(KErrNotSupported);
		}
	}

/**
Create a policy frame that encapsulates the policy dll represented by the library
Retrieves the DLLs factory method which is passed to the frame in oder to create the policy object

@param aLibrary The library object that references the DLL
@return A policy frame containing the loaded policy
@leave Any system error
*/
CSsmSwpPolicyFrame* CSsmSwpPolicyResolver::CreatePolicyLC(const RLibrary& aLibrary) const
	{
	__ASSERT_DEBUG( KNullHandle != aLibrary.Handle(), PanicNow(KPanicSysStateMgr, ESwpPolicyResolverLibraryNotLoaded));

	CSsmSwpPolicyFrame* frame = NULL;
	TRAPD(err, frame = CSsmSwpPolicyFrame::NewL(aLibrary.Lookup(1)));
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


