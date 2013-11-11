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
* Description:   Implementation of the Phone Profile Settings API
*
*/




// INCLUDE FILES
#include "GlobalAudioSettingsImpl.h"
#include <e32std.h>
#include <GlobalAudioSettingsPSKeys.h>
#include <MGlobalAudioSettingsObserver.h>
#include <centralrepository.h>
#include "AudioClientsListManagerAO.h"
#include <AudioClientsListPSKeys.h>

#ifdef _DEBUG
#define PRINT_MESSAGE
#endif // _DEBUG

const TInt KMaxLength=256;
_LIT( KNoSound, "z:\\resource\\No_Sound.wav" );
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::NewL();
// 
// -----------------------------------------------------------------------------
//
CGlobalAudioSettingsImpl::CGlobalAudioSettingsImpl(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver)
:iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver)
	{
	}
	
// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::NewL();
// 
// -----------------------------------------------------------------------------
//
CGlobalAudioSettingsImpl* CGlobalAudioSettingsImpl::NewL(
                                        CGlobalAudioSettings &aGlobalAudioSettings,
                                        MAudioSettingsObserver& aAudioSettingsObserver)
	{
	CGlobalAudioSettingsImpl* self = new (ELeave) CGlobalAudioSettingsImpl(
	                                    aGlobalAudioSettings,aAudioSettingsObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}
	
// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::ConstructL();
// 
// -----------------------------------------------------------------------------
//
void CGlobalAudioSettingsImpl::ConstructL()
	{
	iWarningTonesObserverAO = CWarningTonesObserverAO::NewL(
                	                                        iGlobalAudioSettings,
                	                                        iAudioSettingsObserver,
                	                                        iGlobalAudioSettingsData);
	iWarningTonesObserverAO->Subscribe();
	
	iMessagingTonesObserverAO = CMessagingTonesObserverAO::NewL(
                                        	                iGlobalAudioSettings,
                                        	                iAudioSettingsObserver,
                                        	                iGlobalAudioSettingsData);
	iMessagingTonesObserverAO->Subscribe();

	iSilentProfileObserverAO = CSilentProfileObserverAO::NewL(
                                        	                iGlobalAudioSettings,
                                        	                iAudioSettingsObserver,
                                        	                iGlobalAudioSettingsData);
	iSilentProfileObserverAO->Subscribe();

	iVibraObserverAO = CVibraObserverAO::NewL(
                        	                iGlobalAudioSettings,
                        	                iAudioSettingsObserver,
                        	                iGlobalAudioSettingsData);
	iVibraObserverAO->Subscribe();
	iRingingTypeObserverAO = CRingingTypeObserverAO::NewL(
	                                            iGlobalAudioSettings,
	                                            iAudioSettingsObserver,
	                                            iGlobalAudioSettingsData);
    iRingingTypeObserverAO->Subscribe();
     
	iMessageToneObserverAO = CMessageToneObserverAO::NewL(
	                                            iGlobalAudioSettings,
	                                            iAudioSettingsObserver,
	                                            iGlobalAudioSettingsData);
	iMessageToneObserverAO->Subscribe();
	iEmailToneObserverAO = CEmailToneObserverAO::NewL(
	                                                iGlobalAudioSettings,
	                                                iAudioSettingsObserver,
	                                                iGlobalAudioSettingsData);
	iEmailToneObserverAO->Subscribe();   
	
	
    iVideoCallObserverAO = CVideoCallToneObserverAO::NewL(
                                                            iGlobalAudioSettings,
                                                            iAudioSettingsObserver,
                                                            iGlobalAudioSettingsData);
    iVideoCallObserverAO->Subscribe();  
        
    iRingingTone1ObserverAO = CRingingTone1ObserverAO::NewL(
                                                                    iGlobalAudioSettings,
                                                                    iAudioSettingsObserver,
                                                                    iGlobalAudioSettingsData);
    iRingingTone1ObserverAO->Subscribe();
	
    iRingingTone2ObserverAO = CRingingTone2ObserverAO::NewL(
                                                                            iGlobalAudioSettings,
                                                                            iAudioSettingsObserver,
                                                                            iGlobalAudioSettingsData);
    iRingingTone2ObserverAO->Subscribe();  
	
    iKeypadToneObserverAO = CKeypadToneObserverAO::NewL(
                                                                            iGlobalAudioSettings,
                                                                            iAudioSettingsObserver,
                                                                            iGlobalAudioSettingsData);
    iKeypadToneObserverAO->Subscribe();  
                
    iSilenceModeObserverAO = CSilenceModeObserverAO::NewL(
                                                                                iGlobalAudioSettings,
                                                                                iAudioSettingsObserver,
                                                                                iGlobalAudioSettingsData);
     iSilenceModeObserverAO->Subscribe();  
	                    
    RProperty publicSilenceProperty;
	User::LeaveIfError(publicSilenceProperty.Attach(KGASPSUidGlobalAudioSettings, KGASPublicSilence));
	User::LeaveIfError(publicSilenceProperty.Get(iGlobalAudioSettingsData.iPublicSilence));
	publicSilenceProperty.Close();

    iCAudioClientsListManagerAO = CAudioClientsListManagerAO::NewL( iGlobalAudioSettings,iAudioClientsListObserverArray,KAudioPolicyAudioClients);
            
    iPausedClientsListManagerAO = CAudioClientsListManagerAO::NewL( iGlobalAudioSettings,iAudioClientsListObserverArray,KAudioPolicyApplicationAudioStatePaused );
    }
    
// Destructor
CGlobalAudioSettingsImpl::~CGlobalAudioSettingsImpl()
    {
        delete iWarningTonesObserverAO;
        delete iMessagingTonesObserverAO;
        delete iSilentProfileObserverAO;
        delete iVibraObserverAO;
        delete iRingingTypeObserverAO;
        delete iMessageToneObserverAO;
        delete iEmailToneObserverAO;
        delete iVideoCallObserverAO;
        delete iRingingTone1ObserverAO;
        delete iRingingTone2ObserverAO;
        delete iKeypadToneObserverAO;
        delete iSilenceModeObserverAO;
        delete iCAudioClientsListManagerAO;
        delete iPausedClientsListManagerAO;
        iAudioClientsListObserverArray.Close();
    }

// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
 TBool CGlobalAudioSettingsImpl::IsWarningTonesEnabled()
	{
	if(iGlobalAudioSettingsData.iWarningTones)
	    return(ETrue);
	else
	    return(EFalse);
	}

// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::IsMessageTonesEnabled
// 
// -----------------------------------------------------------------------------
//
TBool CGlobalAudioSettingsImpl::IsMessageTonesEnabled()
	{
	if(iGlobalAudioSettingsData.iMessageTones)
	    return(ETrue);
	else
	    return(EFalse);
	}

// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::IsSilentProfileEnabled
// 
// -----------------------------------------------------------------------------
//
TBool CGlobalAudioSettingsImpl::IsSilentProfileEnabled()
	{
	if(iGlobalAudioSettingsData.iSilentProfile)
	    return(ETrue);
	else
	    return(EFalse);
	}

// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::IsVibraEnabled
// 
// -----------------------------------------------------------------------------
//
 TBool CGlobalAudioSettingsImpl::IsVibraEnabled()
	{
	if(iGlobalAudioSettingsData.iVibra)
	    return(ETrue);
	else
	    return(EFalse);
	}

// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::IsPublicSilenceEnabled
// 
// -----------------------------------------------------------------------------
//
TBool CGlobalAudioSettingsImpl::IsPublicSilenceEnabled()
	{
	if(iGlobalAudioSettingsData.iPublicSilence)
	    return(ETrue);
	else
	    return(EFalse);
	}




TDesC& CGlobalAudioSettingsImpl::MessageAlertTone()
    {
     return *iGlobalAudioSettingsData.iMessageToneName;
    
    }

TBool CGlobalAudioSettingsImpl::IsMessageAlertToneEnabled()
    {
     if(iGlobalAudioSettingsData.iMessageToneName->CompareF(KNoSound)==0)
         return EFalse;
     else
         return ETrue;
    
    }
 TDesC& CGlobalAudioSettingsImpl::EmailAlertTone()
    {
    return *iGlobalAudioSettingsData.iEmailToneName;
        
    }
 TBool CGlobalAudioSettingsImpl::IsEmailAlertToneEnabled()
     {
      if(iGlobalAudioSettingsData.iEmailToneName->CompareF(KNoSound)==0)
          return EFalse;
      else
          return ETrue;
     
     }
  CGlobalAudioSettings::TGASRingingType CGlobalAudioSettingsImpl::RingingType()
    {
    CGlobalAudioSettings::TGASRingingType type;
   type= ( CGlobalAudioSettings::TGASRingingType)iGlobalAudioSettingsData.iRingingType;
 
      return type;
    }
  
  TDesC& CGlobalAudioSettingsImpl::VideoCallAlertTone()
      {
      return *iGlobalAudioSettingsData.iVideoCallToneName;
      }
  TBool CGlobalAudioSettingsImpl::IsVideoCallAlertToneEnabled()
      {
       if(iGlobalAudioSettingsData.iVideoCallToneName->CompareF(KNoSound)==0)
           return EFalse;
       else
           return ETrue;
      
      }
  TDesC& CGlobalAudioSettingsImpl::RingingAlertTone1()
     {
     return *iGlobalAudioSettingsData.iRingingTone1Name;
     }
  TBool CGlobalAudioSettingsImpl::IsRingingAlertTone1Enabled()
      {
       if(iGlobalAudioSettingsData.iRingingTone1Name->CompareF(KNoSound)==0)
           return EFalse;
       else
           return ETrue;
      
      }
  TDesC& CGlobalAudioSettingsImpl::RingingAlertTone2()
     {
     return *iGlobalAudioSettingsData.iRingingTone2Name;
     }
  
  TBool CGlobalAudioSettingsImpl::IsRingingAlertTone2Enabled()
      {
       if(iGlobalAudioSettingsData.iRingingTone2Name->CompareF(KNoSound)==0)
           return EFalse;
       else
           return ETrue;
      
      }
 CGlobalAudioSettings::TGASKeypadVolume CGlobalAudioSettingsImpl::KeyPadToneVolume()
     {
     return (CGlobalAudioSettings::TGASKeypadVolume)iGlobalAudioSettingsData.iKeyPadVolume;
     }
 
 TBool CGlobalAudioSettingsImpl::IsSilenceModeEnabled()
     {
      return iGlobalAudioSettingsData.iSilenceMode;
     }
// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::RegisterAudioClientsListObserver
// 
// -----------------------------------------------------------------------------
//
TInt CGlobalAudioSettingsImpl::RegisterAudioClientsListObserver(
                            MAudioClientsListObserver& aObserver )
	{
	TInt status(KErrAlreadyExists);
	if ( iAudioClientsListObserverArray.Find( &aObserver ) == KErrNotFound )
	    {
	    status = iAudioClientsListObserverArray.Append( &aObserver);
	    if ( ( status == KErrNone ) && (iAudioClientsListObserverArray.Count() == 1 ) )
	        {  	       
	 	    iCAudioClientsListManagerAO->MonitorStart();
	        iPausedClientsListManagerAO->MonitorStart();
	        }
	    }
	return status;
	}
        
// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::UnregisterAudioClientsListObserver
// 
// -----------------------------------------------------------------------------
//
TInt CGlobalAudioSettingsImpl::UnregisterAudioClientsListObserver(
                            MAudioClientsListObserver& aObserver)
	{
	TInt status(KErrNone);
	status = iAudioClientsListObserverArray.Find( &aObserver );
	if ( status != KErrNotFound )
	    {
	    iAudioClientsListObserverArray.Remove( status );
	    if ( iAudioClientsListObserverArray.Count() == 0 )
	        {
	        iCAudioClientsListManagerAO->MonitorStop();
	        iPausedClientsListManagerAO->MonitorStop();	
	        }
	    }
	return status;
	}

// -----------------------------------------------------------------------------
// CGlobalAudioSettingsImpl::GetAudioClientsList
// 
// -----------------------------------------------------------------------------
//
TInt CGlobalAudioSettingsImpl::GetAudioClientsList(
                        CGlobalAudioSettings::TAudioClientListType aType,
                        RArray<TProcessId>& aList )
	{
	TInt status(KErrNone);
	
	switch(aType)
		{
		case CGlobalAudioSettings::EActiveAudioClients:
			{
			status = iCAudioClientsListManagerAO->GetAudioClientsList( aType, aList );	
			}
			break;
        case CGlobalAudioSettings::EPausedAudioClients:
        	{
        	status = iPausedClientsListManagerAO->GetAudioClientsList( aType, aList );     	
        	}
        	break;
		default:		
			break;
			
		}
	 return status;
	

	}

//////////////////////////////////////////////////////////////////////
//	CWarningTonesObserverAO											//
//////////////////////////////////////////////////////////////////////
CWarningTonesObserverAO::CWarningTonesObserverAO(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
	{
	CActiveScheduler::Add(this);
	}

CWarningTonesObserverAO::~CWarningTonesObserverAO()
	{
	Cancel();
	iWarningTonesProperty.Close();
	}

CWarningTonesObserverAO* CWarningTonesObserverAO::NewL(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
	{
	CWarningTonesObserverAO* self = new (ELeave) CWarningTonesObserverAO(
	                                                    aGlobalAudioSettings, 
	                                                    aAudioSettingsObserver,
	                                                    aGlobalAudioSettingsData);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CWarningTonesObserverAO::ConstructL()
	{
	User::LeaveIfError(iWarningTonesProperty.Attach(KGASPSUidGlobalAudioSettings, KGASWarningTones));
    User::LeaveIfError(iWarningTonesProperty.Get(iGlobalAudioSettingsData.iWarningTones));
	}
	
void CWarningTonesObserverAO::Subscribe()
	{
	if (!IsActive())
	    {
    	SetActive();
    	iWarningTonesProperty.Subscribe(iStatus);
	    }
	}

void CWarningTonesObserverAO::RunL()
	{
	TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CWarningTonesObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        status = iWarningTonesProperty.Get(iGlobalAudioSettingsData.iWarningTones);
        if(status == KErrNone)
            {
        MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASWarningTones;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CWarningTonesObserverAO::RunL:Property.Get Error[%d]"), status);            
            }
#endif // PRINT_MESSAGE
        }
	}

void CWarningTonesObserverAO::DoCancel()
	{
	iWarningTonesProperty.Cancel();
	}

TInt CWarningTonesObserverAO::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

//////////////////////////////////////////////////////////////////////
//	CMessagingTonesObserverAO											//
//////////////////////////////////////////////////////////////////////
CMessagingTonesObserverAO::CMessagingTonesObserverAO(
                                CGlobalAudioSettings &aGlobalAudioSettings,
                                MAudioSettingsObserver& aAudioSettingsObserver,
                                TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
	{
	CActiveScheduler::Add(this);
	}

CMessagingTonesObserverAO::~CMessagingTonesObserverAO()
	{
	Cancel();
	iMessagingTonesProperty.Close();
	}

CMessagingTonesObserverAO* CMessagingTonesObserverAO::NewL(
                                               CGlobalAudioSettings &aGlobalAudioSettings,
                                               MAudioSettingsObserver& aAudioSettingsObserver,
                                               TGlobalAudioSettings& aGlobalAudioSettingsData)
	{
	CMessagingTonesObserverAO* self = new (ELeave) CMessagingTonesObserverAO(
                    	                        aGlobalAudioSettings, aAudioSettingsObserver, 
                    	                        aGlobalAudioSettingsData);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CMessagingTonesObserverAO::ConstructL()
	{
	User::LeaveIfError(iMessagingTonesProperty.Attach(KGASPSUidGlobalAudioSettings, KGASMessageTones));
	User::LeaveIfError(iMessagingTonesProperty.Get(iGlobalAudioSettingsData.iMessageTones));
	}
	
void CMessagingTonesObserverAO::Subscribe()
	{
	if (!IsActive())
	    {
    	SetActive();
    	iMessagingTonesProperty.Subscribe(iStatus);
	    }
	}

void CMessagingTonesObserverAO::RunL()
	{
	TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CMessagingTonesObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        status = iMessagingTonesProperty.Get(iGlobalAudioSettingsData.iMessageTones);
        if(status == KErrNone)
            {
        MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASMessageTones;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CMessagingTonesObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
	}

void CMessagingTonesObserverAO::DoCancel()
	{
	iMessagingTonesProperty.Cancel();
	}

TInt CMessagingTonesObserverAO::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}
	
//////////////////////////////////////////////////////////////////////
//	CSilentProfileObserverAO											//
//////////////////////////////////////////////////////////////////////
CSilentProfileObserverAO::CSilentProfileObserverAO(
                                CGlobalAudioSettings &aGlobalAudioSettings,
                                MAudioSettingsObserver& aAudioSettingsObserver,
                                TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
	{
	CActiveScheduler::Add(this);
	}

CSilentProfileObserverAO::~CSilentProfileObserverAO()
	{
	Cancel();
	iSilentProfileProperty.Close();
	}

CSilentProfileObserverAO* CSilentProfileObserverAO::NewL(
                                        CGlobalAudioSettings &aGlobalAudioSettings,
                                        MAudioSettingsObserver& aAudioSettingsObserver,
                                        TGlobalAudioSettings& aGlobalAudioSettingsData)
	{
	CSilentProfileObserverAO* self = new (ELeave) CSilentProfileObserverAO(
                    	                        aGlobalAudioSettings, aAudioSettingsObserver, 
                    	                        aGlobalAudioSettingsData);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSilentProfileObserverAO::ConstructL()
	{
	User::LeaveIfError(iSilentProfileProperty.Attach(KGASPSUidGlobalAudioSettings, KGASSilentProfile));
	User::LeaveIfError(iSilentProfileProperty.Get(iGlobalAudioSettingsData.iSilentProfile));
	}
	
void CSilentProfileObserverAO::Subscribe()
	{
	if (!IsActive())
	    {
    	SetActive();
    	iSilentProfileProperty.Subscribe(iStatus);
	    }
	}

void CSilentProfileObserverAO::RunL()
	{
	TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CSilentProfileObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        status = iSilentProfileProperty.Get(iGlobalAudioSettingsData.iSilentProfile);
        if(status == KErrNone)
            {
        MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASSilentProfile;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CSilentProfileObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
	}

void CSilentProfileObserverAO::DoCancel()
	{
	iSilentProfileProperty.Cancel();
	}

TInt CSilentProfileObserverAO::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}
	
//////////////////////////////////////////////////////////////////////
//	CVibraObserverAO											    //
//////////////////////////////////////////////////////////////////////
CVibraObserverAO::CVibraObserverAO(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
	{
	CActiveScheduler::Add(this);
	}

CVibraObserverAO::~CVibraObserverAO()
	{
	Cancel();
	iVibraProperty.Close();
	}

CVibraObserverAO* CVibraObserverAO::NewL(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
	{
	CVibraObserverAO* self = new (ELeave) CVibraObserverAO(
                	                        aGlobalAudioSettings,
                	                        aAudioSettingsObserver, 
                	                        aGlobalAudioSettingsData);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CVibraObserverAO::ConstructL()
	{
	User::LeaveIfError(iVibraProperty.Attach(KGASPSUidGlobalAudioSettings, KGASVibra));
	User::LeaveIfError(iVibraProperty.Get(iGlobalAudioSettingsData.iVibra));
	}
	
void CVibraObserverAO::Subscribe()
	{
	if (!IsActive())
	    {
    	SetActive();
    	iVibraProperty.Subscribe(iStatus);
	    }
	}

void CVibraObserverAO::RunL()
	{
	TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CVibraObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        status = iVibraProperty.Get(iGlobalAudioSettingsData.iVibra);
        if( status == KErrNone)
            {
        MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASVibra;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings,id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CVibraObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
	}

void CVibraObserverAO::DoCancel()
	{
	iVibraProperty.Cancel();
	}

TInt CVibraObserverAO::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

//////////////////////////////////////////////////////////////////////
//  CRingingTypeObserverAO                                                //
//////////////////////////////////////////////////////////////////////
CRingingTypeObserverAO::CRingingTypeObserverAO(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
    {
    CActiveScheduler::Add(this);
    }

CRingingTypeObserverAO::~CRingingTypeObserverAO()
    {
    Cancel();
    iRingingTypeProperty.Close();
    }

CRingingTypeObserverAO* CRingingTypeObserverAO::NewL(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
    {
    CRingingTypeObserverAO* self = new (ELeave) CRingingTypeObserverAO(
                                            aGlobalAudioSettings,
                                            aAudioSettingsObserver, 
                                            aGlobalAudioSettingsData);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CRingingTypeObserverAO::ConstructL()
    {
    User::LeaveIfError(iRingingTypeProperty.Attach(KGASPSUidGlobalAudioSettings, KGASRingingType));
    TInt value=KErrNone;
    User::LeaveIfError(iRingingTypeProperty.Get(value));
    iGlobalAudioSettingsData.iRingingType=(CGlobalAudioSettings::TGASRingingType)value;
    
    }
    
void CRingingTypeObserverAO::Subscribe()
    {
    if (!IsActive())
        {
        SetActive();
        
        iRingingTypeProperty.Subscribe(iStatus);
        }
    }

void CRingingTypeObserverAO::RunL()
    {
    TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CRingingTypeObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        TInt value=KErrNone;
        status=(iRingingTypeProperty.Get(value));
        iGlobalAudioSettingsData.iRingingType=(CGlobalAudioSettings::TGASRingingType)value;
        if( status == KErrNone)
            {
        MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASRingingType;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CRingingTypeObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
    }

void CRingingTypeObserverAO::DoCancel()
    {
    iRingingTypeProperty.Cancel();
    }

TInt CRingingTypeObserverAO::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

//////////////////////////////////////////////////////////////////////
//  CMessageToneObserverAO                                                //
//////////////////////////////////////////////////////////////////////
CMessageToneObserverAO::CMessageToneObserverAO(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
    {
    CActiveScheduler::Add(this);
    }

CMessageToneObserverAO::~CMessageToneObserverAO()
    {
    Cancel();
    iMessageToneNameProperty.Close();
    if( iGlobalAudioSettingsData.iMessageToneName)
        {
          delete  iGlobalAudioSettingsData.iMessageToneName;
          iGlobalAudioSettingsData.iMessageToneName=NULL;
        }
    }

CMessageToneObserverAO* CMessageToneObserverAO::NewL(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
    {
    CMessageToneObserverAO* self = new (ELeave) CMessageToneObserverAO(
                                            aGlobalAudioSettings,
                                            aAudioSettingsObserver, 
                                            aGlobalAudioSettingsData);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CMessageToneObserverAO::ConstructL()
    {
    iGlobalAudioSettingsData.iMessageToneName=HBufC::NewL(KMaxLength);
    User::LeaveIfError(iMessageToneNameProperty.Attach(KGASPSUidGlobalAudioSettings, KGASMessageToneName));
    TPtr16 ptr=iGlobalAudioSettingsData.iMessageToneName->Des();
    User::LeaveIfError(iMessageToneNameProperty.Get(ptr));
    
    }
    
void CMessageToneObserverAO::Subscribe()
    {
    if (!IsActive())
        {
        SetActive();
        
        iMessageToneNameProperty.Subscribe(iStatus);
        }
    }

void CMessageToneObserverAO::RunL()
    {
    TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CMessageToneObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        delete iGlobalAudioSettingsData.iMessageToneName;
        iGlobalAudioSettingsData.iMessageToneName=NULL;
        iGlobalAudioSettingsData.iMessageToneName=HBufC::NewL(KMaxLength);
        TPtr16 ptr=iGlobalAudioSettingsData.iMessageToneName->Des();
        status = iMessageToneNameProperty.Get(ptr);
        if( status == KErrNone)
            {
            MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASMessageToneName;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CMessageToneObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
    }

void CMessageToneObserverAO::DoCancel()
    {
    iMessageToneNameProperty.Cancel();
    }

TInt CMessageToneObserverAO::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }


/////////////////////////////////////////////////////////////////////
//  CEmailToneObserverAO                                                //
//////////////////////////////////////////////////////////////////////
CEmailToneObserverAO::CEmailToneObserverAO(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
    {
    CActiveScheduler::Add(this);
    }

CEmailToneObserverAO::~CEmailToneObserverAO()
    {
    Cancel();
    iEmailToneNameProperty.Close();
    
    if(iGlobalAudioSettingsData.iEmailToneName)
        {
        delete  iGlobalAudioSettingsData.iEmailToneName;
        iGlobalAudioSettingsData.iEmailToneName=NULL;
        }
   
    }

CEmailToneObserverAO* CEmailToneObserverAO::NewL(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
    {
    CEmailToneObserverAO* self = new (ELeave) CEmailToneObserverAO(
                                            aGlobalAudioSettings,
                                            aAudioSettingsObserver, 
                                            aGlobalAudioSettingsData);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CEmailToneObserverAO::ConstructL()
    {
    
    iGlobalAudioSettingsData.iEmailToneName=HBufC::NewL(KMaxLength);
    User::LeaveIfError(iEmailToneNameProperty.Attach(KGASPSUidGlobalAudioSettings, KGASEmailToneName));
    TPtr16 ptr=iGlobalAudioSettingsData.iEmailToneName->Des();
    User::LeaveIfError(iEmailToneNameProperty.Get(ptr));
    
    }
    
void CEmailToneObserverAO::Subscribe()
    {
    if (!IsActive())
        {
        SetActive();
        
        iEmailToneNameProperty.Subscribe(iStatus);
        }
    }

void CEmailToneObserverAO::RunL()
    {
    TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CEmailToneObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        delete iGlobalAudioSettingsData.iEmailToneName;
        iGlobalAudioSettingsData.iEmailToneName=NULL;
        iGlobalAudioSettingsData.iEmailToneName=HBufC::NewL(KMaxLength);
        TPtr16 ptr=iGlobalAudioSettingsData.iEmailToneName->Des();
        status = iEmailToneNameProperty.Get(ptr);
        if( status == KErrNone)
            {
            MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASEmailToneName;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CEmailToneObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
    }

void CEmailToneObserverAO::DoCancel()
    {
    iEmailToneNameProperty.Cancel();
    }

TInt CEmailToneObserverAO::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }


/////////////////////////////////////////////////////////////////////
//  CVideoCallToneObserverAO                                                //
//////////////////////////////////////////////////////////////////////
CVideoCallToneObserverAO::CVideoCallToneObserverAO(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
    {
    CActiveScheduler::Add(this);
    }

CVideoCallToneObserverAO::~CVideoCallToneObserverAO()
    {
    Cancel();
    iVideoCallToneNameProperty.Close();
    if( iGlobalAudioSettingsData.iVideoCallToneName)
        {
    delete  iGlobalAudioSettingsData.iVideoCallToneName;
    iGlobalAudioSettingsData.iVideoCallToneName=NULL;
        }
     
    
    }

CVideoCallToneObserverAO* CVideoCallToneObserverAO::NewL(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
    {
    CVideoCallToneObserverAO* self = new (ELeave) CVideoCallToneObserverAO(
                                            aGlobalAudioSettings,
                                            aAudioSettingsObserver, 
                                            aGlobalAudioSettingsData);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CVideoCallToneObserverAO::ConstructL()
    {
    
    iGlobalAudioSettingsData.iVideoCallToneName=HBufC::NewL(KMaxLength);
    User::LeaveIfError(iVideoCallToneNameProperty.Attach(KGASPSUidGlobalAudioSettings, KGASVideoCallToneName));
    TPtr16 ptr=iGlobalAudioSettingsData.iVideoCallToneName->Des();
    User::LeaveIfError(iVideoCallToneNameProperty.Get(ptr));
    
    }
    
void CVideoCallToneObserverAO::Subscribe()
    {
    if (!IsActive())
        {
        SetActive();
        
        iVideoCallToneNameProperty.Subscribe(iStatus);
        }
    }

void CVideoCallToneObserverAO::RunL()
    {
    TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CVideocallToneObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        delete iGlobalAudioSettingsData.iVideoCallToneName;
        iGlobalAudioSettingsData.iVideoCallToneName=NULL;
        iGlobalAudioSettingsData.iVideoCallToneName=HBufC::NewL(KMaxLength);
        TPtr16 ptr=iGlobalAudioSettingsData.iVideoCallToneName->Des();
        status = iVideoCallToneNameProperty.Get(ptr);
        if( status == KErrNone)
            {
        MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASVideoCallToneName;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CVideoCallToneObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
    }

void CVideoCallToneObserverAO::DoCancel()
    {
    iVideoCallToneNameProperty.Cancel();
    }

TInt CVideoCallToneObserverAO::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

/////////////////////////////////////////////////////////////////////
//  CRingingToneObserverAO                                                //
//////////////////////////////////////////////////////////////////////
CRingingTone1ObserverAO::CRingingTone1ObserverAO(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
    {
    CActiveScheduler::Add(this);
    }

CRingingTone1ObserverAO::~CRingingTone1ObserverAO()
    {
    Cancel();
    iRingingTone1NameProperty.Close();
    if( iGlobalAudioSettingsData.iRingingTone1Name)
        {
        delete  iGlobalAudioSettingsData.iRingingTone1Name;
        iGlobalAudioSettingsData.iRingingTone1Name=NULL;
        }
    }

CRingingTone1ObserverAO* CRingingTone1ObserverAO::NewL(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
    {
    CRingingTone1ObserverAO* self = new (ELeave) CRingingTone1ObserverAO(
                                            aGlobalAudioSettings,
                                            aAudioSettingsObserver, 
                                            aGlobalAudioSettingsData);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CRingingTone1ObserverAO::ConstructL()
    {
    
    iGlobalAudioSettingsData.iRingingTone1Name=HBufC::NewL(KMaxLength);
    User::LeaveIfError(iRingingTone1NameProperty.Attach(KGASPSUidGlobalAudioSettings, KGASRingingTone1Name));
    TPtr16 ptr=iGlobalAudioSettingsData.iRingingTone1Name->Des();
    User::LeaveIfError(iRingingTone1NameProperty.Get(ptr));
    
    }
    
void CRingingTone1ObserverAO::Subscribe()
    {
    if (!IsActive())
        {
        SetActive();
        
        iRingingTone1NameProperty.Subscribe(iStatus);
        }
    }

void CRingingTone1ObserverAO::RunL()
    {
    TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CRingingTone1ObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        delete iGlobalAudioSettingsData.iRingingTone1Name;
        iGlobalAudioSettingsData.iRingingTone1Name=NULL;
        iGlobalAudioSettingsData.iRingingTone1Name=HBufC::NewL(KMaxLength);
        TPtr16 ptr=iGlobalAudioSettingsData.iRingingTone1Name->Des();
        status = iRingingTone1NameProperty.Get(ptr);
        if( status == KErrNone)
            {
            MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASRingingTone1Name;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CRingingToneObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
    }

void CRingingTone1ObserverAO::DoCancel()
    {
    iRingingTone1NameProperty.Cancel();
    }

TInt CRingingTone1ObserverAO::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

////////////////////////////////////////////////////////////////////
//  CRingingToneObserverAO                                                //
//////////////////////////////////////////////////////////////////////
CRingingTone2ObserverAO::CRingingTone2ObserverAO(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
    {
    CActiveScheduler::Add(this);
    }

CRingingTone2ObserverAO::~CRingingTone2ObserverAO()
    {
    Cancel();
    iRingingTone2NameProperty.Close();
    if(iGlobalAudioSettingsData.iRingingTone2Name)
        {
    delete iGlobalAudioSettingsData.iRingingTone2Name;
    iGlobalAudioSettingsData.iRingingTone2Name=NULL;
        }
     
    
    }

CRingingTone2ObserverAO* CRingingTone2ObserverAO::NewL(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
    {
    CRingingTone2ObserverAO* self = new (ELeave) CRingingTone2ObserverAO(
                                            aGlobalAudioSettings,
                                            aAudioSettingsObserver, 
                                            aGlobalAudioSettingsData);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CRingingTone2ObserverAO::ConstructL()
    {
    
    iGlobalAudioSettingsData.iRingingTone2Name=HBufC::NewL(KMaxLength);
    User::LeaveIfError(iRingingTone2NameProperty.Attach(KGASPSUidGlobalAudioSettings, KGASRingingTone2Name));
    TPtr16 ptr=iGlobalAudioSettingsData.iRingingTone2Name->Des();
    User::LeaveIfError(iRingingTone2NameProperty.Get(ptr));
    
    }
    
void CRingingTone2ObserverAO::Subscribe()
    {
    if (!IsActive())
        {
        SetActive();
        
        iRingingTone2NameProperty.Subscribe(iStatus);
        }
    }

void CRingingTone2ObserverAO::RunL()
    {
    TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CRingingTone2ObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        delete iGlobalAudioSettingsData.iRingingTone2Name;
        iGlobalAudioSettingsData.iRingingTone2Name=NULL;
        iGlobalAudioSettingsData.iRingingTone2Name=HBufC::NewL(KMaxLength);
        TPtr16 ptr=iGlobalAudioSettingsData.iRingingTone2Name->Des();
        status = iRingingTone2NameProperty.Get(ptr);
        if( status == KErrNone)
            {
        MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASRingingTone2Name;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CRingingToneObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
    }

void CRingingTone2ObserverAO::DoCancel()
    {
    iRingingTone2NameProperty.Cancel();
    }

TInt CRingingTone2ObserverAO::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

/////////////////////////////////////////////////////////////////////
//  CKeypadToneObserverAO                                                //
//////////////////////////////////////////////////////////////////////
CKeypadToneObserverAO::CKeypadToneObserverAO(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
    {
    CActiveScheduler::Add(this);
    }

CKeypadToneObserverAO::~CKeypadToneObserverAO()
    {
    Cancel();
    iKeypadToneVolumeProperty.Close();
    }

CKeypadToneObserverAO* CKeypadToneObserverAO::NewL(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
    {
    CKeypadToneObserverAO* self = new (ELeave) CKeypadToneObserverAO(
                                            aGlobalAudioSettings,
                                            aAudioSettingsObserver, 
                                            aGlobalAudioSettingsData);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CKeypadToneObserverAO::ConstructL()
    {
    
    
    User::LeaveIfError(iKeypadToneVolumeProperty.Attach(KGASPSUidGlobalAudioSettings, KGASKeypadToneVolume));
    TInt value=KErrNone;
    User::LeaveIfError(iKeypadToneVolumeProperty.Get(value));
    iGlobalAudioSettingsData.iKeyPadVolume=(CGlobalAudioSettings::TGASKeypadVolume)value;
    }
    
void CKeypadToneObserverAO::Subscribe()
    {
    if (!IsActive())
        {
        SetActive();
        
        iKeypadToneVolumeProperty.Subscribe(iStatus);
        }
    }

void CKeypadToneObserverAO::RunL()
    {
    TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CKeypadToneObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
        TInt value=KErrNone;
            status=iKeypadToneVolumeProperty.Get(value);
            iGlobalAudioSettingsData.iKeyPadVolume=(CGlobalAudioSettings::TGASKeypadVolume)value;
        
        
        if( status == KErrNone)
            {
            MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASKeyPadVolume;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CKeypadToneObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
    }

void CKeypadToneObserverAO::DoCancel()
    {
    iKeypadToneVolumeProperty.Cancel();
    }

TInt CKeypadToneObserverAO::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }


/////////////////////////////////////////////////////////////////////
//  CSilenceModeObserverAO                                                //
//////////////////////////////////////////////////////////////////////
CSilenceModeObserverAO::CSilenceModeObserverAO(
                        CGlobalAudioSettings &aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver,
                        TGlobalAudioSettings& aGlobalAudioSettingsData)
:CActive(EPriorityStandard),
 iGlobalAudioSettings(aGlobalAudioSettings),
 iAudioSettingsObserver(aAudioSettingsObserver),
 iGlobalAudioSettingsData(aGlobalAudioSettingsData)
    {
    CActiveScheduler::Add(this);
    }

CSilenceModeObserverAO::~CSilenceModeObserverAO()
    {
    Cancel();
    iSilenceModeProperty.Close();
    }

CSilenceModeObserverAO* CSilenceModeObserverAO::NewL(
                            CGlobalAudioSettings &aGlobalAudioSettings,
                            MAudioSettingsObserver& aAudioSettingsObserver,
                            TGlobalAudioSettings& aGlobalAudioSettingsData)
    {
    CSilenceModeObserverAO* self = new (ELeave) CSilenceModeObserverAO(
                                            aGlobalAudioSettings,
                                            aAudioSettingsObserver, 
                                            aGlobalAudioSettingsData);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CSilenceModeObserverAO::ConstructL()
    {
    
    
    User::LeaveIfError(iSilenceModeProperty.Attach(KGASPSUidGlobalAudioSettings, KGASSilenceMode));
   
    User::LeaveIfError(iSilenceModeProperty.Get(iGlobalAudioSettingsData.iSilenceMode));
    RDebug::Printf("in constructL,silence mode %d",iGlobalAudioSettingsData.iSilenceMode);
  
    }
    
void CSilenceModeObserverAO::Subscribe()
    {
    if (!IsActive())
        {
        SetActive();
        
        iSilenceModeProperty.Subscribe(iStatus);
        }
    }

void CSilenceModeObserverAO::RunL()
    {
    TInt status(iStatus.Int());
#ifdef PRINT_MESSAGE
    RDebug::Print(_L(" CSilenceModeObserverAO::RunL:iStatus[%d]"), status);
#endif // PRINT_MESSAGE
    if ( status == KErrNone )
        {
        Subscribe();
       
            status=iSilenceModeProperty.Get(  iGlobalAudioSettingsData.iSilenceMode);
            RDebug::Printf("value of silent mode :%d",iGlobalAudioSettingsData.iSilenceMode);
        
        
        if( status == KErrNone)
            {
            MAudioSettingsObserver::TGASEventId id=MAudioSettingsObserver::EGASSilenceMode;
            iAudioSettingsObserver.SettingsChanged(iGlobalAudioSettings, id);
            }
#ifdef PRINT_MESSAGE
        else
            {
            RDebug::Print(_L(" CKeypadToneObserverAO::RunL:Property.Get Error[%d]"), status);
            }
#endif // PRINT_MESSAGE
        }
    }

void CSilenceModeObserverAO::DoCancel()
    {
    iSilenceModeProperty.Cancel();
    }

TInt CSilenceModeObserverAO::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

//End of file
