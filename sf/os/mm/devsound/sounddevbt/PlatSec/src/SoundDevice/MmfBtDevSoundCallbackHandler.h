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


#ifndef __MMFDEVSOUNDCALLBACKHANDLER_H
#define __MMFDEVSOUNDCALLBACKHANDLER_H

#include <e32base.h>
#include <e32std.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mmf/server/sounddevice.h>
#include "MmfBtDevSoundProxy.h"


class CMMFBufferToBeFilledHandler : public CActive
/**
*@internalTechnology
*/
	{
public:
	static CMMFBufferToBeFilledHandler* NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	~CMMFBufferToBeFilledHandler();
	void ReceiveEvents();
	void CancelReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	CMMFBufferToBeFilledHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	void ConstructL();
private:
	enum {EGranularity=8};
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver& iDevSoundObserver;
	TRequestStatus	iRequestStatus;
	TMMFDevSoundProxyHwBufPckg iSetPckg;
	};


class CMMFBufferToBeEmptiedHandler : public CActive
/**
*@internalTechnology
*/
	{
public:
	static CMMFBufferToBeEmptiedHandler* NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	CMMFBufferToBeEmptiedHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	~CMMFBufferToBeEmptiedHandler();
	void ReceiveEvents();
	void CancelReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
public:
	CMMFDataBuffer* iBuffer;
private:
	void ConstructL();
private:
	enum {EGranularity=8};
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver& iDevSoundObserver;
	TRequestStatus	iRequestStatus;
	TMMFDevSoundProxyHwBufPckg iSetPckg;
	};


class CMMFPlayErrorHandler : public CActive
/**
*@internalTechnology
*/
	{
public:
	static CMMFPlayErrorHandler* NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	CMMFPlayErrorHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	~CMMFPlayErrorHandler();
	void ReceiveEvents();
	void CancelReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	void ConstructL();
private:
	enum {EGranularity=8};
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver& iDevSoundObserver;
	TRequestStatus	iRequestStatus;
	TMMFDevSoundProxyHwBufPckg iSetPckg;
	};

class CMMFRecordErrorHandler : public CActive
/**
*@internalTechnology
*/
	{
public:
	static CMMFRecordErrorHandler* NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	CMMFRecordErrorHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	~CMMFRecordErrorHandler();
	void ReceiveEvents();
	void CancelReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	void ConstructL();
private:
	enum {EGranularity=8};
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver& iDevSoundObserver;
	TRequestStatus	iRequestStatus;
	TMMFDevSoundProxyHwBufPckg iSetPckg;
	};

class CMMFToneFinishedHandler : public CActive
/**
*@internalTechnology
*/
	{
public:
	static CMMFToneFinishedHandler* NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	CMMFToneFinishedHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	~CMMFToneFinishedHandler();
	void ReceiveEvents();
	void CancelReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	void ConstructL();
private:
	enum {EGranularity=8};
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver& iDevSoundObserver;
	TRequestStatus	iRequestStatus;
	TMMFDevSoundProxyHwBufPckg iSetPckg;
	};

class CMMFInitializeCompleteHandler : public CActive
/**
*@internalTechnology
*/
	{
public:
	static CMMFInitializeCompleteHandler* NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	CMMFInitializeCompleteHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	~CMMFInitializeCompleteHandler();
	void ReceiveEvents();
	void CancelReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	void ConstructL();
private:
	enum {EGranularity=8};
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver& iDevSoundObserver;
	TRequestStatus	iRequestStatus;
	TMMFDevSoundProxyHwBufPckg iSetPckg;
	};

class CMMFSendEventToClientHandler : public CActive
/**
*@internalTechnology
*/
	{
public:
	static CMMFSendEventToClientHandler* NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	CMMFSendEventToClientHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver);
	~CMMFSendEventToClientHandler();
	void ReceiveEvents();
	void CancelReceiveEvents();
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	void ConstructL();
private:
	enum {EGranularity=8};
	RMMFDevSoundProxy* iDevSoundProxy;
	MDevSoundObserver& iDevSoundObserver;
	TRequestStatus	iRequestStatus;
	TMMFEventPckg iEventPckg;
	};

#endif
