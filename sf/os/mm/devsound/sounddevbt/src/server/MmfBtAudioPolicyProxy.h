// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFBTAUDIOPOLICYPROXY_H__
#define __MMFBTAUDIOPOLICYPROXY_H__

#include <e32base.h>
#include <e32std.h>
#include "MmfBtAudioPolicySession.h"  
#include "mdasound.h"

#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
class RMMFAudioPolicyProxy; // declared here.

class CMMFAudioPolicyRequest;
class TMMFDevSoundInfo;

NONSHARABLE_CLASS( RMMFAudioPolicyProxy ): public RMmfSessionBase
/**
*@internalTechnology
*/
	{
public:
	IMPORT_C TInt Open(RServer2& aPolicyServerHandle);
	IMPORT_C TInt SetDevSoundInfo(TMMFDevSoundInfo& aDevSoundInfo);
	IMPORT_C void MakeRequest(TMMFAudioPolicyPrioritySettings& aPrioritySettings);
	IMPORT_C TInt UpdateState(TMMFAudioPolicyPrioritySettings& aPrioritySettings);
	IMPORT_C void ReceiveEvents(TMMFAudioPolicyEventPckg& aEventPckg, TRequestStatus& aStatus);
	IMPORT_C TInt CancelReceiveEvents();
	IMPORT_C TInt GetPlayFormatsSupported(RMdaDevSound::TSoundFormatsSupportedBuf& aPlayFormatsSupported);
	IMPORT_C TInt GetRecordFormatsSupported(RMdaDevSound::TSoundFormatsSupportedBuf& aRecordFormatsSupported);
	IMPORT_C TInt GetPlayFormat(RMdaDevSound::TCurrentSoundFormatBuf& aPlayFormat);
	IMPORT_C TInt GetRecordFormat(RMdaDevSound::TCurrentSoundFormatBuf& aRecordFormat);
	IMPORT_C TInt LaunchRequests();
	IMPORT_C TInt RequestResourceNotification(TUid aNotificationEventUid, const TDesC8& aNotificationData = KNullDesC8);
	IMPORT_C TInt CancelRequestResourceNotification(TUid aNotificationEventUid);
	IMPORT_C TInt StopNotification();
	IMPORT_C TInt IsRegisteredResourceNotification(TUid aEventType);
private:
	TMMFAudioPolicyPrioritySettingsPckg iPsPckg;
	};

#endif
