// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <bluetooth/btpowercontrol.h>
#include <bluetooth/hciserverclient.h>
#include <bluetooth/hci/hciipc.h>
#include <bluetooth/logger.h>
#include <bluetooth/btpowercontrolmessages.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BTPOWERCONTROL);
#endif


EXPORT_C RBTPowerControl::RBTPowerControl()
	: iHCIServerSession(NULL)
	{
	}

/**
Opens the handle to the power control interface for the Bluetooth hardware.

@panic KBTPowerControlPanic EHandleIsAlreadyOpen If the RBTPowerControl instance has already been opened (and not subsequently closed).
*/
EXPORT_C TInt RBTPowerControl::Open()
	{
	__ASSERT_ALWAYS(!iHCIServerSession, PANIC(KBTPowerControlPanic, EHandleIsAlreadyOpen));
	TInt err = KErrNoMemory;
	iHCIServerSession = new RHCIServerSession;
	if(iHCIServerSession)
		{
		err = iHCIServerSession->Open(KHCIPowerControlManagerUid);
		if(err != KErrNone)
			{
			delete iHCIServerSession;
			iHCIServerSession = NULL;
			}
		}
	return err;
	}

/**
Closes the handle to the Bluetooth power control interface.

Note that it is safe to call this multiple times.
*/
EXPORT_C void RBTPowerControl::Close()
	{
	if(iHCIServerSession)
		{
		iHCIServerSession->Close();
		delete iHCIServerSession;
		iHCIServerSession = NULL;
		}
	}

/**
Makes an asynchronous request to set the power state of the Bluetooth hardware.

@param aState The desired state.
@param aInfo 
@param aStatus The TRequestStatus that will signal the result of the request when completed.

@panic KBTPowerControlPanic EApiUsedWhenHandleIsNotOpen If called when the RBTPowerControl instance has not been opened.
*/
EXPORT_C void RBTPowerControl::SetPower(TBTPowerState aState, TDes8* aInfo, TRequestStatus &aStatus)
	{
	__ASSERT_ALWAYS(iHCIServerSession, PANIC(KBTPowerControlPanic, EApiUsedWhenHandleIsNotOpen));
	TInt state = static_cast<TInt>(aState);
	iHCIServerSession->SendAsync(EHCISetPower, aInfo, NULL, state, aStatus);
	}

/**
Makes a synchronous request to get the current power state of the Bluetooth hardware.

@param aState Set (on return) to the current power state if request was successful.
@param aInfo
@return KErrNone if the request was successful, otherwise one of the system-wide error codes.

@panic KBTPowerControlPanic EApiUsedWhenHandleIsNotOpen If called when the RBTPowerControl instance has not been opened.
*/
EXPORT_C TInt RBTPowerControl::GetPower(TBTPowerState& aState, TDes8* aInfo)
	{
	__ASSERT_ALWAYS(iHCIServerSession, PANIC(KBTPowerControlPanic, EApiUsedWhenHandleIsNotOpen));
	TPckgBuf<TBTPowerState> pckg;
	TInt err = iHCIServerSession->SendSync(EHCIGetPower, aInfo, &pckg, 0);
	if(err==KErrNone)
		{
		aState = pckg();
		}
	return err;
	}

/**
Cancels asynchronous requests made through this handle to the power control interface.

@panic KBTPowerControlPanic EApiUsedWhenHandleIsNotOpen If called when the RBTPowerControl instance has not been opened.
*/
EXPORT_C void RBTPowerControl::Cancel()
	{
	__ASSERT_ALWAYS(iHCIServerSession, PANIC(KBTPowerControlPanic, EApiUsedWhenHandleIsNotOpen));
	// Cast away the return value - we can only do our best here, if there is an error (which shouldn't originate
	// from the HCI Server at least) we just ignore it.
	static_cast<void>(iHCIServerSession->SendSync(KHCICancelPower, NULL, NULL, 0));
	}
