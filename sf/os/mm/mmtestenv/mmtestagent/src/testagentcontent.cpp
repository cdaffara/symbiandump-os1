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
//

#include <caf/caf.h>
#include <caf/attribute.h>
#include "testagentcontent.h"
#include <caf/bitset.h>
#include "testfileconstants.h"

using namespace ContentAccess;

CTestAgentContent* CTestAgentContent::NewL(const TDesC& aURI, TContentShareMode aMode)
	{
	CTestAgentContent* self = NewLC(aURI, aMode);
	CleanupStack::Pop(self);
	return self;
	}

CTestAgentContent* CTestAgentContent::NewLC(const TDesC& aURI, TContentShareMode aMode)
	{
	CTestAgentContent* self=new(ELeave) CTestAgentContent();
	CleanupStack::PushL(self);
	self->ConstructL(aURI, aMode);
	return self;
	}

CTestAgentContent::CTestAgentContent()
	{
	}

CTestAgentContent::~CTestAgentContent()
	{
	}

void CTestAgentContent::ConstructL(const TDesC& /*aURI*/, TContentShareMode /*aMode*/)
	{
	}

TInt CTestAgentContent::OpenContainer(const TDesC& /*aUniqueId*/)
	{
	return KErrNone;
	}

TInt CTestAgentContent::CloseContainer()
	{
	return KErrNone;
	}

void CTestAgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& /*aArray*/)
	{

	}

void CTestAgentContent::GetEmbeddedObjectsL(RStreamablePtrArray<CEmbeddedObject>& /*aArray*/, TEmbeddedType /*aType*/)
	{

	}

TInt CTestAgentContent::Search(RStreamablePtrArray<CEmbeddedObject>& /*aArray*/, const TDesC8& /*aMimeType*/, TBool /*aRecursive*/)
	{
	return KErrNone;
	}

TInt CTestAgentContent::GetAttribute(TInt /*aAttribute*/, TInt& /*aValue*/, const TDesC& /*aUniqueId*/)
	{
	return KErrNone;
	}

TInt CTestAgentContent::GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& /*aUniqueId*/)
	{
	TInt err = KErrNone;
	aAttributeSet.SetValue(EIsProtected, 1, err);
	aAttributeSet.SetValue(EIsForwardable, 0, err);
	aAttributeSet.SetValue(EIsModifyable, 0, err);
	aAttributeSet.SetValue(EIsCopyable, 1, err);
	return err;
	}

TInt CTestAgentContent::GetStringAttribute(TInt /*aAttribute*/, TDes& /*aValue*/, const TDesC& /*aUniqueId*/)
	{
	return KErrNone;
	}

TInt CTestAgentContent::GetStringAttributeSet(RStringAttributeSet& /*aStringAttributeSet*/, const TDesC& /*aUniqueId*/)
	{
	return KErrNone;
	}

TInt CTestAgentContent::AgentSpecificCommand(TInt /*aCommand*/, const TDesC8& /*aInputBuffer*/, TDes8& /*aOutputBuffer*/)
	{
	return KErrNone;
	}

void CTestAgentContent::AgentSpecificCommand(TInt /*aCommand*/, const TDesC8& /*aInputBuffer*/, TDes8& /*aOutputBuffer*/, TRequestStatus& /*aStatus*/)
	{

	}

void CTestAgentContent::NotifyStatusChange(TEventMask /*aMask*/, TRequestStatus& /*aStatus*/, const TDesC& /*aUniqueId*/)
	{

	}

TInt CTestAgentContent::CancelNotifyStatusChange(TRequestStatus& /*aStatus*/, const TDesC& /*aUniqueId*/)
	{
	return KErrNone;
	}

void CTestAgentContent::RequestRights(TRequestStatus& /*aStatus*/, const TDesC& /*aUniqueId*/)
	{

	}

TInt CTestAgentContent::CancelRequestRights(TRequestStatus& /*aStatus*/, const TDesC& /*aUniqueId*/)
	{
	return KErrNone;
	}

void CTestAgentContent::DisplayInfoL(TDisplayInfo /*aInfo*/, const TDesC& /*aUniqueId*/)
	{

	}

TInt CTestAgentContent::SetProperty(TAgentProperty /*aProperty*/, TInt /*aValue*/)
	{
	return KErrNone;
	}



