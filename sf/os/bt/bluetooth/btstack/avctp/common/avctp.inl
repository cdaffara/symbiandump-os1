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
Get the Muxer state factory

  @internalComponent
  @return The muxer state machine
*/
inline CAvctpMuxerStateFactory& CAvctpProtocol::MuxerStateFactory() const
	{
	return *iMuxerStateFactory;
	}

/**
Start the asynchronous callback to continue processing later

  @internalComponent
*/
inline void CAvctpProtocol::StartSendIpidAsyncCallBack()
	{
	iIpidResponseAsyncCallBack->CallBack();
	}

/**
Cancel the asynchronous callback

  @internalComponent
*/
inline void CAvctpProtocol::CancelIpidAsyncCallBack()
	{
	iIpidResponseAsyncCallBack->Cancel();	
	}
	
