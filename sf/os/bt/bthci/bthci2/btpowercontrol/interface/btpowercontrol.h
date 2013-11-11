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
 @publishedPartner
 @released
*/

#ifndef BTPOWERCONTROL_H
#define BTPOWERCONTROL_H

#include <e32base.h>
#include <bluetooth/hci/hcitypes.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <bluetooth/btpowercontrolmessages.h>
#endif

class RHCIServerSession;

NONSHARABLE_CLASS(RBTPowerControl)
/** API used for managing the power state of the controller

*/
	{
public:
	IMPORT_C RBTPowerControl();

	IMPORT_C TInt Open();
	IMPORT_C void Close();

	IMPORT_C void SetPower(TBTPowerState aState, TDes8* aDesc, TRequestStatus &aStatus);
	IMPORT_C TInt GetPower(TBTPowerState& aState, TDes8* aDesc);
	IMPORT_C void Cancel();
	
private:
	RHCIServerSession* iHCIServerSession;
	TUint32 iReserved; // Padding for possible future "per-copy" state.
	};

_LIT(KBTPowerControlPanic, "BTPwrCtrlClient");
enum TBTPowerControlPanic
	{
	EApiUsedWhenHandleIsNotOpen = 0,
	EHandleIsAlreadyOpen = 1,
	};

#endif // BTPOWERCONTROL_H
