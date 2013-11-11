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

inline SymbianAvctp::TPid CAvctpSap::Pid() const
/**
Get the PID for this SAP

  @internalComponent
  @return The SAP's PID
*/
	{
	return iPid;
	}

inline TInt CAvctpSap::Channel() const
/**
Get the channel for this SAP

  @internalComponent
  @return The SAP's channel
*/
	{
	return iChannel;
	}

inline TBool CAvctpSap::IsInList() const
	{
	return iIsInList;
	}

inline void CAvctpSap::StartSendAsyncCallBack()
/**
Start the asynchronous callback to continue processing later

  @internalComponent
*/
	{
	iSendAsyncCallBack->CallBack();
	}

inline void CAvctpSap::CancelSendAsyncCallBack()
/**
Cancel the asynchronous callback

  @internalComponent
*/
	{
	iSendAsyncCallBack->Cancel();
	}

