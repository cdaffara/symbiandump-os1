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

#ifndef __LBSOOMTEST_H__
#define __LBSOOMTEST_H__



//****************************************************************
// Includes
//****************************************************************
// System
#include <e32property.h>


//****************************************************************
// Classes
//****************************************************************
/** 
Get the property key value to decide whether to fail the memory alloc from now on
code should be wrapped inside debug marco

this bus is for Out-Of-Memory test, defined by test code and subscribed 
by components (NetGateway, NRH, AGPSManager) which should still work 
under OOM condition
OOM is simulated by kicking off the __UHEAP_XX in the debug code of the 
affected components, which subscribe to the OOM bus. 
And it is signalled by test code through the bus
@internalComponent
@released
*/
class ROomRequest
    {
public:
    IMPORT_C static void InitializeL();
    IMPORT_C static void ShutDownL();
	IMPORT_C ROomRequest();
    IMPORT_C void OpenL();

    IMPORT_C TInt OomTestUpdates(TBool aOomTestOn);
		                                     
	IMPORT_C void OomTestNotifiyUpdates(TRequestStatus& aStatus);
	IMPORT_C TBool GetOomTestNotifiyUpdates();
	
    // Cancel the subscription to Oom Test bus
    IMPORT_C void CancelToReceiveOomSignal();

    IMPORT_C void Close();
			                                               
private:
	// signal property
    RProperty iOomTestSignal;
    
private:
	static void SetCategoryUidL(TUint32 aKey, TUid aCategoryUid);
	static void GetCategoryUidL(TUint32 aKey, TUid& aCategoryUid);
    };

#endif //__LBSOOMTEST_H__
