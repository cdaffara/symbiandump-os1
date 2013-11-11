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

#include "eirpublishertxpowerlevel.h"
#include "eirmanserver.h"

//**********************************
// CEirPublisherTxPowerLevel
//**********************************
/**
Provides functionality to publish transmission power level to EIR.
**/
CEirPublisherTxPowerLevel* CEirPublisherTxPowerLevel::NewL(CEirManServer& aServer)
	{
	CEirPublisherTxPowerLevel* self = new (ELeave) CEirPublisherTxPowerLevel();
	CleanupStack::PushL(self);
	self->ConstructL(aServer);
	CleanupStack::Pop();
	return self;
	}

CEirPublisherTxPowerLevel::CEirPublisherTxPowerLevel()
	{
	}

void CEirPublisherTxPowerLevel::ConstructL(CEirManServer& aServer)
	{
	iSession = aServer.NewInternalSessionL(*this);
	iSession->RegisterTag(EEirTagTxPowerLevel);
	}

CEirPublisherTxPowerLevel::~CEirPublisherTxPowerLevel()
	{
	delete iSession;
	}

void CEirPublisherTxPowerLevel::MeisnRegisterComplete(TInt aResult)
	{
	if (aResult == KErrNone)
		{
		iTagRegistered = ETrue;
		if (iTxPowerLevelPublished)
			{
			iSession->NewData(KSizeOfTxPowerLevelData);
			}
		}
	}

void CEirPublisherTxPowerLevel::UpdateTxPowerLevel(TInt8 aTxPowerLevel)
	{
	iTxPowerLevel = aTxPowerLevel;
	iTxPowerLevelPublished = ETrue;
	if (iTagRegistered)
		{
		iSession->NewData(KSizeOfTxPowerLevelData);
		}
	}
	
// From MEirPublisherNotifier
void CEirPublisherTxPowerLevel::MeisnSpaceAvailable(TUint aBytesAvailable)
	{
	if (aBytesAvailable >= KSizeOfTxPowerLevelData)
		{
		// only publish TxPowerLevel if enough space
		iPublishBuf.Zero();
		iPublishBuf.Append(iTxPowerLevel);
		iSession->SetData(iPublishBuf, EEirDataComplete);
		}
	else
		{
		// Otherwise publish zero length string
		iPublishBuf.Zero();
		iSession->SetData(iPublishBuf, EEirDataPartial);
		}
	}

void CEirPublisherTxPowerLevel::MeisnSetDataError(TInt /* aError */)
	{
	
	}
