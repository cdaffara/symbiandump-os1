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
// Configuration test step used for setting up the BT GPS Device List
//

#if (!defined __CT_SETBTGPSDEVICELIST_STEP_H__)
#define __CT_SETBTGPSDEVICELIST_STEP_H__

//Includes
#include "ctbtgpsdevicelisthandlerbase.h"


/** Class used to configure the BT GPS device list.  It reads in
an ini file that is past into it from the test script and uses
this information to enter the BT GPS devices into the BT GPS
configuration API device list.
 */
class CT_SetupBTGPSDeviceList : public CT_BTGPSDeviceListHandler
	{
public:
	CT_SetupBTGPSDeviceList();
	~CT_SetupBTGPSDeviceList();
	virtual TVerdict doTestStepL();
	
	};

_LIT(KSetupDeviceConfigList,"SetupBTGPSDeviceList");

#endif	//__CT_SETBTGPSDEVICELIST_STEP_H__
