// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains wrapper helper functions for test code to control the test wrappers 
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <ecom/ecom.h>
#include "startupadaptationwrapper.h"

/**
 * Holds the fake TLS value
 * 
 * @test
 * @internalComponent
 */
static TAny* fakeTls = NULL;

/*
 * Redefined here as the test .exe is not a DLL so doesn't normally have TLS
 */
TInt Dll::SetTls(TAny* aPtr)
	{
	fakeTls = aPtr;
	return KErrNone;
	}

/*
 * Redefined here as the test .exe is not a DLL so doesn't normally have TLS
 */
TAny* Dll::Tls()
	{
	return fakeTls;
	}

/*
 * Wraps how to create a CImplementationInformation as this is not exported from ECOM
 */
CImplementationInformation* CImplementationInformation::NewL(
		TUid	aUid, 
		TInt	aVersion, 
		HBufC*  aName,
		HBufC8* aDataType,
		HBufC8* aOpaqueData,
		TDriveUnit aDrive,
		TBool aRomOnly,
		TBool aRomBased)
	{
	return new (ELeave) CImplementationInformation(aUid, aVersion, aName, aDataType, aOpaqueData, aDrive, aRomOnly, aRomBased);
	} //lint !e1746 Suppress parameters could be made const referenc

/*
 * Wraps how to create a CImplementationInformation as this is not exported from ECOM
 */
CImplementationInformation::CImplementationInformation(
								TUid aUid, 
								TInt aVersion, 
								HBufC*  aName, 
								HBufC8* aDataType,
								HBufC8* aOpaqueData,
								TDriveUnit aDrive,
								TBool aRomOnly,
								TBool aRomBased)
: 	iImplementationUid(aUid),
	iVersion(aVersion),
	iDisplayName(aName),
	iData(aDataType),
	iOpaqueData(aOpaqueData),
	iDisabled(EFalse),
	iDrive(aDrive),
	iRomOnly(aRomOnly),
	iRomBased(aRomBased),
	iVid()
	{
	
	} //lint !e1746 Suppress parameters could be made const referenc
								
/*
 * Wraps how to destroy a CImplementationInformation as this is not exported from ECOM
 */
CImplementationInformation::~CImplementationInformation()
	{
	delete iDisplayName;
	delete iData;
	delete iOpaqueData;
	}

								

/**
 * Fake implementation Uid for test
 * 
 * @test
 * @internalComponent
 */
static TUid KFakeImplId = {0x12345678};

void REComSession::ListImplementationsL(TUid aInterfaceUid, const TEComResolverParams& /*aResolutionParameters*/, TUid /*aResolverUid*/, RImplInfoPtrArray& aImplInfoArray)
	{
	if(aInterfaceUid != KStartupAdaptationIfUid)
		{
		// ECom Wrapper used for other use than in the test it is meant to wrap
		User::Leave(KErrNotSupported);
		}
	
	
	// Create a fake implementation information
	CImplementationInformation* fakeimpl = CImplementationInformation::NewL(
			KFakeImplId, // Fake implementation UID
			0, // Version 0
			KNullDesC().AllocL(), // blank name
			KNullDesC8().AllocL(), // blank data type
			KNullDesC8().AllocL(), // blank opaque data
			TDriveUnit(), // z drive based
			ETrue, // ROM only
			ETrue); // ROM based
	// Add it to the implementation information array
	aImplInfoArray.Append(fakeimpl);
	} //lint !e1746 Suppress parameter 'aInterfaceUid' could be made const referenc

/**
 * Fake destructor key
 * 
 * @test
 * @internalComponent
 */
static TUid KFakeDtorIDKey = {0x12123434};

TAny* REComSession::CreateImplementationL(TUid aImplementationUid, TUid& aDtorIDKey, TAny* aConstructionParameters)
	{
	if(aImplementationUid != KFakeImplId)
		{
		// ECom Wrapper used for other use than in the test it is meant to wrap
		User::Leave(KErrNotSupported);
		}
	aDtorIDKey = KFakeDtorIDKey;
	CStartupAdaptation* startupAdaptation = CStartupAdaptationWrapper::NewStartupAdaptation(aConstructionParameters);
	return startupAdaptation;
	} //lint !e1746 Suppress parameter 'aImplementationUid' could be made const referenc

void REComSession::DestroyedImplementation(TUid /*aDtorIDKey*/)
	{
	CStartupAdaptationWrapper::DeleteStartupAdaptation();
	}

void REComSession::FinalClose()
	{
	// Nothing to do
	}
