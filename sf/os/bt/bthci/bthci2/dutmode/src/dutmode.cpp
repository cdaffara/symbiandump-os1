// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/dutmode.h>

#include <bluetooth/hciserverclient.h>
#include <bluetooth/hci/hciipc.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_DUT_MODE);
#endif

EXPORT_C RBluetoothDutMode::RBluetoothDutMode()
	: iHCIServerSession(NULL)
	{
	}

/**
Opens the handle to the Bluetooth DUT Mode Service

@return KErrNone if successful, otherwise a standard Symbian error code
*/
EXPORT_C TInt RBluetoothDutMode::Open()
	{
	LOG_FUNC
	__ASSERT_ALWAYS(!iHCIServerSession, PANIC(KBluetoothDutModePanic, EHandleIsAlreadyOpen));
	
	TInt err = KErrNoMemory;
	iHCIServerSession = new RHCIServerSession;
	if(iHCIServerSession)
		{
		err = iHCIServerSession->Open(KHCIBluetoothDutModeManagerUid);
		if(err != KErrNone)
			{
			delete iHCIServerSession;
			iHCIServerSession = NULL;
			}
		}
	return err;
	}

/**
Closes the handle to the Bluetooth DUT Mode Service

Note that it is safe to call this multiple times.
*/
EXPORT_C void RBluetoothDutMode::Close()
	{
	LOG_FUNC
	if(iHCIServerSession)
		{
		iHCIServerSession->Close();
		delete iHCIServerSession;
		iHCIServerSession = NULL;
		}
	}

/**
Activate Device Under Test mode

@return KErrNone if successful, otherwise a standard Symbian error code
*/
EXPORT_C TInt RBluetoothDutMode::ActivateDutMode() const
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iHCIServerSession, PANIC(KBluetoothDutModePanic, EApiUsedWhenHandleIsNotOpen));
	return iHCIServerSession->SendSync(EHCIActivateDutMode);
	}


/**
Deactivate Device Under Test mode

@return KErrNone if successful, otherwise a standard Symbian error code
*/
EXPORT_C TInt RBluetoothDutMode::DeactivateDutMode() const
	{
	LOG_FUNC
	__ASSERT_ALWAYS(iHCIServerSession, PANIC(KBluetoothDutModePanic, EApiUsedWhenHandleIsNotOpen));
	return iHCIServerSession->SendSync(EHCIDeactivateDutMode);
	}
