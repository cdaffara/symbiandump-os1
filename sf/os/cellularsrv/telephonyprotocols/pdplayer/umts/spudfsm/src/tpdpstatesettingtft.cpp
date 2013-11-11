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
// Setting TFT state
// 
//

/**
 @file 
 @internalComponent
*/

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tpdpstatesettingtftTraces.h"
#endif

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "cpdpfsm.h"

TInt TPdpStateSettingTFT::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case PdpFsm::ETftSet:
		if (aFsm.iContextType != SpudMan::EMbms)
			{
			aFsm.ChangeStateToCreatedSecondary();
			}
		else
			{
			RPacketContext::TContextStatus 		contextStatus;
			aFsm.Get(contextStatus);
			if (contextStatus == RPacketContext::EStatusActive)
				{
				aFsm.ChangeStateToOpen();
				}
			else
				{
				aFsm.ChangeStateToCreatedMbms();
				}
			}
		SpudManNotify(aFsm, KContextTFTModifiedEvent, KErrNone);
		return KErrNone;
	case PdpFsm::ETftSetFailed:
		if (aFsm.iContextType != SpudMan::EMbms)
			{
			aFsm.ChangeStateToCreatedSecondary();
			}
		else
			{
			aFsm.ChangeStateToCreatedMbms();
			}
		SpudManNotify(aFsm, KContextTFTModifiedEvent, aErrorCode);
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, TPDPSTATESETTINGTFT_INPUT_1, "*** TFT SET FAILED ***");
		return KErrNone;
	}
	
	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}
