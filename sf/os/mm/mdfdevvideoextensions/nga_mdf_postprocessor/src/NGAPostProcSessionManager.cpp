/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#include <e32base.h>
#include <graphics/surfaceupdateclient.h> //RSurfaceUpdateSession
#include <graphics/surfacemanager.h> //RSurfaceManager
#include <graphics/suerror.h> //KAllScreens
 
#include "NGAPostProcSessionManager.h" 
#include "NGAPostProcSessionManagerObserver.h"
#include "MdfRDebug.h"
#include "NGAPostProcNotifier.h"

// --- Constants ---
const TInt KMessageSlots      = 4; 

//**************************************************

CNGAPostProcSessionManager* CNGAPostProcSessionManager::NewL() 
{ 
   PP_DEBUG(_L("CNGAPostProcSessionManager:NewL() ++"));

    CNGAPostProcSessionManager* self = new (ELeave) CNGAPostProcSessionManager; 
    CleanupStack::PushL(self);
    self->ConstructL(); 
    CleanupStack::Pop();

   PP_DEBUG(_L("CNGAPostProcSessionManager:NewL() --"));
    return self; 
}

CNGAPostProcSessionManager::CNGAPostProcSessionManager() 
:iSurfaceUpdateSession(NULL),
 iObserver(NULL)
 
{
}

void CNGAPostProcSessionManager::ConstructL() 
{ 
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:ConstructL() ++"), this);
    
    iSurfaceUpdateSession = new (ELeave) RSurfaceUpdateSession();
    User::LeaveIfError(iSurfaceUpdateSession->Connect(KMessageSlots));
    
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:ConstructL() --"), this);
}

CNGAPostProcSessionManager::~CNGAPostProcSessionManager()
{
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:~() ++"), this);
    if(iSurfaceUpdateSession)
    {
    	iSurfaceUpdateSession->Close();
    	delete iSurfaceUpdateSession;
    	iSurfaceUpdateSession = NULL;
	}
	while (iAOList.Count()>0)
    {
        CNGAPostProcNotifier* notifier = iAOList[0];
        iAOList.Remove(0);
        delete notifier;
    }
    
	iAOList.Reset();
    iAOList.Close();
    
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:~() --"), this);
}

void CNGAPostProcSessionManager::PostPicture(const TSurfaceId& aSurfaceId,
								TInt aBufId, TInt aNumOfBuf, TBool aNotifyRequired) 
{ 
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:PostPicture() ++"), this);
   TInt err = KErrNone;
	if(aNotifyRequired)
	{
	    CNGAPostProcNotifier* iNotifier;
	    if ( iAOList.Count() <= aNumOfBuf )
		{
			TRAP(err, iNotifier = CNGAPostProcNotifier::NewL(*this));
	    	if (err != KErrNone)
	    	{
	    	   PP_DEBUG(_L("CNGAPostProcHwDevice[%x]:Initialize -- failed to create CNGAPostProcNotifier %d."), this, err);
	    	   return;
	    	}
		}
		else
		{
		for(TInt j=0; j < iAOList.Count(); j++)
			{
				iNotifier = iAOList[j];
				if(!iNotifier->IsActive())
				{
					iAOList.Remove(j);
					iAOList.Compress();
					break;
				}
			}
		}
		iNotifier->SetBufferId(aBufId);
		iNotifier->Activate();
		iSurfaceUpdateSession->NotifyWhenAvailable(iNotifier->iStatus);
		iAOList.Append(iNotifier);
	}
	iSurfaceUpdateSession->SubmitUpdate(KAllScreens, aSurfaceId, aBufId);

   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:PostPicture() -- "), this);
}

void CNGAPostProcSessionManager::CancelUpdate() 
{ 
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:SetClockSource() ++"), this);
	iSurfaceUpdateSession->CancelAllUpdateNotifications();
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:SetClockSource() --"), this);
}

void CNGAPostProcSessionManager::HandleBufferRelease(TInt aBufId, TInt aStatus) 
{ 
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:HandleBufferRelease() ++"), this);
    if(iObserver)
    {
		iObserver->BufferAvailable(aBufId, aStatus);
	}
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:HandleBufferRelease() --"), this);
}

void CNGAPostProcSessionManager::SetObserver(MNGAPostProcSessionManagerObserver& aObserver) 
{ 
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:SetClockSource() ++"), this);
	iObserver = &aObserver;
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:SetClockSource() --"), this);
}

