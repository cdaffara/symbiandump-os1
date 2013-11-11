/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#ifndef __ZEROCOPY_LOOPBACK_DRIVER_DEV_H__
#define __ZEROCOPY_LOOPBACK_DRIVER_DEV_H__


#include <comms-infras/commsbufskern.h>

enum { KLoopbackMTU = 1600, KLoopbackQueueLen = 32 };

/**
  Physical Device (factory class) for 'zerocopy loopback'
*/
class DZeroCopyLoopbackPddFactory : public DPhysicalDevice
	{
public:
	DZeroCopyLoopbackPddFactory();
	~DZeroCopyLoopbackPddFactory();

	// Inherited from DPhysicalDevice
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DBase*& aChannel, TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);
	virtual TInt Validate(TInt aUnit, const TDesC8* aInfo, const TVersion& aVer);

public:
	TInt iHardwareInUse;

private:
	enum TMinimumLDDVersion
		{
		EMinimumLddMajorVersion=1,
		EMinimumLddMinorVersion=0,
		EMinimumLddBuild=0 //Not used
		};

public:
	TDynamicDfcQue* iDfcQ;
	};

/**
  Logical Device (factory class) for 'zerocopy loopback'
*/
class DZeroCopyLoopbackFactory : public DLogicalDevice
	{
public:
	DZeroCopyLoopbackFactory();
	~DZeroCopyLoopbackFactory();

	//	Inherited from DLogicalDevice
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	};

class DZeroCopyLoopback;


static const TInt KMaxLddProbePoints = 16;

/**
  Logical Channel class for 'zerocopy loopback'
*/
class DZeroCopyLoopbackChannel : public DLogicalChannel
	{
public:
	DZeroCopyLoopbackChannel();
	virtual ~DZeroCopyLoopbackChannel();
	//	Inherited from DObject
	virtual TInt RequestUserHandle(DThread* aThread, TOwnerType aType);
	// Inherited from DLogicalChannelBase
	virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
	// Inherited from DLogicalChannel
	virtual void HandleMsg(TMessageBase* aMsg);

	// Interface methods for use by PDD
	virtual void SendDataComplete(TInt aResult);
	virtual void ReceiveDataComplete(TInt aResult);
	DCommsPond& Pond() {return *iPond;}

private:
	// Panic reasons
	enum TPanic
		{
		ERequestAlreadyPending = 1
		};

	// Implementation for the differnt kinds of messages sent through RBusLogicalChannel
	TInt DoControl(TInt aFunction, TAny* a1, TAny* a2);
	TInt DoRequest(TInt aReqNo, TRequestStatus* aStatus, TAny* a1, TAny* a2);
	void DoCancel(TUint aMask);

	// Accessor for the PDD
	inline DZeroCopyLoopback* Pdd();

	// Methods for configuration
	TInt GetConfig(TDes8* aConfigBuf);
	TInt SetConfig(const TDesC8* aConfigBuf);
	void CurrentConfig(RZeroCopyLoopbackDriver::TConfig& aConfig);
	TInt LoadPond(TPondTransferBuf* aPond);
	TInt UnloadPond();

	// Methods for processing a SendData request
	TInt SendData(TRequestStatus* aStatus, TInt aShBufHandle);
	void SendDataCancel();
	void DoSendDataComplete();
	static void SendDataDfc(TAny* aPtr);

	// Methods for processing a ReceiveData request
	TInt ReceiveData(TRequestStatus* aStatus, TInt* aBufferHandle);
	void ReceiveDataCancel();
	void DoReceiveDataComplete();
	static void ReceiveDataDfc(TAny* aPtr);

private:

	DThread* iClient;

	// Members used for processing a SendData request
	TRequestStatus* iSendDataStatus;
	TDfc iSendDataDfc;
	TInt iSendDataResult;

	// Members used for processing a ReceiveData request
	TInt* iReceiveDataDescriptor;
	TRequestStatus* iReceiveDataStatus;
	TDfc iReceiveDataDfc;
	TInt iReceiveDataResult;
	TInt iBufferLength;
	DCommsPond* iPond;
	TInt iDebugProbes[KMaxLddProbePoints];
	};

inline DZeroCopyLoopback* DZeroCopyLoopbackChannel::Pdd()
	{ return (DZeroCopyLoopback*)iPdd; }

/**
  Interface to 'zerocopy loopback' physical device
*/
class DZeroCopyLoopback : public DBase
	{
public:
	/**
	Structure for holding PDD capabilities information
	*/
	class TCaps
		{
	public:
		TVersion iVersion;
		};
public:
	virtual TInt BufferSize() const =0;
	virtual TInt Speed() const =0;
	virtual TInt SetSpeed(TInt aSpeed) =0;
	virtual DCommsBuf* SendBuffer() = 0;
	virtual DCommsBuf* ReceiveBuffer() = 0;
	virtual TInt RequestDataSend() =0;
	virtual void SendDataCancel() =0;
	virtual TInt RequestDataReceipt() =0;
	virtual void ReceiveDataCancel() =0;
	virtual TBool ReceivedQueueLen() =0;
	virtual void AdvanceReceiveQueue() = 0;

public:
	DZeroCopyLoopbackChannel* iLdd;
	};

#endif

