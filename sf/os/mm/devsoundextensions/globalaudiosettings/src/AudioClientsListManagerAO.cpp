/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This file contains implementation of Audio Clients List Manager.
*
*/




#include "AudioClientsListManagerAO.h"
#include <AudioClientsListPSKeys.h>
#include <AudioClientsListPSData.h>
#include <MAudioClientsListObserver.h>

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::CAudioClientsListManagerAO
//
// -----------------------------------------------------------------------------
//
CAudioClientsListManagerAO::CAudioClientsListManagerAO(
                                CGlobalAudioSettings& aGlobalAudioSettings,
                                RPointerArray<MAudioClientsListObserver>& aArray, 
                                TUint32 aKey )
:CActive(CActive::EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioClientsListObserverArray(aArray),
 iKey(aKey)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::~CAudioClientsListManagerAO
//
// -----------------------------------------------------------------------------
//
CAudioClientsListManagerAO::~CAudioClientsListManagerAO()
    { 
    Cancel();
    iActiveAudioClients.Close();
    }

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::NewL
//
// -----------------------------------------------------------------------------
//
CAudioClientsListManagerAO* CAudioClientsListManagerAO::NewL(
                                                CGlobalAudioSettings& aGlobalAudioSettings,
                                                RPointerArray<MAudioClientsListObserver>& aArray,
                                                TUint32 aKey )
    {
    CAudioClientsListManagerAO* self = new (ELeave) CAudioClientsListManagerAO(aGlobalAudioSettings,aArray,aKey);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::ConstructL
//
// -----------------------------------------------------------------------------
//
void CAudioClientsListManagerAO::ConstructL()
    {
    User::LeaveIfError( iActiveAudioClients.Attach(
                                KPSUidMMFAudioServer,
                                iKey,
                                EOwnerThread ) );
    }

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::MonitorStart
//
// -----------------------------------------------------------------------------
//
TInt CAudioClientsListManagerAO::MonitorStart()
    {
    TInt status(KErrAlreadyExists);
    if ( !IsActive() )
        {
        SetActive();	
        iActiveAudioClients.Subscribe(iStatus);
        status = KErrNone;
        }
  
    return status;
    }

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::MonitorStop
//
// -----------------------------------------------------------------------------
//
TInt CAudioClientsListManagerAO::MonitorStop()
    {
    TInt status(KErrNone);
    if ( IsActive() )
        {
        Cancel();
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::GetAudioClientsList
//
// -----------------------------------------------------------------------------
//
TInt CAudioClientsListManagerAO::GetAudioClientsList(
                        CGlobalAudioSettings::TAudioClientListType aType,
                        RArray<TProcessId>& aList )
    {
    TInt status(KErrNone);
    aList.Reset();
    switch ( aType )
        {
        case CGlobalAudioSettings::EActiveAudioClients:
            {
            TAudioClientList audioClientListPckg;
            status = iActiveAudioClients.Get( KPSUidMMFAudioServer,
                                              KAudioPolicyAudioClients,
                                              audioClientListPckg );
            if ( status == KErrNone )
                {
                for ( TInt index = 0; index < audioClientListPckg().iNumOfProcesses; index++ )
                    {
                    status = aList.Append((audioClientListPckg().iProcessList)[index]);
                    if ( status != KErrNone )
                        {
                        aList.Reset();
                        break;
                        }
                    }
                }
            }
            break;
            
         case CGlobalAudioSettings::EPausedAudioClients:
            {
            TAudioPolicyProcessIdList audioClientListPckg;
            status = iActiveAudioClients.Get(KPSUidMMFAudioServer,
                                             KAudioPolicyApplicationAudioStatePaused,
                                             audioClientListPckg );
            if ( status == KErrNone )
                {
                for ( TInt index = 0; index < audioClientListPckg().iNumOfProcesses; index++ )
                    {
                    status = aList.Append((audioClientListPckg().iProcessList)[index]);
                    if ( status != KErrNone )
                        {
                        aList.Reset();
                        break;
                        }
                    }
                }
            }
            break;
              
        default:
            status = KErrNotFound;
            break;
        };
    return status;
    }

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::RunL
//
// -----------------------------------------------------------------------------
//
void CAudioClientsListManagerAO::RunL()
    {
    TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CAudioClientsListManagerAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone ) 
        {
        MonitorStart();
        // Go through the array and notify every observer
      	switch(iKey)
      		{ 
      		case KAudioPolicyAudioClients:
      			{      		
        		for (TInt index = 0; index < iAudioClientsListObserverArray.Count(); index++)
            		{            	                		
            		iAudioClientsListObserverArray[index]->Event( iGlobalAudioSettings,
                                    MAudioClientsListObserver::KActiveAudioClientsListChanged);        
            		}
      			}
      			break;
      	 	case KAudioPolicyApplicationAudioStatePaused:
      			{
      			for (TInt index = 0; index < iAudioClientsListObserverArray.Count(); index++)
            		{            		       		
            		iAudioClientsListObserverArray[index]->Event( iGlobalAudioSettings,
                                    MAudioClientsListObserver::KPausedAudioClientsListChanged);
            		}
      			}
      			break;
      		default:
      	  		break;	
        
        	} //end swith(iKey)
        }
    }

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::DoCancel
//
// -----------------------------------------------------------------------------
//
void CAudioClientsListManagerAO::DoCancel()
    {
    iActiveAudioClients.Cancel();
    }

// -----------------------------------------------------------------------------
// CAudioClientsListManagerAO::RunError
//
// -----------------------------------------------------------------------------
//
TInt CAudioClientsListManagerAO::RunError(TInt /*aError*/)
    {
    // Observer callback has leaving code!!!. Ignore it.
    return KErrNone;
    }

// End of file
