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
// Configuration test step used for deleting devices from the device neighbourhood
//

#if (!defined __CT_DELETE_BTGPSDEVICELIST_STEP_H__)
#define __CT_DELETE_BTGPSDEVICELIST_STEP_H__

//Includes
#include "ctbtgpsdevicelisthandlerbase.h"


/** Class used to delete delete the BT GPS devices from the BT GPS 
neighbourhood.  It also deletes devices in the BT notifier list.
 */
class CT_DeleteBTGPSDeviceList : public CT_BTGPSDeviceListHandler
	{
public:
	CT_DeleteBTGPSDeviceList();
	~CT_DeleteBTGPSDeviceList();
	virtual TVerdict doTestStepL();
	
	};

_LIT(KDeleteDeviceConfigList,"DeleteBTGPSDeviceList");

#endif	//__CT_DELETE_BTGPSDEVICELIST_STEP_H__
