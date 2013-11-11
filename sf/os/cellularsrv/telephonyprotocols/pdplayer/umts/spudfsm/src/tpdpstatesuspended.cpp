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
// Suspended state
// 
//

/**
 @file 
 @internalComponent
*/

#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "PDPFSM.h"
#include "cpdpfsm.h"

TInt TPdpStateSuspended::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case SpudMan::EResume:
		aFsm.ChangeStateToOpen();
		SpudManNotify(aFsm, KContextUnblockedEvent, KErrNone);
		return KErrNone;
	case PdpFsm::EContextStatusChangeNetwork:
		if (aFsm.iContextStatus == RPacketContext::EStatusActive)
		{
			aFsm.ChangeStateToOpen();
			SpudManNotify(aFsm, KContextUnblockedEvent, KErrNone); 
			//SpudManNotify (aFsm, KNetworkStatusEvent, KErrNone);
			return KErrNone;
		}
		break;		
	}
	
	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}
