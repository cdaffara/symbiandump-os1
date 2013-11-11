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

/**
 @file
 @internalTechnology
*/

#include <bluetooth/logger.h>
#include <bt_sock.h>

#include "avctpremotedevices.h"
#include "avctpbody.h"
#include "avctpserviceutils.h"
#include "avctpcommon.h"
#include "avctpPriorities.h"
#include "avctpserviceutils.h"
#include "channelcontrollers.h"

using namespace SymbianAvctp;

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP_SERVICES);
#endif

RNestableLock::RNestableLock()
	: iRefCount(0)
	, iThreadId(KInvalidThreadId)	// set to an invalid id
	{
	}

TInt RNestableLock::CreateLocal()
	{
	TInt err = iLock.CreateLocal();
	if(err == KErrNone)
		{
		err = iMetaLock.CreateLocal();
		}
	if(err != KErrNone)
		{
		Close();
		}
	return err;
	}

void RNestableLock::Close()
	{
	iLock.Close();
	iMetaLock.Close();
	iRefCount = 0;
	}

void RNestableLock::Wait()
	{
	iMetaLock.Wait();
	TThreadId currentThreadId = RThread().Id();
	if(iThreadId == TThreadId(KInvalidThreadId) || currentThreadId != iThreadId)
		{
		iMetaLock.Signal();
		iLock.Wait();
		iMetaLock.Wait();
		iThreadId = currentThreadId;
		}
	++iRefCount;
	iMetaLock.Signal();
	}

void RNestableLock::Signal()
	{
	iMetaLock.Wait();
	// Assert if current thread is stored current thread?
	if(--iRefCount == 0)
		{
		iLock.Signal();
		iThreadId = TThreadId(KInvalidThreadId);
		}
	iMetaLock.Signal();
	}

TAvctpRemoteDeviceInfo::TAvctpRemoteDeviceInfo(const TBTDevAddr& aAddr) :
	iAddr(aAddr),
	iHasSecondChannel(EFalse)
	{
	LOG_FUNC
	}

const TBTDevAddr& TAvctpRemoteDeviceInfo::RemoteAddress() const
	{
	LOG_FUNC
	return iAddr;
	}

TBool TAvctpRemoteDeviceInfo::HasSecondChannel() const
	{
	LOG_FUNC
	return iHasSecondChannel;
	}

void TAvctpRemoteDeviceInfo::SetHasSecondChannel(TBool aHasSecondChannel)
	{
	LOG_FUNC
	iHasSecondChannel = aHasSecondChannel;
	}

/**
Two phase constructor of CAvctpRemoteDevices which deals with connection and disconnection 
of a remote device. 
@param aNotify provides access to the MAvctpEventNotify
@param aSocketServ a connected comms session
@internalComponent
*/
CAvctpRemoteDevices* CAvctpRemoteDevices::NewL(MAvctpEventNotify& aNotify, RSocketServ& aSocketServ, SymbianAvctp::TPid aPid)
	{
	LOG_STATIC_FUNC

	CAvctpRemoteDevices* self = CAvctpRemoteDevices::NewLC(aNotify, aSocketServ, aPid);
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Two phase constructor of CAvctpRemoteDevices which deals with connection and disconnection 
of a remote device. 
This leaves newly created object on cleanupstack
@param aAvctpBody provides access to the MAvctpEventNotify
@param aSocketServ a connected comms session
@internalComponent
*/
CAvctpRemoteDevices* CAvctpRemoteDevices::NewLC(MAvctpEventNotify& aNotify, RSocketServ& aSocketServ, SymbianAvctp::TPid aPid)
	{
	LOG_STATIC_FUNC

	CAvctpRemoteDevices* self = new(ELeave) CAvctpRemoteDevices(aNotify, aSocketServ);
	CleanupStack::PushL(self);
	self->ConstructL(aPid);
	return self;	
	}

/**
c'tor
@internalComponent
*/
CAvctpRemoteDevices::CAvctpRemoteDevices(MAvctpEventNotify& aNotify, RSocketServ& aSocketServ):
 	iNotify(aNotify),
	iSocketServ(aSocketServ)
	{
	LOG_FUNC
	}
	

/** 
@internalComponent
*/
void CAvctpRemoteDevices::ConstructL(SymbianAvctp::TPid aPid)
	{
	LOG_FUNC

	User::LeaveIfError(iLock.CreateLocal()); 
	
	iPid = aPid;
	iState = EListening;
	
	iPrimaryChannelController = CPrimaryChannelController::NewL(*this, iSocketServ, aPid);	
	iPrimaryChannelController->Listen();
	}

	
/**
d'tor
@internalComponent
*/	
CAvctpRemoteDevices::~CAvctpRemoteDevices()
	{
	LOG_FUNC
	iLock.Wait();
	iLock.Close();

	iRemoteAddrs.Close();
	delete iSecondaryChannelController;
	delete iPrimaryChannelController;
	}

/**
Starts a connection to a remote device
@param aBTDevice the remote device to connect to
@internalComponent
*/
TInt CAvctpRemoteDevices::PrimaryChannelAttachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	
	iLock.Wait();

	TInt err = KErrRepeatConnectionAttempt;

	// Start the connection attempt
	
	if (iState == EListening)
		{
		if (!IsAttached(aBTDevice))
			{
			iState = ECreatingControlLink;
			iPrimaryChannelController->AttachRequest(aBTDevice);
			err = KErrNone;
			}
		}
	
	if (err != KErrNone)
		{
		iState = EListening;
		}
	
	iLock.Signal();
	return err;
	}

/**
Called after a Attach Indicate. If the connection hasn't been accepted it send back to the server
a refuse attach ioctl, and the client won't be notified through the controlling channel
@param aBTDevice the remote device to add
@param aClientsAccepts ETrue if the client has accepted the Attach Indication or
					   EFalse if the client has denied the Attach Indication
@param aChannel the controlling channel through the indication has been received
@internalComponent
*/
TInt CAvctpRemoteDevices::PropagateAttachRsp(const TBTDevAddr& aBTDevice, 
							  			      TBool aClientsAccepts,
							  			      TInt aChannel)
	{
	LOG_FUNC

	TInt err = KErrNone;

	if (aClientsAccepts) 
		{
		if (err == KErrNone && !IsAttached(aBTDevice) && aChannel == KAvctpPrimaryChannel)
			{
			//only add device on primary control channel connection
			TAvctpRemoteDeviceInfo info(aBTDevice);
			err = iRemoteAddrs.Append(info);
			}
		}
	else
		{
		__ASSERT_DEBUG(aChannel == KAvctpPrimaryChannel || aChannel == KAvctpSecondaryChannel, Panic(EAvctpInvalidChannel));
		
		CBaseChController* pointer = (aChannel == KAvctpPrimaryChannel) ? ((CBaseChController*)iPrimaryChannelController) : ((CBaseChController*)iSecondaryChannelController);
		
		__ASSERT_DEBUG(pointer, Panic(EAvctpNullControllerChannel));
		pointer->RefuseAttach(aBTDevice);	// won't receive a confirmation
		}	
	
	return err;
	}
	
/**
Explicitly disconnect from the give remote device.
@internalComponent
*/
TInt CAvctpRemoteDevices::PrimaryChannelDetachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	TInt err = KErrFirstChannelNotAttached;
	const TAvctpRemoteDeviceInfo* info = RemoteDeviceInfo(aBTDevice);
	if (info)
		{
		iPrimaryChannelController->DetachRequest(aBTDevice);
		err = KErrNone;
		}
	
	iLock.Signal();
	
	// else covered by ret
	return err;
	}

void CAvctpRemoteDevices::PrimaryChannelCancelAttach(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	if (!IsAttached(aBTDevice) && iState == ECreatingControlLink)
		{
		// the notifications are only propagated if the state is indicating an
		// outstanding request. So, simply changing the state to listening will avoid
		// the notification to be propagated. 
		// We also need to notify the stack that we are not interested so to have a consistent state
		// It is safe to call RefuseAttach that deal with this scenario server side.
		iPrimaryChannelController->RefuseAttach(aBTDevice);
		iState = EListening;	
		}
	
	iLock.Signal();
	}

TInt CAvctpRemoteDevices::SecondaryChannelAttachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	
	iLock.Wait();
	TInt err = KErrFirstChannelNotAttached;
	const TAvctpRemoteDeviceInfo* info = RemoteDeviceInfo(aBTDevice);
	if (info)
		{
		err = KErrRepeatConnectionAttempt;
		if (!info->HasSecondChannel())
			{
			__ASSERT_DEBUG(iSecondaryChannelNotify, Panic(EAvctpSecondChannelNotPresent));
			if (iState == EListening)
				{
				iState = ECreatingSecondLink;
				iSecondaryChannelController->AttachRequest(aBTDevice);
				err = KErrNone;
				}
			}
		}
	iLock.Signal();
	return err;
	}

TInt CAvctpRemoteDevices::SecondaryChannelDetachRequest(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	
	iLock.Wait();
	TInt err = KErrSecondChannelNotAttached;
	const TAvctpRemoteDeviceInfo* info = RemoteDeviceInfo(aBTDevice);
	if (info)
		{
		if (info->HasSecondChannel())
			{
			__ASSERT_DEBUG(iSecondaryChannelNotify, Panic(EAvctpSecondChannelNotPresent));
			iSecondaryChannelController->DetachRequest(aBTDevice);
			err = KErrNone;
			}
		}
	iLock.Signal();
	return err;
	}

void CAvctpRemoteDevices::SecondaryChannelCancelAttach(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	if (IsAttached(aBTDevice) && iState == ECreatingSecondLink)
		{
		// the notifications are only propagated if the state is indicating an
		// outstanding request. So, simply changing the state to listening will avoid
		// the notification to be propagated.
		iState = EListening;	
		}
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::RemoveRemoteDevice(const TBTDevAddr& aBTDevice)
	{
	LOG_FUNC
	
	__DEBUG_ONLY
		(
		TBuf<KBTAddressLength> address;
		aBTDevice.GetReadable(address);
		)
	
	LOG1(_L("BT Device 0x%S"), &address);

	TInt index;	  
	for(index= 0; index < iRemoteAddrs.Count(); index++)
		{
		if (iRemoteAddrs[index].RemoteAddress() == aBTDevice)
			{
			iRemoteAddrs.Remove(index);
			break; // since there should only be one remote device per TBTDevAddr
			}
		}
	}

const TAvctpRemoteDeviceInfo* CAvctpRemoteDevices::RemoteDeviceInfo(const TBTDevAddr& aBTDevice) const
	{
	LOG_FUNC

	const TAvctpRemoteDeviceInfo* deviceInfo = NULL;
	
	TInt index;
	for (index = 0; index < iRemoteAddrs.Count(); index++)
		{
		if (iRemoteAddrs[index].RemoteAddress() == aBTDevice)
			{
			deviceInfo = &iRemoteAddrs[index];
			break; // there should only be one remote device on each TBTDevAddr
			}
		}
	
	return deviceInfo;
	}

TBool CAvctpRemoteDevices::IsAttached(const TBTDevAddr& aBTDevice) const
	{
	LOG_FUNC
	
	iLock.Wait();
	
	TBool ans = RemoteDeviceInfo(aBTDevice) ? ETrue : EFalse;
	
#ifdef _DEBUG
//	// Check there is only one remote device on this TBTDevAddr in the array
	TInt numDevices = 0;
	TInt index;
	for (index = 0; index < iRemoteAddrs.Count(); index++)
		{
		if (iRemoteAddrs[index].RemoteAddress() == aBTDevice)
			{
			numDevices++;
			}
		}
	__ASSERT_DEBUG(((ans && numDevices == 1) || (!ans && numDevices == 0)), Panic(EAvctpRemoteAddressOccursMultipleTimes));
#endif // _DEBUG
	
	LOG1(_L("IsAttached() result is: %d"), ans)
	
	iLock.Signal();
	
	return ans;
	}

/**
All Errors to do with a remote device will result in a disconnect Ind happening
because all device errors are currently fatal. NB since the actual error doesn't
matter to the client they don't get informed of it.
*/
void CAvctpRemoteDevices::NotifyError(const TBTDevAddr& aBTDevice, TInt aError, TInt aChannel)
	{
	LOG_FUNC
	
	MAvctpEventNotify* notify = aChannel == KAvctpPrimaryChannel ? &iNotify : iSecondaryChannelNotify; 
	notify->MaenErrorNotify(aBTDevice, aError);	
	// don't do anything here as the client may have closed the RAvctp object after the NotifyError
	}

void CAvctpRemoteDevices::SetSecondaryChannelNotifyL(MAvctpEventNotify* aSecondChannelNotify)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	__ASSERT_DEBUG(!aSecondChannelNotify || !iSecondaryChannelNotify, Panic(EAvctpSecondaryChannelNotifyAlreadyAssigned));

	if(aSecondChannelNotify)
		{
		iSecondaryChannelNotify = aSecondChannelNotify;
		
		iSecondaryChannelController = CSecondaryChannelController::NewL(*this, iSocketServ, iPid);
		iSecondaryChannelController->Listen();
		}
	else
		{
		delete iSecondaryChannelController;
		iSecondaryChannelController = NULL;
		}
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::PrimaryChannelAttachConfirm(const TBTDevAddr& aAddr, TInt aMtu, TInt aError)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	// state must be:
	// ECreatingControlLink because we have requested to be attached.
	// EListening because we had asked to be attached but then we canceled the request.
	__ASSERT_DEBUG(iState == ECreatingControlLink || iState == EListening, Panic(EAvctpInvalidChannelState));
	
	if (iState == ECreatingControlLink)
		{
		iNotify.MaenAttachConfirm(aAddr, aMtu, aError);
		if (aError == KErrNone && !IsAttached(aAddr))
			{
			TAvctpRemoteDeviceInfo info(aAddr);
			aError = iRemoteAddrs.Append(info);
			}
		}
	
	iState = EListening;
	iPrimaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::PrimaryChannelAttachIndicate(const TBTDevAddr& aAddr, TInt aMtu)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	TBool clientAccepts = EFalse;
	iNotify.MaenAttachIndicate(aAddr, aMtu, clientAccepts);
	TInt err = PropagateAttachRsp(aAddr, clientAccepts, KAvctpPrimaryChannel);
	// we can have an error if the array append fails 
	if (err == KErrNone)
		{
		if (clientAccepts)
			{
			iPrimaryChannelController->AgreeAttachment(aAddr);
			// iSecondaryChannelController->AttachPassively(aAddr);
			}
		}
	else 
		{
		NotifyError(aAddr, err, KAvctpPrimaryChannel);
		}

	// whatever happen (even an error) we need to listen anyway.
	iState = EListening;
	iPrimaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::PrimaryChannelDetachConfirm(const TBTDevAddr& aAddr, TInt aError)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	if (aError == KErrNone)
		{
		const TAvctpRemoteDeviceInfo* info = RemoteDeviceInfo(aAddr);
		__ASSERT_DEBUG(info, Panic(EAvctpRemoteDeviceNotConnected));
		
		RemoveRemoteDevice(aAddr);
		iNotify.MaenDetachConfirm(aAddr, KErrNone);
		}
	else
		{
		NotifyError(aAddr, aError, KAvctpPrimaryChannel);
		}
	
	// whatever happens we need to listen
	iState = EListening;
	iPrimaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::PrimaryChannelDetachIndicate(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	// it can happen that we are in ECreatingSecondLink and the remote disconnects
	// the first channel.
	// server side we first notify the secondary channel, then the first one. 
	// But we are not guarantee that the secondary channel will be served first
	// so we need to save the oldstate to update after this process.
	// in this case, when we'll receive the secondary channel event we are in the 
	// correct state.
	// even if the state at the end won't be listening, we have to listen on the 
	// primary channel anyway.
	
	TState oldState = iState;
	
    const TAvctpRemoteDeviceInfo* info = RemoteDeviceInfo(aAddr);
    __ASSERT_DEBUG(info, Panic(EAvctpRemoteDeviceNotConnected));

    RemoveRemoteDevice(aAddr);
    iNotify.MaenDetachIndicate(aAddr);
	
	// we go to the previous state (before detaching the primary channel). see the comment above
	// for more info
	iState = oldState;
	// but we need to listen on the primary channel anyway.
	iPrimaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::PrimaryChannelIoctlError(const TBTDevAddr& aAddr, TInt aError)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	iNotify.MaenErrorNotify(aAddr, aError);
	
	// after having notified the error we must back to listen, despite the state we were before.
	iState = EListening;
	iPrimaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::PrimaryChannelAgreementError(const TBTDevAddr& aAddr, TInt __DEBUG_ONLY(aError))
	{
	LOG_FUNC
	
	iLock.Wait();
	
	const TAvctpRemoteDeviceInfo* info = RemoteDeviceInfo(aAddr);
	__ASSERT_DEBUG(info, Panic(EAvctpRemoteDeviceNotConnected));
	__ASSERT_DEBUG(!info->HasSecondChannel(), Panic(EAvctpSecondaryChannelUnexpected));
	__ASSERT_DEBUG(aError == KErrMuxerNotFound || aError == KErrNoMemory, Panic(EAvctpUnexpectedErrorCode));
	
	RemoveRemoteDevice(aAddr);
	iNotify.MaenDetachIndicate(aAddr);
	iState = EListening;
	iPrimaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::SecondaryChannelAttachConfirm(const TBTDevAddr& aAddr, TInt aMtu, TInt aError)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	// state must be:
	// ECreatingControlLink because we have requested to be attached.
	// EListening because we had asked to be attached but then we canceled the request.
	__ASSERT_DEBUG(iState == ECreatingSecondLink || iState == EListening, Panic(EAvctpInvalidChannelState));
		
	if (iState == ECreatingSecondLink)
		{
		iSecondaryChannelNotify->MaenAttachConfirm(aAddr, aMtu, aError);
		if (aError == KErrNone)
			{
			TAvctpRemoteDeviceInfo* info = const_cast<TAvctpRemoteDeviceInfo*>(RemoteDeviceInfo(aAddr));
			__ASSERT_DEBUG(info, Panic(EAvctpRemoteDeviceNotConnected));
			info->SetHasSecondChannel(ETrue);
			}
		else
			{
			if (aError == KErrMuxerShutDown)
				{
				RemoveRemoteDevice(aAddr);
				}
			}
		}
	iState = EListening;
	iSecondaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::SecondaryChannelAttachIndicate(const TBTDevAddr& aAddr, TInt aMtu)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	TAvctpRemoteDeviceInfo* info = const_cast<TAvctpRemoteDeviceInfo*>(RemoteDeviceInfo(aAddr));

	// if info is NULL it probably means that the first channel attach indication was refused or
	// the attach request was cancelled. However, in that case, we do nothing.
	if (info)
		{
		TBool clientAccepts = EFalse;
		if(iSecondaryChannelNotify)
			{
			iSecondaryChannelNotify->MaenAttachIndicate(aAddr, aMtu, clientAccepts);
	#ifdef _DEBUG		
			TInt err = 
	#endif
			PropagateAttachRsp(aAddr, clientAccepts, KAvctpSecondaryChannel);	
			
			// err must always be KErrNone when PropagateConnectRsp is called on the secondary channel
			__ASSERT_DEBUG(err == KErrNone, Panic(EAvctpUnexpectedErrorCode));
			
			if (clientAccepts)
				{
				iSecondaryChannelController->AgreeAttachment(aAddr);
				info->SetHasSecondChannel(ETrue);
				}
			}
		else // No-one's interested
			{
			iSecondaryChannelController->RefuseAttach(aAddr);
			}
		}
	iState = EListening;
	iSecondaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::SecondaryChannelDetachConfirm(const TBTDevAddr& aAddr, TInt aError)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	if (aError == KErrNone)
		{
		__ASSERT_DEBUG(iSecondaryChannelNotify, Panic(EAvctpSecondChannelNotPresent));
		
		TAvctpRemoteDeviceInfo* info = const_cast<TAvctpRemoteDeviceInfo*>(RemoteDeviceInfo(aAddr));
		__ASSERT_DEBUG(info, Panic(EAvctpRemoteDeviceNotConnected));
		__ASSERT_DEBUG(info->HasSecondChannel(), Panic(EAvctpSecondChannelNotPresent));
		iSecondaryChannelNotify->MaenDetachConfirm(aAddr, KErrNone);
		info->SetHasSecondChannel(EFalse);
		}
	else
		{
		NotifyError(aAddr, aError, KAvctpSecondaryChannel);
		}

	iState = EListening;
	iSecondaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::SecondaryChannelDetachIndicate(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	// In the case where we haven't got a secondary channel notify we may still get
	// a detach indicate if the refusal for the attach indicate has not been 
	// processed before the remote disconnects.  
	if(iSecondaryChannelNotify)
		{
		iSecondaryChannelNotify->MaenDetachIndicate(aAddr);
		TAvctpRemoteDeviceInfo* info = const_cast<TAvctpRemoteDeviceInfo*>(RemoteDeviceInfo(aAddr));
        if (info && info->HasSecondChannel())
            {
            info->SetHasSecondChannel(EFalse);
            }
		}
#ifdef _DEBUG
	else
		{
		// in this case we should not have any remote device info - check
		TAvctpRemoteDeviceInfo* info = const_cast<TAvctpRemoteDeviceInfo*>(RemoteDeviceInfo(aAddr));
		__ASSERT_ALWAYS(!info || !info->HasSecondChannel(), Panic(EDetachIndicateForSecondChannelWithNoConsumer));
		}
#endif
	
	iState = EListening;
	iSecondaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::SecondaryChannelIoctlError(const TBTDevAddr& aAddr, TInt aError)
	{
	LOG_FUNC
	
	iLock.Wait();
	
	__ASSERT_DEBUG(iSecondaryChannelNotify, Panic(EAvctpSecondChannelNotPresent));
	iSecondaryChannelNotify->MaenErrorNotify(aAddr, aError);
	// after having notified the error we must back to listen, despite the state we were before.
	iState = EListening;
	iSecondaryChannelController->Listen();
	
	iLock.Signal();
	}

void CAvctpRemoteDevices::SecondaryChannelAgreementError(const TBTDevAddr& aAddr, TInt __DEBUG_ONLY(aError))
	{
	LOG_FUNC
	
	iLock.Wait();
	
	TAvctpRemoteDeviceInfo* info = const_cast<TAvctpRemoteDeviceInfo*>(RemoteDeviceInfo(aAddr));
	__ASSERT_DEBUG(info, Panic(EAvctpRemoteDeviceNotConnected));
	__ASSERT_DEBUG(info->HasSecondChannel(), Panic(EAvctpSecondaryChannelUnexpected));
	__ASSERT_DEBUG(aError == KErrMuxerNotFound, Panic(EAvctpUnexpectedErrorCode));
	
	RemoveRemoteDevice(aAddr);
	iSecondaryChannelNotify->MaenDetachIndicate(aAddr);
	iState = EListening;
	iSecondaryChannelController->Listen();
	
	iLock.Signal();
	}
