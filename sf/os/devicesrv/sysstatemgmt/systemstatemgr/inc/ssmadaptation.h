// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMADAPTATION_H__
#define __SSMADAPTATION_H__

#include <e32base.h>

#include <ssm/ssmstate.h>
#include <ssm/ssmadaptation.hrh>

/**
 The UID2 value for SSM State Adaptation DLLs.
 @publishedPartner
 @released
 */
const TInt KStateAdaptationDllTypeUidValue=0x2000D76A;

/**
 The UID2 for SSM State Adaptation DLLs.
 @publishedPartner
 @released
 */
const TUid KStateAdaptationDllTypeUid={KStateAdaptationDllTypeUidValue};

/**
@publishedPartner
@released
*/
class MStateAdaptation
	{
public:
	/** 
	Request a Cooperating System to change system state.
	@param aState contains the new System State
	@param aStatus to complete when the operation has finished
	@see TSsmState
	@see TRequestStatus
	*/
	virtual void RequestCoopSysStateChange(TSsmState aState, TRequestStatus& aStatus) = 0;

	/**
	Request the Cooperating System to perform a self test.
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/
	virtual void RequestCoopSysSelfTest(TRequestStatus& aStatus) = 0;

	/**
	Request the Cooperating System to perform any required restart actions, since a device restart is about to be performed.
	@param aReason reason used by Cooperating System
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/
	virtual void RequestCoopSysPerformRestartActions(TInt aReason, TRequestStatus& aStatus) = 0;

	/**
	Request the Cooperating System to perform any required shutdown actions, since a device shutdown is about to be performed.
	@param aReason reason used by Cooperating System
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/
	virtual void RequestCoopSysPerformShutdownActions(TInt aReason, TRequestStatus& aStatus) = 0;

	/**
	Request the Cooperating System to Restore Factory Settings.
	@param aRfsType type of factory settings to be restored
	@param aStatus to complete when the operation has finished
	@see TSsmRfsType
	@see TRequestStatus
	*/
	virtual void RequestCoopSysPerformRfsActions(TSsmRfsType aRfsType, TRequestStatus& aStatus) = 0;

	/**
	This cancels a previous request from one of the other five Request methods in this class.
	Only one request is allowed to be outstanding on each connection session.
	*/
	virtual void RequestCancel() = 0;

	/**
	Requests notification from the Cooperating system of any events that occur.
	@param aEvent one of TSsmCoopSysEventType event's for which notification is requested, packaged in a descriptor
	@param aStatus to complete when the notification request has occurred
	@see TSsmCoopSysEventType
	@see TRequestStatus
	*/
	virtual void NotifyCoopSysEvent(TDes8& aEvent, TRequestStatus& aStatus) = 0;

	/**
	This cancels the outstanding request for notification of events.
	Only one request for notification of fatal events is allowed for each connected session.
	*/
	virtual void NotifyCancel() = 0;

	/**
	It is expected that Release will usually just call "delete this" on the object,
	but this will depend on the implementation of each policy.
	*/
	virtual void Release() = 0;
	};

/**
 The UID2 value for SSM RTC Adaptation DLLs.
 @publishedPartner
 @released
 */
const TInt KRtcAdaptationDllTypeUidValue=0x2000D76C;

/**
 The UID2 for SSM RTC Adaptation DLLs.
 @publishedPartner
 @released
 */
const TUid KRtcAdaptationDllTypeUid={KRtcAdaptationDllTypeUidValue};

/**
@publishedPartner
@released
*/
class MRtcAdaptation
	{
public:
	/**
	Check that the RTC is valid.
	@param aValidityPckg on return contains the status of the validity of the RTC as a boolean value
	@param aStatus to complete when the operation has finished
	@see TRequestStatus	
	*/
	virtual void ValidateRtc(TDes8& aValidityPckg, TRequestStatus& aStatus) = 0;

	/**
	Set a device wake-up alarm time, in UTC (coordinated universal time), in the RTC.
	@param aAlarmTimePckg requested wake up time
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/

	virtual void SetWakeupAlarm(TDesC8& aAlarmTimePckg, TRequestStatus& aStatus) = 0;

	/**
	Delete the current device wake-up alarm time in the RTC.
	@param aStatus to complete when the operation has finished
	@see TRequestStatus	
	*/
	virtual void UnsetWakeupAlarm(TRequestStatus& aStatus) = 0;

	/**
	This cancels a previous request from one of the other three methods in this class.
	Only one request is allowed to be outstanding on each connection session.
	*/
	virtual void Cancel() = 0;

	/** 
	It is expected that Release will usually just call "delete this" on the object, 
	but this will depend on the implementation of each policy. 
	*/
	virtual void Release() = 0;
	};

/**
 The UID2 value for SSM SIM Adaptation DLLs.
 @publishedPartner
 @released
 */
const TInt KSimAdaptationDllTypeUidValue=0x2000D76B;

/**
 The UID2 for SSM RTC Adaptation DLLs.
 @publishedPartner
 @released
 */
const TUid KSimAdaptationDllTypeUid={KSimAdaptationDllTypeUidValue};

/**
@publishedPartner
@released
*/
class MSimAdaptation
	{
public:
	/** 
	Check if the current SIM is among the group of recently used (i.e. Owned) SIM cards.
	@param aOwnedPckg SIM details
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/
	virtual void GetSimOwned(TDes8& aOwnedPckg, TRequestStatus& aStatus) = 0;
	
	/**
	This cancels the previous get request for SIM information.
	Only one request for SIM information (either owned or changed) can be outstanding on each connected session.
	*/
	virtual void GetCancel() = 0;
	
	/** 
	Requests notification of SIM events.
	@param aTypePckg contains the SIM event on notification completion
	@param aStatus to complete when the operation has finished
	@see TSsmSimEventType
	@see TRequestStatus
	*/
	virtual void NotifySimEvent(TDes8& aTypePckg, TRequestStatus& aStatus) = 0;
	
	/**
	This cancels the previous request for notification of SIM events.
	Only one request for SIM events can be outstanding on each connected session.
	*/
	virtual void NotifyCancel() = 0;
	
	/** 
	It is expected that Release will usually just call "delete this" on the object, 
	but this will depend on the implementation of each policy. 
	*/
	virtual void Release() = 0;	
	};

/**
 The UID2 value for SSM Emergency Call Rf Adaptation DLLs.
 @publishedPartner
 @released
 */
const TInt KEmergencyCallRfAdaptationDllTypeUidValue=0x2000E657;

/**
 The UID2 for SSM Emergency Call Rf Adaptation DLLs.
 @publishedPartner
 @released
 */
const TUid KEmergencyCallRfAdaptationDllTypeUid={KEmergencyCallRfAdaptationDllTypeUidValue};

/**
@publishedPartner
@released
*/
class MEmergencyCallRfAdaptation
	{
public:
	/**
	Request RF activation so that an emergency call can be made.
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/
	virtual void ActivateRfForEmergencyCall(TRequestStatus& aStatus) = 0;

	/**
	Request RF deactivation after an emergency call have been made.
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/
	virtual void DeactivateRfForEmergencyCall(TRequestStatus& aStatus) = 0;
	
	/**
	This cancels the previous outstanding request made for Activation/De-activation of emergency calls.
	Only one request is allowed to be outstanding for each connected session.
	*/	
	virtual void Cancel() = 0;

	/** 
	It is expected that Release will usually just call "delete this" on the object, 
	but this will depend on the implementation of each policy.
	*/
	virtual void Release() = 0;
	};

/**
 The UID2 value for SSM Miscellaneous Adaptation DLLs.
 @publishedPartner
 @released
 */
const TInt KMiscAdaptationDllTypeUidValue=0x2000E658;

/**
 The UID2 for SSM Miscellaneous Adaptation DLLs.
 @publishedPartner
 @released
 */
const TUid KMiscAdaptationDllTypeUid={KMiscAdaptationDllTypeUidValue};

/**
@publishedPartner
@released
*/
class MMiscAdaptation
	{
public:
	/**
	Request the next security state.
	@param aState next security state
	@param aResponsePckg response to state change request on completion
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/
	virtual void SecurityStateChange(TInt aState, TDes8& aResponsePckg, TRequestStatus& aStatus) = 0;

	/**
	Get the start-up mode from the Cooperating System.
	@param aModePckg should contain start-up of the Cooperating System on completion
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/
	virtual void GetGlobalStartupMode(TDes8& aModePckg, TRequestStatus& aStatus) = 0;

	/**
	Prepare language codes stored in the SIM in preferred language lists.
	The next call should be GetSimLanguagesL()
	@param aPriority priority for which the language lists need to be prepared
	@param aSizePckg on return contains the size of the buffer, which a client should allocate and pass as the one of the parameters (CBufBase* aBuf, TInt aCount) to GetSimLanguagesL()
	@param aStatus to complete when the operation has finished
	@see TSsmLanguageListPriority
	@see TRequestStatus
	@see GetSimLanguagesL
	*/
	virtual void PrepareSimLanguages(TSsmLanguageListPriority aPriority, TDes8& aSizePckg, TRequestStatus& aStatus) = 0;

	/**
	Get language codes stored in the SIM in preferred language lists.
	PrepareSimLanguages() call should precede this call
	@param aBuf should contain the language lists on completion
	@param aCount to contain the count of the languages
	@see PrepareSimLanguages
	*/
	virtual void GetSimLanguagesL(CBufBase* aBuf, TInt aCount) = 0;

	/**
	This cancels the previous request from any of the other four methods in this class.
	Only one request is allowed to be outstanding for each connected session.
	*/
	virtual void Cancel() = 0;

	/** 
	It is expected that Release will usually just call "delete this" on the object, 
	but this will depend on the implementation of each policy.
	*/
	virtual void Release() = 0;
	
	/** 
	Retrieve information about Hidden reset.
	@param aHiddenResetPckg Result package (ETrue or EFalse)
	@param aStatus to complete when the operation has finished
	@see TRequestStatus
	*/
	virtual void GetHiddenReset(TDes8& aHiddenResetPckg, TRequestStatus& aStatus) = 0;
	};

#endif // __SSMADAPTATION_H__
