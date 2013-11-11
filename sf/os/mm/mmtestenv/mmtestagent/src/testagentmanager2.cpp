// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// f32agentmanager.cpp
// 
//

#include <caf/caf.h>
#include "testagentmanager2.h"
#include "testfileconstants.h"

using namespace ContentAccess;

_LIT(KTestDrmContentExtension, "DRM");

CTestAgentManager* CTestAgentManager::NewL()
	{
	CTestAgentManager* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentManager* CTestAgentManager::NewLC()
	{
	CTestAgentManager* self=new(ELeave) CTestAgentManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestAgentManager::CTestAgentManager()
	{
	}

CTestAgentManager::~CTestAgentManager()
	{
	}

void CTestAgentManager::ConstructL()
	{
	}

TInt CTestAgentManager::DeleteFile(const TDesC &aFileName)
	{
	// Most likely would have some kind of rights check here to determine if it can be
	// deleted
	RFs fs;
	TInt err = fs.Connect();
	if(err != KErrNone)
		return err;
	CleanupClosePushL(fs);
	err = fs.Delete(aFileName);
	CleanupStack::PopAndDestroy();
	return err;
	}

TInt CTestAgentManager::CopyFile(const TDesC& /*aSource*/, const TDesC& /*aDestination*/)
	{
	return KErrNone;
	}

TInt CTestAgentManager::RenameFile(const TDesC& /*aSource*/, const TDesC& /*aDestination*/)
	{
	return KErrNone;
	}

TInt CTestAgentManager::MkDir(const TDesC& /*aPath*/)
	{
	return KErrNone;
	}

TInt CTestAgentManager::MkDirAll(const TDesC& /*aPath*/)
	{
	return KErrNone;
	}

TInt CTestAgentManager::RmDir(const TDesC& /*aPath*/)
	{
	return KErrNone;
	}

TInt CTestAgentManager::GetDir(const TDesC& /*aName*/,TUint /*anEntryAttMask*/,TUint /*anEntrySortKey*/, CDir*& /*anEntryList*/) const
	{
	return KErrNone;
	}

TInt CTestAgentManager::GetDir(const TDesC& /*aName*/,TUint /*anEntryAttMask*/,TUint /*anEntrySortKey*/, CDir*& /*anEntryList*/,CDir*& /*aDirList*/) const
	{
	return KErrNone;
	}

TInt CTestAgentManager::GetDir(const TDesC& /*aName*/,const TUidType& /*anEntryUid*/,TUint /*anEntrySortKey*/, CDir*& /*aFileList*/) const
	{
	return KErrNone;
	}

TInt CTestAgentManager::GetAttribute(TInt /*aAttribute*/, TInt& /*aValue*/, const TVirtualPathPtr& /*aVirtualPath*/)
	{
	return KErrNone;
	}

TInt CTestAgentManager::GetAttributeSet(RAttributeSet& /*aAttributeSet*/, const TVirtualPathPtr& /*aVirtualPath*/)
	{
	return KErrNone;
	}

TInt CTestAgentManager::GetStringAttribute(TInt /*aAttribute*/, TDes& /*aValue*/, const TVirtualPathPtr& /*aVirtualPath*/)
	{
	return KErrNone;
	}

TInt CTestAgentManager::GetStringAttributeSet(RStringAttributeSet& /*aStringAttributeSet*/, const TVirtualPathPtr& /*aVirtualPath*/)
	{
	return KErrNone;
	}

void CTestAgentManager::NotifyStatusChange(const TDesC& /*aURI*/, TEventMask /*aMask*/, TRequestStatus& /*aStatus*/)
	{

	}

TInt CTestAgentManager::CancelNotifyStatusChange(const TDesC& /*aURI*/, TRequestStatus& /*aStatus*/)
	{
	return KErrNone;
	}

TInt CTestAgentManager::SetProperty(TAgentProperty /*aProperty*/, TInt /*aValue*/)
	{
	return KErrNone;
	}

void CTestAgentManager::DisplayInfoL(TDisplayInfo /*aInfo*/, const TVirtualPathPtr& /*aVirtualPath*/)
	{

	}

TBool CTestAgentManager::IsRecognizedL(const TDesC& aURI, TContentShareMode /*aShareMode*/) const
	{
	TInt result;

	// Convert URI to uppercase
	HBufC *uriUpper = aURI.AllocL();
	TPtr uriPtr(uriUpper->Des());
	uriPtr.UpperCase();

	// Check that the file extension is .DRM
	TPtrC extension = uriPtr.Right(KTestDrmContentExtension().Length());
	result = extension.Compare(KTestDrmContentExtension);
	delete uriUpper;

	if(result == 0)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

TBool CTestAgentManager::IsRecognizedL(RFile& /*aFile*/) const
	{
	return EFalse;
	}

TBool CTestAgentManager::RecognizeFileL(const TDesC& /*aFileName*/, const TDesC8& /*aBuffer*/, TDes8& /*aFileMimeType*/, TDes8& /*aContentMimeType*/) const
	{
	// don't return anything to apparc for now
	return EFalse;
	}

TInt CTestAgentManager::AgentSpecificCommand(TInt /*aCommand*/, const TDesC8& /*aInputBuffer*/, TDes8& /*aOutputBuffer*/)
	{
	return KErrNone;
	}

void CTestAgentManager::AgentSpecificCommand(TInt /*aCommand*/, const TDesC8& /*aInputBuffer*/, TDes8& /*aOutputBuffer*/, TRequestStatus& /*aStatus*/)
	{

	}

void CTestAgentManager::DisplayManagementInfoL()
	{

	}

void CTestAgentManager::PrepareHTTPRequestHeaders(RStringPool& /*aStringPool*/, RHTTPHeaders& /*aRequestHeaders*/) const
	{

	}

