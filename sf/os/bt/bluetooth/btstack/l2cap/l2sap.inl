// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "l2util.h"

inline CL2CapSAPSignalHandler& CL2CAPConnectionSAP::SignalHandler() const
	{
	return *iL2CapSAPSignalHandler;
	}

inline CL2CapSDUQueue& CL2CAPConnectionSAP::DataQueue() const
	{
	return *iL2CapDataQueue;
	}

inline MSocketNotify* CL2CAPConnectionSAP::Socket() const
	{
	return iSocket;
	}

inline CL2CAPProtocol& CL2CAPConnectionSAP::Protocol() const
	{
	return iProtocol;
	}

inline TBool CL2CAPConnectionSAP::IsSDUQueueClosed() const
	{
	return (iL2CapDataQueue == NULL);
	}

inline CL2CAPConnectionSAP* CL2CAPConnectionSAP::ListeningSAP() const
	{
	return iListeningSAP;
	}

inline void CL2CAPConnectionSAP::SetListeningSAP(CL2CAPConnectionSAP& aSAP)
	{
	__ASSERT_DEBUG(!iListeningSAP, Panic(EL2CAPAttemptToAsignCloneToSecondListeningSAP));
	iListeningSAP = &aSAP;
	}
	
inline void CL2CAPConnectionSAP::ClearListeningSAP()
	{
	iListeningSAP = NULL;
	}

inline void CL2CAPConnectionSAP::StartNewDataAsyncCallBack()
	{
	iNewDataAsyncCallBack->CallBack();
	}

inline CL2CAPConnectionSAP::TShutdownType CL2CAPConnectionSAP::ShutdownReceived() const
	{
	return iShutdownReceived;
	}

inline TUint8 CL2CAPConnectionSAP::ChannelPriority() const
	{
	return iChannelPriority;
	}

inline TInt CL2CAPConnectionSAP::SocketErrorCode() const
	{
	return iSocketErrorCode;
	}
	
inline TUint CL2CAPConnectionSAP::SocketErrorAction() const	
	{
	return iSocketErrorAction;
	}

inline void CL2CAPConnectionSAP::SetSocketErrorCode(TInt aError)
	{
	iSocketErrorCode = aError;
	}
	
inline void CL2CAPConnectionSAP::SetSocketErrorAction(MSocketNotify::TOperationBitmasks aAction)
	{
	iSocketErrorAction = aAction;
	}
	
inline TBool CL2CAPConnectionSAP::IsAcceptPending() const
	{
	return iAcceptPending;
	}	
	

