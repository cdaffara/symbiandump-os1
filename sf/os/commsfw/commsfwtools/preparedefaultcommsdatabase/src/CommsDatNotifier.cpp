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
// Meta-database declarations 
// 
//

/**
 @file 
*/
 
#include "CommsDatInternalDefs.h"
#include "CommsDatTypeInfoV1_1.h"
#include "NotifierServ.H"
#include <cdbcols.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
using namespace CommsDat;

CCDNotifier::CCDNotifier(CMDBSessionImpl& aOwner)
 : iSuppressRollBackEvents(EFalse), iOwner(aOwner) 
    {
    }


CCDNotifier::~CCDNotifier()
    {
    CancelAllNotifications();
   
    iNotifications.Close();    
    }


CCDNotifier* CCDNotifier::NewL(CMDBSessionImpl& aOwner)
    {
	CCDNotifier* r=new(ELeave) CCDNotifier(aOwner);
    CleanupStack::PushL(r);
    r->ConstructL();
    CleanupStack::Pop();
	return r;
    }

void CCDNotifier::ConstructL()
    {
    }


TInt CCDNotifier::RegisterNotifyL(TRequestStatus& aStatus)
/**
@internalTechnology
*/
    {
    TInt ret(KErrNone);		
	if(!iNotifier.Handle())
                {
                ret = iNotifier.Connect(); //Also starts the server if not running allready
                }	
	
	if(ret == KErrNone)
               {
               iNotifier.RequestNotification(aStatus);
               }
               
        return ret;
    }



TInt CCDNotifier::CancelAllNotifications()
/**
@internalComponent
*/
    {
    if(iNotifier.Handle())
    {    	
    iNotifier.CancelAllNotifications();
    iNotifier.Close();
    }
    
    return KErrNone;
    }



TInt CCDNotifier::CancelNotify(TRequestStatus& aStatus)
/**
This function will cancel individual requests if given a particular TRequestStatus
  
@internalComponent
*/
    {
    if(iNotifier.Handle())
    {    	
    iNotifier.CancelNotification(aStatus);
    return KErrNone;
    }        
    
    return KErrNotFound;
    }


TInt CCDNotifier::NotifyClients(TInt aEventType)
/**
@internalTechnology
*/
    {
    // Discreetly drop the signalling of this event if our process lacks the necessary capabilities.
    //
    // For an EClose event, this means that readers finishing a session won't signal the fact that
    // they've closed a session. However, since they don't signal the fact that they've opened a 
    // session either, that's not such a concern.
    // 
    // For all other events, the client requires the same capability to Set the property as they
    // do for the associated CommsDat operation that it is signalling, so we presume that if they've
    // got this far, they have the necessary capabilities.
    //
    if (!RProcess().HasCapability(KCommsDatNotificationSetCapability, KSuppressPlatSecDiagnostic))
        {
        return KErrNone;
        }
        
    return RProperty::Set(KUidSystemCategory, KUidCommDbNotificationEvent.iUid, aEventType);
    }
    

//
// for Publish and Subscribe notifications


void CCDNotifier::NotifyAllChanges()
/*
Does the notification for all the changes that are stored in the member variable iNotifications.

@internalComponent
*/
	{
	TInt result(KErrNone);
	// we attempt all notifications regardless of the failure of individual ones since
	// different clients will be waiting on different notifications
	for (TInt i=0; i < iNotifications.Count(); ++i)
   		{
   		//nithin: iValue maybe redundant...could be removed
 		result = RProperty::Set(KUidSystemCategory, iNotifications[i].iUid.iUid, iNotifications[i].iValue);
 		if ( result != KErrNone)
            {
            // Log error but nothing to do
            }
   		}
	iNotifications.Reset();
	}

void CCDNotifier::ClearPubSubNotifications()
/*
Does the notification for all the changes that are stored in the member variable iNotifications.

@internalComponent
*/
	{
	iNotifications.Close();
	}

/*
Does the notification for all the changes that are stored in the member variable iNotifications.

@internalComponent
*/
void CCDNotifier::MaybeNotifyChange(TMDBElementId aElementId)
	{
	TCommsDatPubSubNotification notification;
	ConvertToUid(aElementId, notification);
	if (notification.iUid != KNullUid)
		{
	 	iNotifications.Append(notification); // nothing to do if fails
		}	
	}
	
/*
Does the notification for delete operations and stores in member variable iNotifications.

@internalComponent
*/
void CCDNotifier::MaybeNotifyChangeForDelete(TMDBElementId aElementId, TBool aModemBearerDeleted)
	{
	MaybeNotifyChange(aElementId);

	if (aModemBearerDeleted)
		{
		// there are three special notification when the modem bearer table is deleted
		
		TCommsDatPubSubNotification notification;

		// TSY name change notification
		const TUint32 rand1 = GetNewNumber(KUidCommDbModemTsyNameChange);
		notification.Set(KUidCommDbModemTsyNameChange, rand1);
		iNotifications.Append(notification);

		// Data and fax change notification
		const TUint32 rand2 = GetNewNumber(KUidCommDbModemDataAndFaxChange);
		notification.Set(KUidCommDbModemDataAndFaxChange, rand2);
		iNotifications.Append(notification);

		// Phone services and SMS change notification
		const TUint32 rand3 = GetNewNumber(KUidCommDbModemPhoneServicesAndSMSChange);
		notification.Set(KUidCommDbModemPhoneServicesAndSMSChange, rand3);
		iNotifications.Append(notification);		
		}
	}

void CCDNotifier::ConvertToUid(TMDBElementId aId, TCommsDatPubSubNotification& aNotification)

//Checks for the type of the setting passed and sets the value in the Commdb publish subscribe notification appropriately.

//@param aSetting Setting to get
//@param aVal Setting value.
//@param aNotification A reference to TCommDbPublishSubscribeNotification
	{
	
	if( (aId & KCDMaskShowRecordType) == KCDTIdDefaultWCDMARecord)
		{
		const TUint32 rand = GetNewNumber(KUidCommDbGPRSDefaultParamsChange);
	    aNotification.Set(KUidCommDbGPRSDefaultParamsChange, rand);	
		}
	else if((aId & KCDMaskShowRecordType) == KCDTIdModemBearerRecord)
		{
		const TUint32 rand = GetNewNumber(KUidCommDbModemRecordChange);
	    aNotification.Set(KUidCommDbModemRecordChange, rand);
	    
	    if((aId & KCDMaskShowType) == KCDTIdTsyName)
	    	{
	    	const TUint32 rand = GetNewNumber(KUidCommDbModemTsyNameChange);
	        aNotification.Set(KUidCommDbModemTsyNameChange, rand);	
	    	}    
		}
	else if((aId & KCDMaskShowRecordType) == KCDTIdProxiesRecord)
		{
		const TUint32 rand = GetNewNumber(KUidCommDbProxiesRecordChange);
	    aNotification.Set(KUidCommDbProxiesRecordChange, rand);	
		}
	else if((aId & KCDMaskShowRecordType) == KCDTIdGlobalSettingsRecord)
		{
		switch(aId & KCDMaskShowType)
        	{
	        case KCDTIdSMSBearer : // global settings field
		        {
		        aNotification.Set(KUidCommDbSMSBearerChange, 1);
	            break;
		        }
	        case KCDTIdSMSReceiveMode :  // global settings field
		        {
		        aNotification.Set(KUidCommDbSMSReceiveModeChange, 1);
		        break;
	            }
	        case KCDTIdGPRSAttachMode :  // global settings field
		        {
		        aNotification.Set(KUidCommDbGPRSAttachModeChange, 1);
		        break;
	            }      
	        case KCDTIdModemDataFax :  // Global setting field
		        {
		        const TUint32 rand = GetNewNumber(KUidCommDbModemDataAndFaxChange);
		        aNotification.Set(KUidCommDbModemDataAndFaxChange, rand);
		        break;
	            }
	        case KCDTIdModemPhoneServicesSMS :  // Global setting field
		        {
		        const TUint32 rand = GetNewNumber(KUidCommDbModemPhoneServicesAndSMSChange);
		        aNotification.Set(KUidCommDbModemPhoneServicesAndSMSChange, rand);
		        break;
	            }
        	};			
		}
				
	// else not an error since we dont want to notify
	}


TUint32 CCDNotifier::GetNewNumber(TUid aUid)
//Generates a new number for the given UID

//@param aUid UID for which the new number is to be generated
//@return The new number generated

	{
 	TInt value;
 	TInt result = RProperty::Get(KUidSystemCategory, aUid.iUid, value);
 	return ++value;	//don't worry about overflow - it doesn't matter
	}



//


TCommsDatPubSubNotification::TCommsDatPubSubNotification()
	: iUid(KNullUid), iValue(0)
    {
    }

	
void TCommsDatPubSubNotification::Set(TUid aUid,TInt aVal)
//Sets the given values of aUid and aVal to the members iUid and aVal.

//@param aUid UID to be set
//@param aVal Value to be set
	{
	iUid=aUid;
	iValue=aVal;
	}

//


RCommsdatNotifier::RCommsdatNotifier()
	/** Constructor. */
	{}

		
TInt RCommsdatNotifier::StartNotifierProcess()
	{	
	RProcess server;
	
	TInt ret=server.Create(KCommsdatNotifierExecutable, KNullDesC);
	
	if (ret!=KErrNone)
		{
		return ret;	
		}
	TRequestStatus stat;	
	server.Rendezvous(stat);
	server.Resume();
	server.Close();
	User::WaitForRequest(stat);
	return stat.Int();		
	}	
	
TInt RCommsdatNotifier::Connect()
	/** Connects to the server.
	*
	* 
	* @return	KErrNone if successful; otherwise, a system-wide error code. */
	{
	TInt ret = CreateSession(COMMSDATNOTIFIER_SERVER_NAME,Version(),-1);

	if (ret!=KErrNone)
		{		
		ret=StartNotifierProcess();
		if (ret==KErrNone || ret==KErrAlreadyExists)
			{
			ret = CreateSession(COMMSDATNOTIFIER_SERVER_NAME,Version(),-1);
			}
		}      
	
	if (ret==KErrNone)	
	{		
		ret = ShareProtected();
	}
		
	return ret;
	}

TVersion RCommsdatNotifier::Version() const
	/** Gets the client side version number.
	*
	* @return	The client version number. */
	{
	return(TVersion(KCommsdatNotifierMajorVersionNumber,KCommsdatNotifierMinorVersionNumber,KCommsdatNotifierSpecialBuildVersionNumber));
	}

void RCommsdatNotifier::RequestNotification(TRequestStatus& aStatus)
	{
	SendReceive(ERequestNotify, TIpcArgs(),aStatus);
	}

void RCommsdatNotifier::CancelNotification(TRequestStatus& aStatus)
/**
Cancel a previously registered notification.

@param aStatus the TRequestStatus of the notification.  This will be completed with KErrCancel.
*/
	{
	SendReceive(ECancelNotify, TIpcArgs(&aStatus));
	}	

void RCommsdatNotifier::CancelAllNotifications()
	{
	SendReceive(ECancelAllNotify, TIpcArgs());
	}		

void RCommsdatNotifier::Close()
	{
	RHandleBase::Close();
	}	

// EOF
