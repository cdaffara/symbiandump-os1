// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "eirpublisherlocalname.h"
#include "linkutil.h"
#include <bluetooth/logger.h>

#include "eirmanserver.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_EIRMANAGER);
#endif


//**********************************
// CEirPublisherLocalName
//**********************************
/**
Provides functionality to publish Local Name to EIR.
**/
CEirPublisherLocalName* CEirPublisherLocalName::NewL(CEirManServer& aServer)
	{
	LOG_STATIC_FUNC
	CEirPublisherLocalName* self = new (ELeave) CEirPublisherLocalName();
	CleanupStack::PushL(self);
	self->ConstructL(aServer);
	CleanupStack::Pop();
	return self;
	}

CEirPublisherLocalName::CEirPublisherLocalName()
	{
	LOG_FUNC
	}

CEirPublisherLocalName::~CEirPublisherLocalName()
	{
	LOG_FUNC
	delete iPublishBuf;
	delete iSession;
	}
	
void CEirPublisherLocalName::ConstructL(CEirManServer& aServer)
	{
	LOG_FUNC
	iSession = aServer.NewInternalSessionL(*this);
	iSession->RegisterTag(EEirTagName);
	}

void CEirPublisherLocalName::MeisnRegisterComplete(TInt aResult)
	{
	if (aResult == KErrNone)
		{
		iTagRegistered = ETrue;
		if (iLocalName.Length() > 0)
			{
			iSession->NewData(iLocalName.Length());
			}
		}
	}

void CEirPublisherLocalName::MeisnSetDataError(TInt /* aError */)
	{
	
	}


void CEirPublisherLocalName::UpdateName(const TDesC8& aName)
	{
	LOG_FUNC
	// Check aName isn't longer than 248 characters
	__ASSERT_DEBUG(aName.Length() <= 248, Panic(EEIRPublisherUpdateNameTooLong));
	iLocalName = aName;
	if (iTagRegistered)
		{
		iSession->NewData(iLocalName.Size());
		}
	}
	

// From MEirPublisherNotifier
void CEirPublisherLocalName::MeisnSpaceAvailable(TUint aBytesAvailable)
	{
	LOG_FUNC
	// Delete memory from last time this was called
	delete iPublishBuf, iPublishBuf = NULL;
	
	TUint8 namelen = iLocalName.Length();
	TUint avail = aBytesAvailable;
	avail = avail < namelen ? avail : namelen;
		
	if (avail < namelen)
		{
		// Truncate the device name to space available
		iPublishBuf = iLocalName.Left(avail).Alloc();
		if(iPublishBuf)
			{
			iSession->SetData(*iPublishBuf, EEirDataPartial);
			}
		}
	else
		{
		// Zero-length device names will be published as "complete"
		// as defined in the specification, volume 3, part C, 8.1
		iPublishBuf = iLocalName.Alloc();
		if(iPublishBuf)
			{
			iSession->SetData(*iPublishBuf, EEirDataComplete);
			}
		}
	// Final case to handle if OOM occurs.
	if(!iPublishBuf)
		{
		iSession->SetData(KNullDesC8(), EEirDataPartial);
		}
	}


