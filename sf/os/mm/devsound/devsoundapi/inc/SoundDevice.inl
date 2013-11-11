
// SoundDevice.inl

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

#ifndef SOUNDDEVICE_INL
#define SOUNDDEVICE_INL

#include <mmf/server/mmfdevsoundcustominterfacesupport.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/server/mmfdevsoundcustominterfaceobserver.h>
#include <mmf/server/mmfdevsoundcustominterfacesupportclasses.h>
#endif

#include <mmf/common/mmfstandardcustomcommands.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfstandardcustomcommandsenums.h>
#include <mmf/common/mmfstandardcustomcommandsimpl.h>
#endif


inline TInt CMMFDevSound::RegisterAsClient(TUid aEventType, const TDesC8& aNotificationRegistrationData)
	{
	MAutoPauseResumeSupport* support = 
				static_cast<MAutoPauseResumeSupport*>(CustomInterface(KMmfUidDevSoundAudioResourceCustomInterface));
	if (support==NULL)
		{
		return KErrNotSupported;
		}
	if(aEventType != KMMFEventCategoryAudioResourceAvailable)
		{
		return KErrNotSupported;
		}
	else
		{ 
		return support->RegisterAsClient(aEventType, aNotificationRegistrationData);	
		}
	}

inline TInt CMMFDevSound::CancelRegisterAsClient(TUid aEventType)
	{
	MAutoPauseResumeSupport* support = 
			static_cast<MAutoPauseResumeSupport*>(CustomInterface(KMmfUidDevSoundAudioResourceCustomInterface));
	if (support==NULL)
		{
		return KErrNotSupported;
		}
	if(aEventType != KMMFEventCategoryAudioResourceAvailable)
		{
		return KErrNotSupported;
		}
	else
		{
		return support->CancelRegisterAsClient(aEventType);
		}
	}

inline TInt CMMFDevSound::GetResourceNotificationData(TUid aEventType,TDes8& aNotificationData)
	{
	MAutoPauseResumeSupport* support = 
			static_cast<MAutoPauseResumeSupport*>(CustomInterface(KMmfUidDevSoundAudioResourceCustomInterface));
	if (support==NULL)
		{
		return KErrNotSupported;
		}
	if(aEventType != KMMFEventCategoryAudioResourceAvailable)
		{
		return KErrNotSupported;
		}
	else
		{
		return support->GetResourceNotificationData(aEventType, aNotificationData);
		}
	}

inline TInt CMMFDevSound::WillResumePlay()
	{
	MAutoPauseResumeSupport* support = 
			static_cast<MAutoPauseResumeSupport*>(CustomInterface(KMmfUidDevSoundAudioResourceCustomInterface));
	if (support==NULL)
		{
		return KErrNotSupported;
		}
	return support->WillResumePlay();
	}

inline TInt CMMFDevSound::EmptyBuffers()
	{
	MMMFDevSoundEmptyBuffers* emptybuffers = 
			static_cast<MMMFDevSoundEmptyBuffers*>(CustomInterface(KMmfUidDevSoundEmptyBuffersCustomInterface));
	if (emptybuffers == NULL)
		{
		return KErrNotSupported;
		}
	return emptybuffers->EmptyBuffers();
	}

inline TInt CMMFDevSound::CancelInitialize()
	{
	MMMFDevSoundCancelInitialize* cancelInitialize = 
			static_cast<MMMFDevSoundCancelInitialize*>(CustomInterface(KMmfUidDevSoundCancelInitializeCustomInterface));
	if (cancelInitialize == NULL)
		{
		return KErrNotSupported;
		}
	return cancelInitialize->CancelInitialize();
	}

/*
 *  -doxygen comments in header-
 */
inline TInt CMMFDevSound::SetClientThreadInfo(TThreadId aTid)
	{
	MAudioClientThreadInfo* threadInfo = 
			static_cast<MAudioClientThreadInfo*>(CustomInterface(KMmfUidDevSoundAudioClientThreadInfoCustomInterface));
	if (threadInfo)
		{
		return threadInfo->SetClientThreadInfo(aTid);
		}
	else
		{
		return KErrNotSupported;
		}
	}

inline TInt CMMFDevSound::GetTimePlayed(TTimeIntervalMicroSeconds& aTime)
	{
	MMMFDevSoundTimePlayed* timePlayed = 
			static_cast<MMMFDevSoundTimePlayed*>(CustomInterface(KMmfUidDevSoundTimePlayedCustomInterface));
	if (timePlayed == NULL)
		{
		return KErrNotSupported;
		}
	return timePlayed->GetTimePlayed(aTime);
	}

inline TBool CMMFDevSound::IsGetTimePlayedSupported()
	{
	MMMFDevSoundTimePlayed* timePlayed = 
			static_cast<MMMFDevSoundTimePlayed*>(CustomInterface(KMmfUidDevSoundTimePlayedCustomInterface));
	if (timePlayed == NULL)
		{
		return EFalse;
		}
	return ETrue;
	}

inline TBool CMMFDevSound::QueryIgnoresUnderflow()
	{
	MMMFDevSoundQueryIgnoresUnderflow* ignoresUnderflow = 
			static_cast<MMMFDevSoundQueryIgnoresUnderflow*>(CustomInterface(KMmfUidDevSoundQueryIgnoresUnderflowCustomInterface));
	if (ignoresUnderflow == NULL)
		{
		return EFalse;
		}
	return ignoresUnderflow->QueryIgnoresUnderflow();
	}

inline TBool CMMFDevSound::IsResumeSupported()
	{
	MMMFDevSoundTruePause* truePause = 
			static_cast<MMMFDevSoundTruePause*>(CustomInterface(KMmfUidDevSoundTruePauseCustomInterface));
	if (truePause == NULL)
		{
		return EFalse;
		}
	return truePause->IsResumeSupported();
	}

inline TInt CMMFDevSound::Resume()
	{
	MMMFDevSoundTruePause* truePause = 
			static_cast<MMMFDevSoundTruePause*>(CustomInterface(KMmfUidDevSoundTruePauseCustomInterface));
	if (truePause == NULL || EFalse == truePause->IsResumeSupported() )
		{
		return KErrNotSupported;
		}
	return truePause->Resume();
	}

#endif // SOUNDDEVICE_INL
