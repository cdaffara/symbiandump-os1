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

#if !defined (__CMNPANICCODES_H__)
#define __CMNPANICCODES_H__

#include <e32std.h>

_LIT(KPanicSsmCmn, "Ssm-Cmn");

enum TCmnPanicCodes
	{
	//CSsmPropertyMonitor errors
	ECmnPropMonError1 = 1,
	
	//CSsmStateMonitor errors
	ECmnStateMonError1,
	
	//CSsmSystemWideProperty errors
	ECmnSwpSession1,
	
	//SsmStateAwareSession errors
	ECmnErrState,
	ECmnErrRqstStateNotif,
	ECmnErrRqstStateNotifCancel,
	ECmnErrAcknldgStateNotif,
	ECmnErrAcknldgRqstStateNotif,
	ECmnErrStateMon,
	
	//TSsmState errors
	ECmnStateMaxValue1,		//Out of range value supplied to the main state in TSsmState::TSsmState()
	ECmnStateMaxValue2,		//Out of range value supplied to the main state in TSsmState::TSsmState()
	ECmnStateMaxValue3,		//Out of range value supplied to the main state in TSsmState::Set()
	ECmnStateMaxValue4		//Out of range value supplied to the main state in TSsmState::SetFromInt()
	};

#endif // __CMNPANICCODES_H__
