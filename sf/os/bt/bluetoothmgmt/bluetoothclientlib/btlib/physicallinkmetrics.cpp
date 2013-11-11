// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bt_sock.h>
#include "physicallinkmetrichelpers.h"

_LIT(KPhysLinkMetricFaultCat, "BTPhysLinkMetric");
enum TPhysLinkMetricFault
	{
	EInvalidMetric,
	};

/**
Creates a new CBluetoothPhysicalLinkMetrics object from a connected socket
@param aObserver The object which will receive asynchronous events
@param aSockServ A handle to an existing session on the socket server (ESock).
@param 	aConnectedSocket An open connected socket (ESock subsession) on that existing ESock session 
*/
EXPORT_C CBluetoothPhysicalLinkMetrics* CBluetoothPhysicalLinkMetrics::NewL(
					MBluetoothPhysicalLinkMetricsObserver& aObserver,
					RSocketServ& aSockServ,
					RSocket& aConnectedSocket)
	{
	CBluetoothPhysicalLinkMetrics* self = new(ELeave) CBluetoothPhysicalLinkMetrics(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSockServ, aConnectedSocket);
	CleanupStack::Pop(self);
	return self;
	}

/**
Creates a new CBluetoothPhysicalLinkMetrics object from a connected socket
@param aObserver The object which will receive asynchronous events
@param aSockServ A handle to an existing session on the socket server (ESock).
@param 	aAddr The Bluetooth address of a remote device with which there is an existing connection 
*/
EXPORT_C CBluetoothPhysicalLinkMetrics* CBluetoothPhysicalLinkMetrics::NewL(
					MBluetoothPhysicalLinkMetricsObserver& aObserver,
					RSocketServ& aSockServ,
					const TBTDevAddr& aAddr)
	{
	CBluetoothPhysicalLinkMetrics* self = new(ELeave) CBluetoothPhysicalLinkMetrics(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aSockServ, aAddr);
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor
*/
EXPORT_C CBluetoothPhysicalLinkMetrics::~CBluetoothPhysicalLinkMetrics()
	{
	delete iRssiBaseband;
	delete iLinkQualityBaseband;
	delete iFailedContactCounterBaseband;
	delete iTransmitPowerLevelBaseband;
	}

/**
Subscribes for notification of changes to the RSSI (Received Signal Strengh Indication)
of the link.
MBluetoothPhysicalLinkMetricsObserver::MbplmoRssiChanged will be called with the
initial value, then subsequently every time the RSSI changes, subject to the polling
interval specified in the stack.
The stack will continue to run a timer to poll the hardware until the physical link is disconnected
or CBluetoothPhysicalLinkMetrics::Cancel is called
*/
EXPORT_C void CBluetoothPhysicalLinkMetrics::SubscribeRssi()
	{
	iRssiBaseband->SubscribePhysicalLinkMetric(KLMReadRssiIoctl);
	}

/**
Subscribes for notification of changes to the Link Quality.
MBluetoothPhysicalLinkMetricsObserver::MbplmoLinkQualityChanged will be called with the
initial value, then subsequently every time the link quality changes, subject to the 
polling interval specified in the stack.
The stack will continue to run a timer to poll the hardware until the physical link is disconnected
or CBluetoothPhysicalLinkMetrics::Cancel is called
*/
EXPORT_C void CBluetoothPhysicalLinkMetrics::SubscribeLinkQuality()
	{
	iLinkQualityBaseband->SubscribePhysicalLinkMetric(KLMReadLinkQualityIoctl);
	}

/**
Subscribes for notification of changes to the Failed Contact counter of the link.
MBluetoothPhysicalLinkMetricsObserver::MbplmoFailedContactCounterChanged will be called
with the initial value, then subsequently every time the failed contact counter	changes,
subject to the polling interval specified in the stack.
The stack will continue to run a timer to poll the hardware until the physical link is disconnected
or CBluetoothPhysicalLinkMetrics::Cancel is called
*/	
EXPORT_C void CBluetoothPhysicalLinkMetrics::SubscribeFailedContactCounter()
	{
	iFailedContactCounterBaseband->SubscribePhysicalLinkMetric(KLMReadFailedContactCounterIoctl);
	}

/**
Subscribes for notification of changes to the Transmit Power Level of the link.
MBluetoothPhysicalLinkMetricsObserver::MbplmoTransmitPowerLevelChanged will be called
with the initial value, then subsequently every time the transmit power level changes,
subject to the polling interval specified in the stack.
The stack will continue to run a timer to poll the hardware until the physical link is disconnected
or CBluetoothPhysicalLinkMetrics::Cancel is called
*/
EXPORT_C void CBluetoothPhysicalLinkMetrics::SubscribeTransmitPowerLevel()
	{
	iTransmitPowerLevelBaseband->SubscribePhysicalLinkMetric(KLMReadCurrentTransmitPowerLevelIoctl);
	}

/**
Cancels subscriptions to all physical link notifications.
*/
EXPORT_C void CBluetoothPhysicalLinkMetrics::Cancel()
	{
	iRssiBaseband->Cancel();
	iLinkQualityBaseband->Cancel();
	iTransmitPowerLevelBaseband->Cancel();
	iFailedContactCounterBaseband->Cancel();
	}

void CBluetoothPhysicalLinkMetrics::RssiChanged(TInt8 aRssi)
	{
	iObserver.MbplmoRssiChanged(aRssi);
	}

void CBluetoothPhysicalLinkMetrics::LinkQualityChanged(TUint8 aLinkQuality)
	{
	iObserver.MbplmoLinkQualityChanged(aLinkQuality);
	}

void CBluetoothPhysicalLinkMetrics::FailedContactCounterChanged(TUint16 aFailedContactCounter)
	{
	iObserver.MbplmoFailedContactCounterChanged(aFailedContactCounter);
	}

void CBluetoothPhysicalLinkMetrics::TransmitPowerLevelChanged(TInt8 aTransmitPowerLevel)
	{
	iObserver.MbplmoTransmitPowerLevelChanged(aTransmitPowerLevel);
	}

void CBluetoothPhysicalLinkMetrics::SubscribeError(TInt aError)
	{
	Cancel();
	iObserver.MbplmoError(aError);
	}

CBluetoothPhysicalLinkMetrics::CBluetoothPhysicalLinkMetrics(MBluetoothPhysicalLinkMetricsObserver& aObserver):
	iObserver(aObserver)
	{
	
	}
template <class T>
void CBluetoothPhysicalLinkMetrics::ConstructL(RSocketServ& aSockServ, T& aLinkIdentifier)
	{
	iRssiBaseband = CBluetoothPhysicalLinkMetricSubscriber::NewL(*this, aSockServ, aLinkIdentifier);
	iLinkQualityBaseband = CBluetoothPhysicalLinkMetricSubscriber::NewL(*this, aSockServ, aLinkIdentifier);
	iFailedContactCounterBaseband = CBluetoothPhysicalLinkMetricSubscriber::NewL(*this, aSockServ, aLinkIdentifier);
	iTransmitPowerLevelBaseband = CBluetoothPhysicalLinkMetricSubscriber::NewL(*this, aSockServ, aLinkIdentifier);
	}

CBluetoothPhysicalLinkMetricSubscriber* CBluetoothPhysicalLinkMetricSubscriber::NewL(
							CBluetoothPhysicalLinkMetrics& aParent,
							RSocketServ& aSockServ,
							RSocket& aConnectedSocket)
	{
	CBluetoothPhysicalLinkMetricSubscriber* self = new(ELeave) CBluetoothPhysicalLinkMetricSubscriber(aParent);
	CleanupStack::PushL(self);
	self->ConstructL(aSockServ, aConnectedSocket);
	CleanupStack::Pop(self);
	return self;

	}

CBluetoothPhysicalLinkMetricSubscriber* CBluetoothPhysicalLinkMetricSubscriber::NewL(
								CBluetoothPhysicalLinkMetrics& aParent,
								RSocketServ& aSockServ,
								const TBTDevAddr& aAddr)
	{
	CBluetoothPhysicalLinkMetricSubscriber* self = new(ELeave) CBluetoothPhysicalLinkMetricSubscriber(aParent);
	CleanupStack::PushL(self);
	self->ConstructL(aSockServ, aAddr);
	CleanupStack::Pop(self);
	return self;
	}

CBluetoothPhysicalLinkMetricSubscriber::~CBluetoothPhysicalLinkMetricSubscriber()
	{
	Cancel();
	iBaseband.Close();
	}

void CBluetoothPhysicalLinkMetricSubscriber::SubscribePhysicalLinkMetric(TBTLMIoctls aIoctl)
	{
	if (!IsActive())
		{
		iPhysicalLinkMetric = aIoctl;
		iValue() = KMaxTInt;
		// KMaxTInt is not a valid value that will be returned by the controller, so this is used
		// as a value to get the initial value from the stack.
		iBaseband.ReadNewPhysicalLinkMetricValue(iStatus, iValue, iPhysicalLinkMetric);
		SetActive();
		}
	}


CBluetoothPhysicalLinkMetricSubscriber::CBluetoothPhysicalLinkMetricSubscriber(CBluetoothPhysicalLinkMetrics& aParent):
	CActive(CActive::EPriorityStandard), iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}

void CBluetoothPhysicalLinkMetricSubscriber::ConstructL(RSocketServ& aSockServ,	RSocket& aConnectedSocket)
	{
	User::LeaveIfError(iBaseband.Open(aSockServ, aConnectedSocket));
	}

void CBluetoothPhysicalLinkMetricSubscriber::ConstructL(RSocketServ& aSockServ,	const TBTDevAddr& aAddr)
	{
	User::LeaveIfError(iBaseband.Open(aSockServ, const_cast<TBTDevAddr&>(aAddr)));
	}

void CBluetoothPhysicalLinkMetricSubscriber::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	iBaseband.ReadNewPhysicalLinkMetricValue(iStatus, iValue, iPhysicalLinkMetric);
	SetActive();
	switch (iPhysicalLinkMetric)
		{
		case KLMReadRssiIoctl:
			iParent.RssiChanged(iValue());
			break;
		case KLMReadCurrentTransmitPowerLevelIoctl:
			iParent.TransmitPowerLevelChanged(iValue());
			break;
		case KLMReadLinkQualityIoctl:
			iParent.LinkQualityChanged(iValue());
			break;
		case KLMReadFailedContactCounterIoctl:
			iParent.FailedContactCounterChanged(iValue());
			break;
		default:
			__ASSERT_ALWAYS(EFalse,User::Panic(KPhysLinkMetricFaultCat, EInvalidMetric));
		}
	}

TInt CBluetoothPhysicalLinkMetricSubscriber::RunError(TInt aError)
	{
	iParent.SubscribeError(aError);
	return KErrNone;
	}

void CBluetoothPhysicalLinkMetricSubscriber::DoCancel()
	{
	iBaseband.CancelPhysicalLinkMetricUpdate();
	}

EXPORT_C TAny* MBluetoothPhysicalLinkMetricsObserver::MbplmoExtensionInterfaceL(TUid /* aInterface */)
	{
	User::Leave(KErrExtensionNotSupported);
	return NULL;
	}
