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
// Opening phone state
// 
//

/**
 @file 
 @internalComponent
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tpdpstateopeningphoneTraces.h"
#endif

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "cpdpfsmfactory.h"
#include "cpdpfsm.h"
#include "PDPFSM.h"

TInt TPdpStateOpeningPhone::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	// ETel notifications
	case PdpFsm::EPhoneOpened:
		aFsm.ChangeStateToCreatingPrimary();
		iPdpFsmFactory->iEtelOpened = ETrue;		
		EtelDriverInput(aFsm, EtelDriver::ECreate1ryPdpContext);
		return KErrNone;
	case PdpFsm::EPhoneOpenedFailed:
		aFsm.ChangeStateToInitialised();
		iPdpFsmFactory->iEtelOpened = EFalse;		
		SpudManNotify (aFsm, KPrimaryContextCreated, aErrorCode);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATEOPENINGPHONE_INPUT_1, "*** PHONE OPEN FAILED ***");
		return KErrNone;
  case SpudMan::ECancelContextCreate:
		aFsm.ChangeStateToStopping();
		EtelDriverCancel(aFsm);
		EtelDriverInput(aFsm, EtelDriver::EContextDelete);
		return KErrNone;
	}
	
	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}
