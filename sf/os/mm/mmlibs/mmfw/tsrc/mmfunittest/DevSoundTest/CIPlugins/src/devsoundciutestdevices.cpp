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

#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include "devsoundciutestdevices.hrh"
#include "aacdecoderconfigtestdevice.h"
#include "g729encoderconfigtestdevice.h"
#include "g729decoderconfigtestdevice.h"
#include "audiovibracontroltestdevice.h"
#include "audiobufferprefilltestdevice.h"
#include "audiodspcontroltestdevice.h"
#include "speechencoderconfigtestdevice.h"
#include "g711decoderconfigtestdevice.h"
#include "g711encoderconfigtestdevice.h"
#include "ilbcdecoderconfigtestdevice.h"
#include "ilbcencoderconfigtestdevice.h"
#include "errorconcealmenttestdevice.h"
#include "gsmconfigtestdevice.h"
#include "underflowautostopcontroltestdevice.h"
#include "playbackstatustestdevice.h"
#include "sbcencodertestdevice.h"
#include "eaacplusdecodertestdevice.h"
#include "setdrmprotectedtestdevice.h"
#include "advancedaacencodesettingstestdevice.h"
#include "bufferframesconfigtestdevice.h"
#include "gettimestampstestdevice.h"
#include "playbackstatustestdevice.h"
#include "sbcencodertestdevice.h"
#include "eaacplusdecodertestdevice.h"
#include "setdrmprotectedtestdevice.h"
#include "advancedaacencodesettingstestdevice.h"
#include "bufferframesconfigtestdevice.h"
#include "gettimestampstestdevice.h"
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
#include "MMFDevSoundCIMuxDeMuxPluginImp.h"
#endif

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidAacDecoderConfigTestDevice,	CAacDecoderConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidG729EncoderConfigTestDevice,	CG729EncoderConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidG729DecoderConfigTestDevice,	CG729DecoderConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidAudioVibraControlTestDevice,	CAudioVibraControlTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidSampleBufferingTestDevice,	CSampleBufferingTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidAudioDspControlTestDevice,	CAudioDspControlTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidSpeechEncoderConfigTestDevice,	CSpeechEncoderConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidG711DecoderConfigTestDevice,		CG711DecoderConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidG711EncoderConfigTestDevice,		CG711EncoderConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidIlbcDecoderIntfcTestDevice,		CIlbcDecoderConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidIlbcEncoderIntfcTestDevice,		CIlbcEncoderConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidErrorConcealmentIntfcTestDevice,	CErrorConcealmentTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidGsmConfigTestDevice,				CGsmConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidUnderflowAutoStopControlTestDevice,	CUnderflowAutoStopControlTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidPlayBackStatusTestDevice, CPlaybackStatusTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidSbcEncoderTestDevice, CSbcEncoderTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidEAacPlusDecoderTestDevice, CEAacPlusDecoderTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidSetDrmProtectedTestDevice, CSetDRMProtectedTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidAdvancedAacEncodeSettingsTestDevice, CAdvancedAacEncodeSettingsTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidBufferFramesConfigTestDevice, CBufferFramesConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidGetTimestampsTestDevice, CGetTimestampsTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidPlayBackStatusTestDevice, CPlaybackStatusTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidSbcEncoderTestDevice, CSbcEncoderTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidEAacPlusDecoderTestDevice, CEAacPlusDecoderTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidSetDrmProtectedTestDevice, CSetDRMProtectedTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidAdvancedAacEncodeSettingsTestDevice, CAdvancedAacEncodeSettingsTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidBufferFramesConfigTestDevice, CBufferFramesConfigTestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidGetTimestampsTestDevice, CGetTimestampsTestDevice::NewL),
#ifdef SYMBIAN_MULTIMEDIA_A3FDEVSOUND
	IMPLEMENTATION_PROXY_ENTRY(KUidMmfDevSoundCustomInterfaceMuxPlugin,	CMMFDevSoundCIMuxPluginImp::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KUidMmfDevSoundCustomInterfaceDeMuxPlugin,	CMMFDevSoundCIDemuxPluginImp::NewL)
#endif	
	};

//
// ImplementationGroupProxy
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

#ifndef EKA2
/** DLL Entry point */
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return(KErrNone);
	}
#endif
