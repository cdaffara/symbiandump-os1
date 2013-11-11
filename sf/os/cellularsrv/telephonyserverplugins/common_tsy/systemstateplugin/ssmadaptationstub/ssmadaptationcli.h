// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMADAPTATIONCLI_H__
#define __SSMADAPTATIONCLI_H__

/** @file
@internalComponent
@test
*/

#include <e32std.h>
#include <e32svr.h>

class RSsmEmergencyCallRfAdaptation
	{
	
public:
	IMPORT_C static void InitializeLibL();
    IMPORT_C static void UnInitializeLib();

	IMPORT_C TInt Connect();
	IMPORT_C TInt Connect(TInt aAsyncMessageSlotCount);
	IMPORT_C void ActivateRfForEmergencyCall(TRequestStatus& aStatus);
	IMPORT_C void DeactivateRfForEmergencyCall(TRequestStatus& aStatus);
	IMPORT_C void Close();
	IMPORT_C void Cancel();	
	
private:
	TRequestStatus iRequestStatus;
	};

	
#endif	// __SSMSUSCLI_H__
