// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// State after secondary has been created, but not activated
// 
//

/**
 @file 
 @internalComponent
*/

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "pdpfsmnmspace.h"
#include "PDPFSM.h"
#include "cpdpfsm.h"

TInt TPdpStateCreatedSecondary::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case SpudMan::EContextActivate: 
		aFsm.ChangeStateToActivatingSecondary();
		EtelDriverInput(aFsm, EtelDriver::EActivatePdp);
		return KErrNone;		
	case SpudMan::EContextQoSSet:
		aFsm.ChangeStateToSettingQoS();
		EtelDriverInput(aFsm, EtelDriver::ESetQoS);
		return KErrNone;
	case SpudMan::EContextTFTModify:
		aFsm.ChangeStateToSettingTFT();
		EtelDriverInput(aFsm, EtelDriver::ESetTft);
		return KErrNone;
	}
	
	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}


