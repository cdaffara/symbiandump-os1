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

#ifndef __A2DPBTHEADSETAUDIOIFSERVER_H__
#define __A2DPBTHEADSETAUDIOIFSERVER_H__

#include <e32base.h>
#include <mmf/common/mmfipc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfipcserver.h>
#endif
#include "A2dpBTHeadsetAudioIfClientServer.h"

class CA2dpBTHeadsetAudioInterface; // fwd declaration
class TFourCC;
class TRange;

/**
 *	CA2dpBTHeadsetAudioIfServer definition
 */
class CA2dpBTHeadsetAudioIfServer : public CMmfIpcServer
	{
public:
	IMPORT_C static TInt StartThread(TAny* aParms);	// thread entry point
	
	static CA2dpBTHeadsetAudioIfServer* NewL();
	~CA2dpBTHeadsetAudioIfServer();
	CMmfIpcSession* NewSessionL(const TVersion &aVersion) const;
private:
	CA2dpBTHeadsetAudioIfServer();
	void ConstructL();
private:
	CA2dpBTHeadsetAudioInterface* iBTAudioInterface;
	};

/**
 *	CA2dpBTHeadsetIfEventHandler
 */
class CA2dpBTHeadsetIfEventHandler : public CActive
	{
public:
	static CA2dpBTHeadsetIfEventHandler* NewL();
	~CA2dpBTHeadsetIfEventHandler();
	void StartL(const RMmfIpcMessage& aMessage);
	void Stop();
private:
	CA2dpBTHeadsetIfEventHandler();
	void RunL();
	void DoCancel();
	void ConstructL();
private:
	RMmfIpcMessage* iMessage;	
	};
	
/**
 *	CA2dpBTHeadsetAudioIfSession definition
 */
class CA2dpBTHeadsetAudioIfSession : public CMmfIpcSession
	{
public:
	static CA2dpBTHeadsetAudioIfSession* NewL(CA2dpBTHeadsetAudioInterface* aBTAudioInterface);
 	~CA2dpBTHeadsetAudioIfSession();
	void ServiceL(const RMessage2& aMessage);
	
private:
	CA2dpBTHeadsetAudioIfSession(CA2dpBTHeadsetAudioInterface* aBTAudioInterface);
	void ConstructL();
	
	TBool DoInitializeL(const RMmfIpcMessage& aMessage);
	TBool DoCancelInitializeL(const RMmfIpcMessage& aMessage);
	TBool DoCopyFourCCArrayDataL(const RMmfIpcMessage& aMessage);
	TBool DoCopyUintArrayDataL(const RMmfIpcMessage& aMessage, RArray<TUint> aArray);
	TBool DoCopyChannelsArrayDataL(const RMmfIpcMessage& aMessage);
	TInt  DoGetSupportedDataTypesL(const RMmfIpcMessage& aMessage);// returns error value
	TInt  DoGetSupportedSampleRatesL(const RMmfIpcMessage& aMessage);
	TBool DoGetSupportedSampleRatesDiscreteL(const RMmfIpcMessage& aMessage);
	TBool DoGetSupportedSampleRatesRangeL(const RMmfIpcMessage& aMessage);
	TInt  DoGetSupportedChannelsL(const RMmfIpcMessage& aMessage);
	TBool DoGetStereoSupportL(const RMmfIpcMessage& aMessage);
	TInt DoSetDataTypeL(const RMmfIpcMessage& aMessage);	// returns error value
	TInt DoSetSampleRateL(const RMmfIpcMessage& aMessage);// returns error value
	TInt DoSetChannelsL(const RMmfIpcMessage& aMessage);// returns error value
	TBool DoOpenDeviceL(const RMmfIpcMessage& aMessage);
	TBool DoCancelOpenDevice(const RMmfIpcMessage& aMessage);
	TBool DoCloseDeviceL(const RMmfIpcMessage& aMessage);
	TBool DoVolumeL(const RMmfIpcMessage& aMessage);
	TInt DoSetVolumeL(const RMmfIpcMessage& aMessage);// returns error value
	TBool DoPlayDataL(const RMmfIpcMessage& aMessage);
	TBool DoCancelPlayDataL(const RMmfIpcMessage& aMessage);
	TBool DoFlushBufferL(const RMmfIpcMessage& aMessage);
	TBool DoBytesPlayedL(const RMmfIpcMessage& aMessage);
	TBool DoResetBytesPlayedL(const RMmfIpcMessage& aMessage);
	TBool DoPauseBufferL(const RMmfIpcMessage& aMessage);
	TBool DoResumePlayingL(const RMmfIpcMessage& aMessage);
	TBool DoNotifyErrorL(const RMmfIpcMessage& aMessage);
	TBool DoCancelNotifyErrorL(const RMmfIpcMessage& aMessage);

private:
	CA2dpBTHeadsetAudioInterface* iBTAudioInterface;	// Not owned
	// Hmm - one handler per asynch call and then no need to store messages?
	CA2dpBTHeadsetIfEventHandler* iInitHandler;	// AO to manage asynch calls to the interface
	CA2dpBTHeadsetIfEventHandler* iOpenDeviceHandler;	// AO to manage asynch calls to the interface
	CA2dpBTHeadsetIfEventHandler* iCloseDeviceHandler;	// AO to manage asynch calls to the interface
	CA2dpBTHeadsetIfEventHandler* iPlayDataHandler;	// AO to manage asynch calls to the interface
	CA2dpBTHeadsetIfEventHandler* iNotifyErrorHandler;	// AO to manage asynch calls to the interface
	
	RArray<TFourCC> iFourCCArray;
	RArray<TUint> iDiscreteArray;
	RArray<TRange> iRangeArray;
	RArray<TUint> iStereoSupportArray;
	};
#endif
