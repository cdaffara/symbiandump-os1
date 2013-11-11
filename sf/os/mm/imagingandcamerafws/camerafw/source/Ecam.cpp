// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ecam.h>
#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>
#include "ecamversion.h"
#include <mm/mmpluginutils.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecamuidsdef.hrh>
#include <ecamconst.h>
#endif
_LIT(KECamImplFaulty,"FaultyECamImpl");
const TUint KPanicECamWrongEventClassUsed = 1;

const TInt KCameraIndex0 = 0;
const TInt KCameraIndex1 = 1;
const TInt KCameraIndex2 = 2;
const TInt KCameraIndex3 = 3;
const TInt KCameraIndex4 = 4;
const TInt KCameraIndex5 = 5;
const TInt KCameraIndex6 = 6;
const TInt KCameraIndex7 = 7;

EXPORT_C TECAMEvent::TECAMEvent(TUid aEventType, TInt aErrorCode)
: iEventType(aEventType), iErrorCode(aErrorCode)
	{
	}

EXPORT_C TECAMEvent::TECAMEvent()
	{
	}

/**
This method may be used by MCameraObserver2::HandleEvent to know whether a particular event received 
has been encapsulated in a correct version of TECAMEvent base class. 
Indirectly, its provided implementation also specifies different events which are supposed to use a specific version
of TECAMEvent base class.
This static method will be provided per derived version of TECAMEvent base class. So, MCameraObserver2::HandleEvent 
implementation may consist of 2 parts: 
1) Recognize the class which encapsulated the event(aECAMEvent) and type-cast to that class.
2) Recognize the event.

Note : MCameraObserver2::HandleEvent should ignore unrecognised events and unrecognized version of TECAMEvent base 
class. Unless clearly stated to use a particular derived version of TECAMEvent base class, events should use 
class TECAMEvent.
	
@param  aECAMEvent
	    TECAMEvent class reference providing details of a particular event

@return ETrue : This implies that the received event is recognized and belongs to this class. 
		MCameraObserver2::HandleEvent implementation may type-cast the aECAMEvent to TECAMEvent2 and proceed.
		EFalse : This is not an error condition. This implies that the event does not belong to this class. If event 
		is a known event, this may belong to the base class or to some other version of base class.  
		MCameraObserver2::HandleEvent implementation may call any other such methods existing for different derived versions 
		of TECAMEvent base class untill ETrue is retrieved.
		If EFalse is retrieved in each of such methods, implementation may type-cast the aECAMEvent to 
		the base class and proceed further to recognise the event. This implies that:-
		1) the given event is unrecognised and belong to base class or any derived class or an unrecognised derived class.
		2) the given event is recognised and belong to base class. 
						
*/
EXPORT_C TBool TECAMEvent2::IsEventEncapsulationValid(const TECAMEvent& aECAMEvent)
	{
	/*
	checks whether the event type being represented by aECAMEvent belongs to this class or not.
	*/
	
	if(aECAMEvent.iEventType.iUid <= KECamUidEvent2RangeEnd && aECAMEvent.iEventType.iUid >= KECamUidEvent2RangeBegin)
		{
		TECAMEvent2 ecamEvent = static_cast<const TECAMEvent2&>(aECAMEvent);
			
		/*
		event type is being correctly represented only if its class uid 
		matches with uid used for class TECAMEvent2
		*/
		if (ecamEvent.EventClassUsed().iUid == KUidECamEventClass2UidValue)
			{
			return ETrue;
			}
		/*
		event type is not being correctly represented only if its class uid does not
		match with uid used for class TECAMEvent2. Faulty ECam Implementation
		*/
		else
			{
			User::Panic(KECamImplFaulty, KPanicECamWrongEventClassUsed);
			}
		}
			
	switch(aECAMEvent.iEventType.iUid)
		{
		//fall through
		case KUidECamEventCameraSettingPreCaptureWarningUidValue:
		case KUidECamEventCIPSetColorSwapEntryUidValue:
		case KUidECamEventCIPRemoveColorSwapEntryUidValue:
		case KUidECamEventCIPSetColorAccentEntryUidValue:
		case KUidECamEventCIPRemoveColorAccentEntryUidValue:
		/*
		event type belongs to this class
		*/
			{
			TECAMEvent2 ecamEvent = static_cast<const TECAMEvent2&>(aECAMEvent);
			
			/*
			event type is being correctly represented only if its class uid 
			matches with uid used for class TECAMEvent2
			*/
			if (ecamEvent.EventClassUsed().iUid == KUidECamEventClass2UidValue)
				{
				return ETrue;
				}
			/*
			event type is not being correctly represented only if its class uid does not
			match with uid used for class TECAMEvent2. Faulty ECam Implementation
			*/
			else
				{
				User::Panic(KECamImplFaulty, KPanicECamWrongEventClassUsed);
				}
			}
		
		default: 
			/*
			Ignore if event type does not belong to this class
			*/
			return EFalse;
		}
	}

/**
Constructor.

@param  aEventType
        A UID to define the type of event.
        
@param  aErrorCode
        The error code associated with the event.

@param  aParam
		This signifies different things for different valid events.
		
		For example: 
		For KUidECamEventCIPSetColorSwapEntry; KUidECamEventCIPRemoveColorSwapEntry; KUidECamEventCIPSetColorAccentEntry
		and KUidECamEventCIPRemoveColorAccentEntry, aParam represents color entry.
		
		For KUidECamEventCameraSettingPreCaptureWarning, aParam represents bit field describing all PreCaptureWarnings currently issued.
		
		Future events may also use this class.
*/
EXPORT_C TECAMEvent2::TECAMEvent2(TUid aEventType, TInt aErrorCode, TInt aParam)
: TECAMEvent(aEventType, aErrorCode), iEventClassUsed(KUidECamEventClass2), iParam(aParam)
	{	
	}

/**
Gives the uid representing this version of TECAMEvent base class.

@return Uid representing this version of TECAMEvent base class. Uid used is KUidECamEventClass2.
*/
EXPORT_C const TUid& TECAMEvent2::EventClassUsed() const
	{
	return iEventClassUsed;
	}
  	

EXPORT_C TInt CCamera::CamerasAvailable()
	{
	return CCameraPlugin::CamerasAvailable();
	}


EXPORT_C CCamera* CCamera::NewL(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority)
	{
	return CCameraPlugin::NewL(aObserver, aCameraIndex, aPriority, KCameraDefaultVersion);
	}

EXPORT_C CCamera* CCamera::NewDuplicateL(MCameraObserver2& aObserver,TInt aCameraHandle)
	{
	return CCameraPlugin::NewDuplicateL(aObserver, aCameraHandle, KCameraDefaultVersion);
	}


EXPORT_C CCamera* CCamera::NewL(MCameraObserver& aObserver,TInt aCameraIndex)
	{
	return CCameraPlugin::NewL(aObserver, aCameraIndex, KCameraDefaultVersion);
	}

EXPORT_C CCamera* CCamera::NewDuplicateL(MCameraObserver& aObserver,TInt aCameraHandle)
	{
	return CCameraPlugin::NewDuplicateL(aObserver, aCameraHandle, KCameraDefaultVersion);
	}
	
EXPORT_C CCamera* CCamera::New2L(MCameraObserver2& aObserver,TInt aCameraIndex,TInt aPriority)
	{
	return CCameraPlugin::NewL(aObserver, aCameraIndex, aPriority,KCameraFirstVersion);
	}

EXPORT_C CCamera* CCamera::NewDuplicate2L(MCameraObserver2& aObserver,TInt aCameraHandle)
	{
	return CCameraPlugin::NewDuplicateL(aObserver, aCameraHandle, KCameraFirstVersion);
	}
	
EXPORT_C TInt CCamera::CameraVersion()
	{
	return((static_cast<CCameraPlugin*>(this))->CameraVersion());
	}

/**
@internalComponent

Used to create the CCameraStatusWatch class object. This is needed by the method TReservedInfo::SubscribeReserveInfoL
in order to grab the reserve notification from the ECam implementation and hence further forward it to the observer
MReserveObserver.

@param aReserveObserver
	   Reference to an observer.

@param aCameraIndex
	   The camera index for which the subscription is needed.
	   
@param aSecureId
	   The secure ID of the process where serialized part of the ECam implementation runs.

@leave May leave with any error code
*/
CCameraStatusWatch* CCameraStatusWatch::NewL(MReserveObserver& aReserveObserver, TInt aCameraIndex, TInt aSecureId)
	{
	CCameraStatusWatch* self = new(ELeave) CCameraStatusWatch(aReserveObserver, aCameraIndex, aSecureId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Destructor. 
The destructor has to be called by the client when it does not need the desired reserve notifications any more.

@publishedPartner
@prototype
*/
EXPORT_C CCameraStatusWatch::~CCameraStatusWatch()
	{
	Cancel();
	iProperty.Close();
	}

/**
Constructor
*/	
CCameraStatusWatch::CCameraStatusWatch(MReserveObserver& aReserveObserver, TInt aCameraIndex, TInt aSecureId): 
																					   CActive(EPriorityHigh), 
																					   iReserveObserver(aReserveObserver),
																					   iCameraIndex(aCameraIndex),
																					   iSecureId(aSecureId)
	{
	}

/**
Second Phase Constructor.
Attaches to the Property as implied by the camera index, adds the active object to the active scheduler list and kicks
off the	RunL.

@leave May leave with any error code

@note Serialized part of the ECam implementation is assumed to define the Properties and publish them whenever there is 
	  change in the reserve status.
*/
void CCameraStatusWatch::ConstructL()
	{
	switch(iCameraIndex)
		{
		case KCameraIndex0:
			{
			User::LeaveIfError(iProperty.Attach(TUid::Uid(iSecureId), KUidECamPropertyCameraIndex0ReservedStatus));
			break;	
			}
		case KCameraIndex1:
			{
			User::LeaveIfError(iProperty.Attach(TUid::Uid(iSecureId), KUidECamPropertyCameraIndex1ReservedStatus));
			break;	
			}
		case KCameraIndex2:
			{
			User::LeaveIfError(iProperty.Attach(TUid::Uid(iSecureId), KUidECamPropertyCameraIndex2ReservedStatus));
			break;	
			}
		case KCameraIndex3:
			{
			User::LeaveIfError(iProperty.Attach(TUid::Uid(iSecureId), KUidECamPropertyCameraIndex3ReservedStatus));
			break;	
			}
		case KCameraIndex4:
			{
			User::LeaveIfError(iProperty.Attach(TUid::Uid(iSecureId), KUidECamPropertyCameraIndex4ReservedStatus));
			break;	
			}
		case KCameraIndex5:
			{
			User::LeaveIfError(iProperty.Attach(TUid::Uid(iSecureId), KUidECamPropertyCameraIndex5ReservedStatus));
			break;	
			}
		case KCameraIndex6:
			{
			User::LeaveIfError(iProperty.Attach(TUid::Uid(iSecureId), KUidECamPropertyCameraIndex6ReservedStatus));
			break;	
			}
		case KCameraIndex7:
			{
			User::LeaveIfError(iProperty.Attach(TUid::Uid(iSecureId), KUidECamPropertyCameraIndex7ReservedStatus));
			break;	
			}
		default:
			{
			User::Leave(KErrArgument);
			}
		}

	CActiveScheduler::Add(this);
	
	iProperty.Subscribe(iStatus);
	SetActive();
	}

/**
Re-issues the subscription notice. Checks the updated value of the Property and notifies the client via 
MReserveObserver. 

@note Serialized part of the ECam implementation is assumed to define the Properties and publish them whenever there is 
	  change in the reserve status.
*/
void CCameraStatusWatch::RunL()
	{
	TBool reserved;
	TInt error = iProperty.Get(reserved);
	
	TECamReserveStatus reserveStatus;
	if(error != KErrNone)
		{
		reserveStatus = ECameraStatusUnknown;
		}
	else
		{
		if(!reserved)
			{
			reserveStatus = ECameraUnReserved;
			}
		else
			{
			reserveStatus = ECameraReserved;
			}	
		}
	
	iReserveObserver.ReserveStatus(iCameraIndex, reserveStatus, error);
	
	if(error == KErrNone)
		{
		iProperty.Subscribe(iStatus);
		SetActive();
		}
	}	

/** 
Cancels any outstanding subscription.
*/
void CCameraStatusWatch::DoCancel()
	{
	iProperty.Cancel();
	}

/**
Client uses it to subscribe for the reserve notification of particular camera. The notifications are send to the client
through MReserveObserver callbacks. Should the client not wish to continue with the subscription, it is supposed
to delete the CCameraStatusWatch class pointer passed to it. 

@param aReserveObserver
	   Reference to an observer
	   
@param aCameraIndex
	   The index of the camera for which the subscription is to be done.
	   
@param aCameraStatusWatch
	   Reference to the CCameraStatusWatch*. Client is supposed to delete the CCameraStatusWatch object once it does not wish to
	   further continue with the subscription.
	   
@leave May leave with any error code.
*/
EXPORT_C void TReservedInfo::SubscribeReserveInfoL(MReserveObserver& aReserveObserver, TInt aCameraIndex, CCameraStatusWatch*& aCameraStatusWatch)
	{
	// Get the MSecureIdPlugin 
	TUid interfaceUid = {KUidOnboardCameraSecureIdPlugin};
	TUid dtor;
	MSecureIdPlugin* secureIdInfo = 
		static_cast<MSecureIdPlugin*>
			(MmPluginUtils::CreateImplementationL(interfaceUid, dtor, KECamPluginMatchString, KRomOnlyResolverUid));
												  
	CleanupStack::PushL(secureIdInfo);
	
	//Retrieve the secure ID
	TInt secureId = 0;
	secureIdInfo->GetSecureIdL(secureId);
	
	secureIdInfo->Release();
	
	CleanupStack::Pop(secureIdInfo);

	REComSession::DestroyedImplementation(dtor);
	REComSession::FinalClose(); // don't have to do this here, but might as well tidy up
	
	//create CCameraStatusWatch and kicks off RunL. This implies property attached and subscribed.
	CCameraStatusWatch* cameraStatusWatch = CCameraStatusWatch::NewL(aReserveObserver, aCameraIndex, secureId);
	aCameraStatusWatch = cameraStatusWatch;
	}
	
/**
Default Constructor
*/		
EXPORT_C CCamera::CCamera()
	{	
	}
