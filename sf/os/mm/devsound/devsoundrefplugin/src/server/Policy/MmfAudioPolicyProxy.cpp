// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include <e32math.h>
#include "mmfAudioPolicyProxy.h"
#include "MmfAudioPolicyStart.h"
#include "MmfBase.hrh"
#include "MmfDevSoundInfo.h"
#include "MmfAudioPolicyServer.h"



EXPORT_C TInt RMMFAudioPolicyProxy::Open(RServer2& aPolicyServerHandle)
	{
	ASSERT(aPolicyServerHandle.Handle());
	
	// Server is already running and attempt to create a session
	// 4 message slots
	TInt err = CreateSession(aPolicyServerHandle, TVersion(KMMFAudioPolicyVersion,
													KMMFAudioPolicyMinorVersionNumber,
													KMMFAudioPolicyBuildVersionNumber));		
	return err;		
	}

EXPORT_C TInt RMMFAudioPolicyProxy::CreateServer(RServer2& aPolicyServerHandle)
	{
	TServerStart start(aPolicyServerHandle);
	TThreadFunction serverFunc = CMMFAudioPolicyServer::StartThread;
	RThread server;
	TInt err = server.Create(_L(""),serverFunc, KAudioPolicyServerStackSize,
							KAudioPolicyServerInitHeapSize, KAudioPolicyServerMaxHeapSize,
							&start, EOwnerProcess);
	if(err != KErrNone)
		{
		return err;
		}		
		// Synchronise with the server
	TRequestStatus reqStatus;
	server.Rendezvous(reqStatus);
	if (reqStatus!=KRequestPending)
		{
		server.Kill(0);
		}
	else 
		{
		// Start the test harness
		server.Resume();
		// Server will call the reciprocal static synchronise call
		}
	User::WaitForRequest(reqStatus); // wait for start or death
	server.Close();
	if(reqStatus.Int() != KErrNone)
		{
		return reqStatus.Int();
		}
	return err;		
	}

EXPORT_C TInt RMMFAudioPolicyProxy::SetDevSoundInfo(TMMFDevSoundInfo& aDevSoundInfo)
	{
	TMMFDevSoundInfoPckg psPckg(aDevSoundInfo);
	return SendReceive(EMMFPolicySetDevSoundInfo, psPckg);
	}

EXPORT_C void RMMFAudioPolicyProxy::MakeRequest(TMMFAudioPolicyPrioritySettings& aPrioritySettings)
	{
	iPsPckg().iState = aPrioritySettings.iState;
	iPsPckg().iPref = aPrioritySettings.iPref;
	iPsPckg().iPriority = aPrioritySettings.iPriority;
	iPsPckg().iCapabilities = aPrioritySettings.iCapabilities;
	SendReceive(EMMFPolicyMakeRequest, iPsPckg);
	}

EXPORT_C TInt RMMFAudioPolicyProxy::UpdateState(TMMFAudioPolicyPrioritySettings& aPrioritySettings)
	{
	TMMFAudioPolicyPrioritySettingsPckg psPckg(aPrioritySettings);
	return SendReceive(EMMFPolicyUpdateState, psPckg);
	}

EXPORT_C void RMMFAudioPolicyProxy::ReceiveEvents(TMMFAudioPolicyEventPckg& aEventPckg, TRequestStatus& aStatus)
	{
	SendReceiveResult(EMMFPolicyReceiveEvents, aEventPckg, aStatus);
	}

EXPORT_C TInt RMMFAudioPolicyProxy::CancelReceiveEvents()
	{
	return SendReceive(EMMFPolicyCancelReceiveEvents);
	}

EXPORT_C TInt RMMFAudioPolicyProxy::GetPlayFormatsSupported(RMdaDevSound::TSoundFormatsSupportedBuf& aPlayFormatsSupported) 
	{
	RMdaDevSound::TSoundFormatsSupportedBuf playFormatsSupported;
	TInt err = SendReceiveResult(EMMFPolicyGetPlayFormatsSupported, playFormatsSupported);
	if (!err)
		aPlayFormatsSupported = playFormatsSupported;
	return  err;
	}

EXPORT_C TInt RMMFAudioPolicyProxy::GetRecordFormatsSupported(RMdaDevSound::TSoundFormatsSupportedBuf& aRecordFormatsSupported) 
	{
	RMdaDevSound::TSoundFormatsSupportedBuf recordFormatsSupported;
	TInt err = SendReceiveResult(EMMFPolicyGetRecordFormatsSupported, recordFormatsSupported);
	if (!err)
		aRecordFormatsSupported = recordFormatsSupported;
	return  err;
	}

EXPORT_C TInt RMMFAudioPolicyProxy::GetPlayFormat(RMdaDevSound::TCurrentSoundFormatBuf& aPlayFormat) 
	{
	RMdaDevSound::TCurrentSoundFormatBuf playFormat;
	TInt err = SendReceiveResult(EMMFPolicyGetPlayFormat, playFormat);
	if (!err)
		aPlayFormat = playFormat;
	return  err;
	}

EXPORT_C TInt RMMFAudioPolicyProxy::GetRecordFormat(RMdaDevSound::TCurrentSoundFormatBuf& aRecordFormat) 
	{
	RMdaDevSound::TCurrentSoundFormatBuf recordFormat;
	TInt err = SendReceiveResult(EMMFPolicyGetRecordFormat, recordFormat);
	if (!err)
		aRecordFormat = recordFormat;
	return  err;
	}

EXPORT_C TInt RMMFAudioPolicyProxy::LaunchRequests()
	{
	return SendReceive(EMMFPolicyLaunchRequests);
	}

EXPORT_C TInt RMMFAudioPolicyProxy::RequestResourceNotification(TUid aNotificationEventUid, const TDesC8& aNotificationDelay)
	{
	TUid eventType = KNullUid;
	TMMFAudioPolicyResourceNotificationSettingsPckg pckg;
	pckg().iNotificationUid = eventType;
	TInt err = SendReceiveResult(EMMFPolicyGetResourceNotificationEvent, pckg);
	eventType = pckg().iNotificationUid;
	if(!err)
		{
		if(eventType != aNotificationEventUid )
			{
			pckg().iNotificationUid = aNotificationEventUid;
			pckg().iNotificationDelay = aNotificationDelay;
			return SendReceive(EMMFPolicyRequestResourceNotification, pckg);		
			}
		else
			{
			return KErrAlreadyExists;
			}
		}
	return err;	
	}
	
EXPORT_C TInt RMMFAudioPolicyProxy::CancelRequestResourceNotification(TUid aNotificationEventUid)
	{
	TUid eventType = KNullUid;
	TMMFAudioPolicyResourceNotificationSettingsPckg pckg;
	pckg().iNotificationUid = eventType;
	TInt err = SendReceiveResult(EMMFPolicyGetResourceNotificationEvent, pckg);
	eventType = pckg().iNotificationUid;
	if(!err)
		{
		if(eventType == aNotificationEventUid )
			{
			pckg().iNotificationUid = aNotificationEventUid;
			return SendReceive(EMMFPolicyCancelRequestResourceNotification, pckg);	
			}
		else
			{
			return KErrCancel;
			}
		}
	return err;
	}
	
EXPORT_C TInt RMMFAudioPolicyProxy::StopNotification()
	{
	return SendReceive(EMMFPolicyStopNotification);
	}

EXPORT_C TInt RMMFAudioPolicyProxy::IsRegisteredResourceNotification(TUid aEventType)
	{
	TUid eventType = KNullUid;
	TMMFAudioPolicyResourceNotificationSettingsPckg pckg;
	pckg().iNotificationUid = eventType;
	TInt err = SendReceiveResult(EMMFPolicyGetResourceNotificationEvent, pckg);
	if(err != KErrNone)
		{
		return err;
		}
	eventType = pckg().iNotificationUid;
	if(eventType == aEventType)
		{
		return KErrNone;
		}
	return KErrNotSupported;
	}

