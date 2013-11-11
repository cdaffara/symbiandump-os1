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
// Configuration test step used for checking the BT GPS Device List
//

#if (!defined __CT_CHECKBTGPSDEVICELIST_STEP_H__)
#define __CT_CHECKBTGPSDEVICELIST_STEP_H__

//Includes
#include "ctbtgpsdevicelisthandlerbase.h"


/** Class used to check the BT GPS device list.  It reads in
an ini file that is past into it from the test script and uses
this information to create a temp device list. It then checks
to see whether this temp list is the same as the actual device
list.  This is used after the test is run to see if it went OK.
 */
class CT_CheckBTGPSDeviceList : public CT_BTGPSDeviceListHandler
	{
public:
	CT_CheckBTGPSDeviceList();
	~CT_CheckBTGPSDeviceList();
	virtual TVerdict doTestStepL();
	
	};

_LIT(KCheckDeviceConfigList,"CheckBTGPSDeviceList");

#endif	//__CT_CHECKBTGPSDEVICELIST_STEP_H__
