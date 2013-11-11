// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "mmfaudioserverfactory.h"
#include <a3f/mmfaudiosvrservice.hrh>
#include <mm/mmpluginutils.h>
#include <mm/mmcleanup.h>
#include <badesca.h>

/**
 * CMMFAudioServerFactory::CMMFAudioServerFactory
 * C++ default constructor.
 */
CMMFAudioServerFactory::CMMFAudioServerFactory()
	{
	}

/**
 * CMMFAudioServerFactory::ConstructL
 * Symbian 2nd phase constructor.
 */
void CMMFAudioServerFactory::ConstructL()
	{
	TUid KAudioServiceInterfaceDefinitionUid = {KUidA3fAudioServicePlugin};
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);
	
	MmPluginUtils::FindImplementationsL(KAudioServiceInterfaceDefinitionUid, ecomArray);
	
	TInt count = ecomArray.Count();
	
	for (TInt i=0; i<count; i++)
		{
		TUid destructorKey;
		TUid implementationUid = ecomArray[i]->ImplementationUid();
		
		TAny* ptr = REComSession::CreateImplementationL( implementationUid,destructorKey,NULL);
		MAudioSvrService* audioservice = (static_cast <MAudioSvrService*>(ptr));
		audioservice->PassDestructorKey(destructorKey);
		CleanupReleasePushL(*audioservice);
		iAudioServList.AppendL(audioservice);
		CleanupStack::Pop(audioservice);
		User::LeaveIfError(audioservice->Load());
		}
		
	CleanupStack::PopAndDestroy(&ecomArray);
	}

// -----------------------------------------------------------------------------
// CMMFAudioServerFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMMFAudioServerFactory* CMMFAudioServerFactory::NewL()
	{
	CMMFAudioServerFactory* self = new (ELeave)CMMFAudioServerFactory;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Destructor
EXPORT_C CMMFAudioServerFactory::~CMMFAudioServerFactory()
	{
	TInt audioservicecount = iAudioServList.Count();
	for(TInt i= 0;i<audioservicecount;i++ )
		{
		iAudioServList[i]->Release();
		}
	iAudioServList.Close();
	}

/**
 * CMMFAudioServerFactory::StartL
 * Called by Audio Server when Audio Server is started
 * (other items were commented in a header).
 */
EXPORT_C void CMMFAudioServerFactory::StartL(
								const CMMFAudioServer& /*aAudioServer*/)
	{
	TInt audioservicecount = iAudioServList.Count();
	for (TInt i=0; i<audioservicecount; i++)
		{
		User::LeaveIfError(iAudioServList[i]->Start());
		}
	
	}

/**
 * CMMFAudioServerFactory::Stop
 * Called by Audio Server when Audio Server is shutting down
 */
EXPORT_C void CMMFAudioServerFactory::Stop(
								const CMMFAudioServer& /*aAudioServer*/)
	{
	TInt audioservicecount = iAudioServList.Count();
	for(TInt i= 0;i<audioservicecount;i++ )
		{
		iAudioServList[i]->Stop();
		}
	
	}

// End of File
