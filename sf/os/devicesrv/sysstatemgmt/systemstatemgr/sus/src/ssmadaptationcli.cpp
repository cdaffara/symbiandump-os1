// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssm/ssmadaptationcli.h>
#include <s32mem.h>
#include <e32base.h>
#include <e32cmn.h>
#include "susadaptionclisrv.h"

const TInt KAsyncMessageSlots = -1;


/**
 Connect this session without pre-allocating any asynchronous message slots.
 
 @return KErrNone, if successful, otherwise one of the other system-wide
 			error codes.
 
 @publishedPartner
 @released
 */
EXPORT_C TInt RSsmAdaptationBase::Connect()
	{
	return Connect(KAsyncMessageSlots);	 
	}

/**
 Connect this session and pre-allocate the number of asynchronous message slots you will need.
 I.e. the max number of asynchronoud calls to this call you will ever need.
 
 Use this version of @c Connect if you want to be guarantee allocation of slots rather
 than relying on slots being allocated from the system wide pool.

 @return KErrNone, if successful, otherwise one of the other system-wide
 			error codes.
 
 @publishedPartner
 @released
 */
EXPORT_C TInt RSsmAdaptationBase::Connect(TInt aAsyncMessageSlotCount)
	{
	const TInt err = DoConnect(KSusAdaptionServerName, aAsyncMessageSlotCount);
	return err;
	} 

/**
 * This exists for testing purposes only
 * @internalComponent
 */
#ifdef TEST_SSM_SERVER

TInt RSsmAdaptationBase::Connect(const TDesC& aServerName)
    {
    return DoConnect(aServerName, KAsyncMessageSlots);
    }
#endif //TEST_SSM_SERVER

/**
 @internalComponent
 */
TInt RSsmAdaptationBase::DoConnect(const TDesC& aServerName, const TInt aAsyncMessageSlotCount)
	{
	if(!Handle())
		{
		return CreateSession(aServerName, Version(), aAsyncMessageSlotCount);
		}
	
	return KErrAlreadyExists;
	}

/**
 @internalComponent
 @released
 */
TVersion RSsmAdaptationBase::Version() const
	{
	return(TVersion(KSsmAdaptationMajorVersionNumber, KSsmAdaptationMinorVersionNumber, KSsmAdaptationBuildVersionNumber));
	}

/**
 Requests the cooperating system to perform a state change matching the current system state.
  
 @param aState		The requested system state to change to for the cooperating system.
 					After succesful execution of this API the Cooperating system will be
 					the state aState
 
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current client
					process does not have the SecureID of the System State Manager, otherwise one of
					the other system-wide error codes.
					
 @note The aStatus can be completed with a value other than KErrNone if the cooperating system
 		has rejected the requested state change.
 
 @note This API can only be called by processes with the SecureID of the System State Manager.
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmStateAdaptation::RequestCoopSysStateChange(TSsmState aState, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ERequestCoopSysStateChange, TIpcArgs(aState.Int()), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	} //lint !e1746 Suppress parameter 'aState' could be made const reference

/**
 Requests the cooperating system to perform self tests.
  
 @param aStatus 	The request status.
					This is set to KErrNone on successful completion of self tests, KErrNotSupported
					if this operations is not supported by the adaptation, KErrPermissionDenied
					if the current process does not have the SecureID of the System State Manager,
					otherwise one of the other system-wide error codes.

 @note This API can only be called by processes with the SecureID of the System State Manager.
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmStateAdaptation::RequestCoopSysSelfTest(TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ERequestCoopSysSelfTest, aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Requests the cooperating system to perform any actions necessary before the system is restarted.
 
 @param aReason		The reason for the system restart. The meaning of this value is specific to
 					the	cooperating system.
  
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current process
					does not have the SecureID of the System State Manager, otherwise one of
					the other system-wide error codes.

 @note This API can only be called by processes with the SecureID of the System State Manager.
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmStateAdaptation::RequestCoopSysPerformRestartActions(TInt aReason, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ERequestCoopSysPerformRestartActions, TIpcArgs(aReason), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Requests the cooperating system to perform any actions necessary before the system is shutdown.
 
 @param aReason		The reason for the system shutdown. The meaning of this value is specific to
 					the	cooperating system.
  
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current process
					does not have the SecureID of the System State Manager, otherwise one of
					the other system-wide error codes.
 
 @note This API can only be called by processes with the SecureID of the System State Manager.
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmStateAdaptation::RequestCoopSysPerformShutdownActions(TInt aReason, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ERequestCoopSysPerformShutdownActions, TIpcArgs(aReason), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Requests the cooperating system to perform any actions necessary for restoring settings to default values.
 
 @param aType		The level of restoring settings to default values to perform.
 
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current process
					does not have the SecureID of the System State Manager, otherwise one of
					the other system-wide error codes.
 
 @note This API can only be called by processes with the SecureID of the System State Manager.
 
 @see TSsmRfsType
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmStateAdaptation::RequestCoopSysPerformRfsActions(TSsmRfsType aType, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ERequestCoopSysPerformRfsActions, TIpcArgs(aType), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/** 
 Cancels the current outstanding request from the cooperating system.
 
 Calling this API does not cancel any outstanding notification request. Use @c NotifyCancel()
 to cancel outstanding notification requests.
 
 If the request is successfully cancelled then the TRequestStatus associated with that request
 will be completed with KErrCancel.
  
 @publishedPartner
 @released
 */
EXPORT_C void RSsmStateAdaptation::RequestCancel()
	{
	if(Handle())
		{
		SendReceive(ERequestCoopSysCancel);
		}
	}

/** 
 Returns the last event occured in the aTypePckg which is a package of TSsmCoopSysEventType 
 
 Calling this API will get the last notification received by Adaptation server.If the client
 needs to be notified about the notifications client has to register using NotifyCoopSysEvent
 But once the client receives a notification it has to register again using NotifyCoopSysEvent
 for  further notification.
 To get any events that are missed between two notification requests the API GetLastCoopSysEvent
 can be used.
 This API can also be used to just get the last notified State event from Adaptation server.
  
 @publishedPartner
 @released
 */


EXPORT_C void RSsmStateAdaptation::GetLastCoopSysEvent(TDes8& aTypePckg)
	{
	if(Handle())
		{
		SendReceive(EGetLastCoopSysEvent, TIpcArgs(&aTypePckg));
		}
	}


/**
 Requests notification of cooperating system events.
 
 @param aChangedPckg 	A descriptor packaging a TSsmCoopSysEventType object, such as TPckg<TSsmCoopSysEventType>.
 					The descriptor referenced should exist until aStatus is completed.
 					When aStatus is completed with KErrNone this descriptor will contain a valid value
 					describing the cooperating system event.
 
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current process
					does not have the SecureID of the SSM Utility Server, otherwise one of
					the other system-wide error codes.
 
 @note This API can only be called by processes with the SecureID of the SSM Utility Server.
 
 @see TSsmCoopSysEventType
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmStateAdaptation::NotifyCoopSysEvent(TDes8& aTypePckg, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ENotifyCoopSysEvent, TIpcArgs(&aTypePckg), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Cancels the current outstanding request for notification of cooperating system events.
 
 Calling this API does not cancel any outstanding 'Request...' request. Use @c RequestCancel()
 to cancel outstanding 'Request...' requests.
 
 If the request is successfully cancelled then the TRequestStatus associated with the notification
 request will be completed with KErrCancel.
  
 @publishedPartner
 @released
 */
EXPORT_C void RSsmStateAdaptation::NotifyCancel()
	{
	if(Handle())
		{
		SendReceive(ENotifyCoopSysCancel);
		}
	}

/**
 Retrieve information about if the current SIM card, if any, is classed as owned.
 
 The conditions on a SIM card being classed as owned are dependent on the adaptation implementation.
 
 @param aOwnedPckg 	A descriptor packaging a TBool object, such as TPckg<TBool>. The descriptor
 					referenced should exist until aStatus is completed. When aStatus is completed
 					with KErrNone this descriptor will contain ETrue if the SIM card is owned, EFalse
 					otherwise.
 
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, otherwise one of the other system-wide 
					error codes.
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmSimAdaptation::GetSimOwned(TDes8& aOwnedPckg, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(EGetSimOwned, TIpcArgs(&aOwnedPckg), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Cancels the current outstanding request for either the SIM changed or SIM owned information.
 
 Calling this API does not cancel any outstanding notification request. Use @c NotifyCancel()
 to cancel outstanding notification requests.
 
 If the request is successfully cancelled then the TRequestStatus associated with that request
 will be completed with KErrCancel.
  
 @publishedPartner
 @released
 */
EXPORT_C void RSsmSimAdaptation::GetCancel()
	{
	if(Handle())
		{
		SendReceive(EGetSimCancel);
		}
	}

/** 
 Returns the last event occured in the aTypePckg which is a package of TSsmSimEventType 
 
 Calling this API will get the last notification received by Adaptation server.If the client
 needs to be notified about the notifications client has to register using NotifySimEvent
 But once the client receives a notification it has to register again using NotifySimEvent for 
 further notification.
 To get any events that are missed between two notification requests the API GetLastSimEvent
 can be used.
 This API can also be used to just get the last notified Sim event from Adaptation server.
  
 @publishedPartner
 @released
 */


EXPORT_C void RSsmSimAdaptation::GetLastSimEvent(TDes8& aTypePckg)
	{
	if(Handle())
		{
		SendReceive(EGetLastSimEvent, TIpcArgs(&aTypePckg));
		}
	}




/**
 Requests notification of SIM events.
 
 @param aChangedPckg 	A descriptor packaging a TSsmSimEventType object, such as TPckg<TSsmSimEventType>.
 					The descriptor referenced should exist until aStatus is completed. When
 					aStatus is completed with KErrNone this descriptor will contain a valid
 					value describing the SIM event.
 
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, otherwise one of the other system-wide 
					error codes.
 
 @see TSsmSimEventType
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmSimAdaptation::NotifySimEvent(TDes8& aTypePckg, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ENotifySimEvent, TIpcArgs(&aTypePckg), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Cancels the current outstanding request for notification of SIM events.
 
 Calling this API does not cancel any outstanding 'Get...' request. Use @c GetCancel()
 to cancel outstanding 'Get...' requests.
 
 If the request is successfully cancelled then the TRequestStatus associated with the notification
 request will be completed with KErrCancel.
  
 @publishedPartner
 @released
 */
EXPORT_C void RSsmSimAdaptation::NotifyCancel()
	{
	if(Handle())
		{
		SendReceive(ENotifySimCancel);
		}
	}

/**
 Retrieve information about if the real time clock, if present, contains a valid time.
 
 @param aValidityPckg 	A descriptor packaging a TBool object, such as TPckg<TBool>. The descriptor
 					referenced should exist until aStatus is completed. When aStatus is completed
 					with KErrNone this descriptor will contain ETrue if the real time clock is valid,
 					EFalse otherwise.
 
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, otherwise one of the other system-wide 
					error codes.
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmRtcAdaptation::ValidateRtc(TDes8& aValidityPckg, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(EValidateRtc, TIpcArgs(&aValidityPckg), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}	
	}

/**
 Request that the device wake-up at a certain time, even if the device is switched off.
 
 @param aAlarmTimePckg 	A descriptor packaging a TTime object, such as TPckgC<TTime>. The descriptor
 					referenced should exist until aStatus is completed. This descriptor should
 					contain a TTime object representing the UTC time that the device should wake-up.
 
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current process
					does not have the SecureID of the Alarm Server, otherwise one of
					the other system-wide error codes.
 
 @note This API can only be called by processes with the SecureID of the Alarm Server.
 
 @see TTime
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmRtcAdaptation::SetWakeupAlarm(TDesC8& aAlarmTimePckg, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ESetWakeupAlarm, TIpcArgs(&aAlarmTimePckg), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Stops the device from waking-up at a time set by a previous call to @c SetWakeupAlarm().
  
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current process
					does not have the SecureID of the Alarm Server, otherwise one of
					the other system-wide error codes.
 
 @note This API can only be called by processes with the SecureID of the Alarm Server.
 
 @see RSsmRtcAdaptation::SetWakeupAlarm()
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmRtcAdaptation::UnsetWakeupAlarm(TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(EUnsetWakeupAlarm, aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Cancels the current outstanding request on this R class.
 
 Calling this API does not cancel any wake-up time that is set. Use @c UnsetWakeupAlarm() to do this.
 
 If the request is successfully cancelled then the TRequestStatus associated with that request
 will be completed with KErrCancel.
  
 @publishedPartner
 @released
*/
EXPORT_C void RSsmRtcAdaptation::Cancel()
	{
	if(Handle())
		{
		SendReceive(ERtcCancel);
		}
	}

/**
 Communicates a security state change with the adaptation.
 
 @param aState      The security state that has been changed to by the security state machine. The
 					meaning of this value is dependent on the adaptation.
 
 @param aResponsePckg	A descriptor packaging a TInt object, such as TPckg<TInt>. The descriptor
 					referenced should exist until aStatus is completed. This descriptor will contain
 					the response from the adaptation to this security state change. The meaning of this
 					value is dependent on the adaptation.
  
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current process
					does not have the SecureID of the System State Manager, otherwise one of
					the other system-wide error codes.
 
 @note This API can only be called by processes with the SecureID of the System State Manager.
 
 @publishedPartner
 @released
*/
EXPORT_C void RSsmMiscAdaptation::SecurityStateChange(TInt aState, TDes8& aResponsePckg, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(ESecurityStateChange, TIpcArgs(aState, &aResponsePckg), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Retrieves the startup mode from the adaptation.
 
 @param aModePckg	A descriptor packaging a TInt object, such as TPckg<TInt>. The descriptor
 					referenced should exist until aStatus is completed. When aStatus is completed
 					with KErrNone this descriptor will contain the startup mode. The meaning of this
 					startup mode value is dependent on the adaptation.
 
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current process
					does not have the SecureID of the System State Manager, otherwise one of
					the other system-wide error codes.
 
 @note This API can only be called by processes with the SecureID of the System State Manager.
 
 @publishedPartner
 @released
*/
EXPORT_C void RSsmMiscAdaptation::GetGlobalStartupMode(TDes8& aModePckg, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(EGetGlobalStartupMode, TIpcArgs(&aModePckg), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
  Retrieve information about Hidden reset.

 @param aHiddenResetPckg	A descriptor packaging a TBool object, such as TPckg<TBool>. The descriptor
 							referenced should exist until aStatus is completed. When aStatus is completed
 							with KErrNone this descriptor will contain ETrue if Hidden reset is to be done , EFalse
 							otherwise.
 
@param aStatus 				The request status. This is set to KErrNone on successful completion, KErrNotSupported if this 
                            operations is not supported by the adaptation, KErrPermissionDenied if the current process
							does not have the SecureID of the System State Manager, KErrDisconnected if there is no handle to
							adaptation server, otherwise one of the other system-wide error codes.
 
 @note This API can only be called by processes with the SecureID of the System State Manager.
  
 @publishedPartner
 @released
*/
EXPORT_C void RSsmMiscAdaptation::GetHiddenReset(TDes8& aHiddenResetPckg, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(EGetHiddenReset, TIpcArgs(&aHiddenResetPckg), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Prepares the SIM languages list for retrieval.
 
 @param aPriority  Specifies which languages list should be prepared for retrieval.
 
 @param aSizePckg   A descriptor packaging a TInt object, such as TPckg<TInt>. The descriptor
 					referenced should exist until aStatus is completed. When aStatus is completed
 					with KErrNone this descriptor will contain the number of elements in the
 					language list.
 
 @param aStatus 	The request status.
					This is set to KErrNone on completion, KErrNotSupported if this operations 
					is not supported by the adaptation, KErrPermissionDenied if the current process
					does not have the SecureID of the System State Manager, otherwise one of
					the other system-wide error codes.
 
 @note This API can only be called by processes with the SecureID of the System State Manager.
 
 @see RSsmMiscAdaptation::SimLanguagesArrayL()
 
 @publishedPartner
 @released
*/
EXPORT_C void RSsmMiscAdaptation::PrepareSimLanguages(TSsmLanguageListPriority aPriority, TDes8& aSizePckg, TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(EPrepareSimLanguages, TIpcArgs(aPriority, &aSizePckg), aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Retrieves the SIM lanugages list.
 
 This will retrieve the SIM languages list in a dynamic array, the list was prepared by a previous successful call to @c PrepareSimLanguages().
 
 @param aSimLanguageCount   The count of the languages as prepared in a previous call to @c PrepareSimLanguages().
                            This count is used to allocate and expand the size of the flat buffer. 
                                                
 @return A pointer to the fixed length array CArrayFix<TInt32> of language codes.
 
 @note This API can only be called by processes with the SecureID of the System State Manager.
 
 @see RSsmMiscAdaptation::PrepareSimLanguages
 
 @publishedPartner
 @released
*/
EXPORT_C CArrayFix<TInt32>* RSsmMiscAdaptation::SimLanguagesArrayL(TInt aSimLanguageCount)
	{
	if(! Handle())
		{
		User::Leave(KErrDisconnected);
		}
	CArrayFix<TInt32>* clientArray = new (ELeave) CArrayFixFlat<TInt32>(2);
	CleanupStack::PushL(clientArray);

	CBufFlat* buffer=CBufFlat::NewL(aSimLanguageCount);
	CleanupStack::PushL(buffer);
	const TInt length = aSimLanguageCount * sizeof(TInt32);
	buffer->ExpandL(0,length);
	TPtr8 bufPtr = buffer->Ptr(0);
	User::LeaveIfError(SendReceive(EGetSimLanguagesArray, TIpcArgs(&bufPtr, aSimLanguageCount)));

	RBufReadStream readStream(*buffer);
	CleanupClosePushL(readStream);
	
	TInt32 langId = 0; //Initialized with value 0 because this value is not a language id 
	for (TInt i=0; i < aSimLanguageCount; ++i)
		{
		langId = readStream.ReadInt32L();
		clientArray->AppendL(langId);  
		}
	
	CleanupStack::PopAndDestroy(2); //readStream, buffer
	CleanupStack::Pop(clientArray);
	return clientArray;
	}

/**
 Cancels the current outstanding request on this R class.
 
 If the request is successfully cancelled then the TRequestStatus associated with that request
 will be completed with KErrCancel.
 
 @publishedPartner
 @released
*/
EXPORT_C void RSsmMiscAdaptation::Cancel()
	{
	if(Handle())
		{
		SendReceive(EMiscCancel);
		}
	}


/**
 Requests the adaptation to enable RF communication for emergency calls.
   
 @param aStatus 	The request status.
					This is set to KErrNone on successful completion of self tests, KErrNotSupported
					if this operations is not supported by the adaptation, KErrPermissionDenied
					if the current process does not have sufficient capabilities, otherwise one
					of the other system-wide error codes.

 @capability WriteDeviceData
 @capability PowerMgmt
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmEmergencyCallRfAdaptation::ActivateRfForEmergencyCall(TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(EActivateRfForEmergencyCall, aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
  Requests the adaptation to disable RF communication for emergency calls.
   
 @param aStatus 	The request status.
					This is set to KErrNone on successful completion of self tests, KErrNotSupported
					if this operations is not supported by the adaptation, KErrPermissionDenied
					if the current process does not have sufficient capabilities, otherwise one
					of the other system-wide error codes.
					
 @capability WriteDeviceData
 @capability PowerMgmt
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmEmergencyCallRfAdaptation::DeactivateRfForEmergencyCall(TRequestStatus& aStatus)
	{
	if(Handle())
		{
		SendReceive(EDeactivateRfForEmergencyCall, aStatus);
		}
	else
		{
		aStatus = KRequestPending;
		TRequestStatus* clientStatus = &aStatus;
		User::RequestComplete(clientStatus, KErrDisconnected);
		}
	}

/**
 Cancels the current outstanding request on this R class.
 
 If the request is successfully cancelled then the TRequestStatus associated with that request
 will be completed with KErrCancel.
 
 @publishedPartner
 @released
 */
EXPORT_C void RSsmEmergencyCallRfAdaptation::Cancel()
	{
	if(Handle())
		{
		SendReceive(EEmergencyCallRfCancel);
		}
	}

/**
 The Session calling this API will be set as priorityClient. 
 This has to be called after Connect() and before calling ActivateRfForEmergencyCall(). This will allow priority client to
 make emergency call during OOM condition.
 Session having Telephony SecureId can only be set as a PriorityClient by SSM.
 This API preloads the emergencyclient dll if it is not loaded and also allows to use reserved
 memory in OOM condition
 @return KErrNone, if successful, KErrAlreadyExists if there is already a priority client set, 
                   KErrDisconnected, if there is no handle, KErrNoMemory in case of OOM,otherwise one of the other system-wide error codes.
 @publishedPartner
 @released
 */
EXPORT_C TInt RSsmEmergencyCallRfAdaptation::SetAsPriorityClient()
    {
    if(Handle()) 
        {        
        return(SendReceive(ESetAsPriorityClient));
        }
    return KErrDisconnected;
    }

