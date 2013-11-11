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

void CNGAPostProcSessionManager::CreateNotifierL(TInt aNumSurfaceBuffers)
    {
    PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:CreateNotifierL() ++"), this);
    
    CNGAPostProcNotifier* notifier;
    for(TInt index = 0; index < aNumSurfaceBuffers; ++index)
        {
        notifier = CNGAPostProcNotifier::NewL(*this);
        CleanupStack::PushL(notifier);
        
        iAOList.AppendL(notifier); 
        
        CleanupStack::Pop();
        }   
    PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:CreateNotifierL() --"), this);
    }

CNGAPostProcSessionManager::~CNGAPostProcSessionManager()
{
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:~() ++"), this);
   //Its safer to destry notifiers before surfaceupdatesession object since notifiers
   //guarantee synchronous cancellation.
   iAOList.ResetAndDestroy();
   
    if(iSurfaceUpdateSession)
    {
    	CancelUpdate();
    	iSurfaceUpdateSession->Close();
    	delete iSurfaceUpdateSession;
    	iSurfaceUpdateSession = NULL;
	}
    
    
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:~() --"), this);
}

TInt CNGAPostProcSessionManager::PostPicture(const TSurfaceId& aSurfaceId,
								TInt aBufId, TBool aNotifyRequired) 
{ 
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:PostPicture() ++"), this);
   TInt err = KErrNone;
	if(aNotifyRequired)
	{
	    CNGAPostProcNotifier* notifier = NULL;
		for(TInt j=0; j < iAOList.Count(); j++)
		{
            //notifier = iAOList[j];
            if(!iAOList[j]->IsActive())
            {
                notifier = iAOList[j];
                iAOList.Remove(j);
                //iAOList.Compress(); //why this is required? may create fragmentation.
                break;
            }
		}
		if(!notifier)
		{
			
			return KErrInUse;
	        
		}
		
		notifier->SetBufferId(aBufId);
		notifier->Activate();
		iSurfaceUpdateSession->NotifyWhenAvailable(notifier->iStatus);
		iAOList.Append(notifier);
		
		
	}
	err = iSurfaceUpdateSession->SubmitUpdate(KAllScreens, aSurfaceId, aBufId);

   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:PostPicture() -- err = %d"), this, err);
   return err;
}

void CNGAPostProcSessionManager::CancelUpdate() 
{ 
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:CancelUpdate() ++"), this);
	iSurfaceUpdateSession->CancelAllUpdateNotifications();
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:CancelUpdate() --"), this);
}

void CNGAPostProcSessionManager::CancelNotifiers() 
{ 
    
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:CancelNotifiers() ++"), this);
   TInt notifierCount = iAOList.Count();
   for (TInt i = notifierCount - 1; i >= 0; --i) //reverse loop since the active notifier is appended to the list
   {
       if (iAOList[i]->IsActive())
       {
           iAOList[i]->Cancel();
       }
       else
       {
           break;
       }
   }
   

   
   PP_DEBUG(_L("CNGAPostProcSessionManager[%x]:CancelNotifiers() --"), this);

	
	
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

