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
// Open state
// 
//

/**
 @file 
 @internalComponent
*/

#include <etelpckt.h>
#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "PDPFSM.h"
#include "cpdpfsm.h"

TInt TPdpStateOpen::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case PdpFsm::EQoSProfileChangeNetwork:
//		RPacketQoS::TQoSR99_R4Negotiated
		SpudManNotify (aFsm, KContextParametersChangeEvent, KErrNone);
		return KErrNone;
	case PdpFsm::EConfigGPRSChangeNetwork:
//		//RPacketContext::TContextConfigGPRS
		SpudManNotify (aFsm, KContextParametersChangeEvent, KErrNone);
		return KErrNone;
	case PdpFsm::EContextStatusChangeNetwork:
		if (aFsm.iContextStatus == RPacketContext::EStatusSuspended)
    		{
			aFsm.ChangeStateToSuspended();
			SpudManNotify(aFsm, KContextBlockedEvent, KErrNone); 
    		}
		else if (aFsm.iContextStatus == RPacketContext::EStatusDeleted)
    		{
			aFsm.ChangeStateToClosing();
			EtelDriverInput(aFsm, EtelDriver::EContextDelete);
    		}
		else
    		{
			// no change in state - the upper layer must change the state if it needs to
			SpudManNotify (aFsm, KContextParametersChangeEvent, aErrorCode);
    		}
		return KErrNone;
	case SpudMan::EContextDelete:
		aFsm.ChangeStateToClosing();
		EtelDriverInput(aFsm, EtelDriver::EContextDelete);
		return KErrNone;
	case SpudMan::EContextQoSSet:
		aFsm.ChangeStateToChangingQoS();
		EtelDriverInput(aFsm, EtelDriver::ESetQoS);
		return KErrNone;
	case SpudMan::EContextModifyActive:
		aFsm.ChangeStateToModifingActive();
		EtelDriverInput(aFsm, EtelDriver::EModifyActive);
		return KErrNone;
	case SpudMan::EContextTFTModify:
		aFsm.ChangeStateToChangingTFT();
		EtelDriverInput(aFsm, EtelDriver::EChangeTft);
		return KErrNone;
	case SpudMan::ESuspend: // think this is superfluous - DAN will see at review :)
		aFsm.ChangeStateToSuspended();
		SpudManNotify(aFsm, KContextBlockedEvent, KErrNone);
		return KErrNone;
	case SpudMan::EMbmsSessionUpdate:
		EtelDriverInput(aFsm, EtelDriver::ESessionUpdate);
		return KErrNone;
		
	case SpudMan::EMbmsParameterUpdate:
		SpudManNotify (aFsm, KContextParametersChangeEvent, aErrorCode);
		return KErrNone;
	}
	
	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);
}


