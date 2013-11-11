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
// Initialised state
// 
//

/**
 @file 
 @internalComponent
*/
 
#include <networking/umtsnifcontrolif.h>
#include "tpdpstates.h"
#include "pdpfsmnmspace.h"
#include "cpdpfsm.h"
#include "cpdpfsmfactory.h"
#include "eteldrivernmspace.h"
using namespace SpudMan;

TInt TPdpStateInitialised::Input (CPdpFsm& aFsm, const TInt aOperation, const TInt aErrorCode)
{
	switch (aOperation)
	{
	case SpudMan::ECreatePrimaryPDPContext:
		aFsm.iContextType = SpudMan::EPrimary;

		// have to ensure EOpenPhone is called the first time a primary context is created
		if (iPdpFsmFactory->iEtelOpened == EFalse)
			{
			aFsm.ChangeStateToOpeningPhone();
			EtelDriverInput(aFsm, EtelDriver::EOpenPhone);
			}
		else // just create the primary
			{
			aFsm.ChangeStateToCreatingPrimary();
			EtelDriverInput (aFsm, EtelDriver::ECreate1ryPdpContext);
			}
		return KErrNone;
	case SpudMan::ECreateSecondaryPDPContext:
		aFsm.ChangeStateToCreatingSecondary();

		aFsm.iContextType = SpudMan::ESecondary;

		EtelDriverInput (aFsm, EtelDriver::ECreate2ryPdpContext);
		return KErrNone;
		
		
	case SpudMan::ECreateMbmsPDPContext:
		aFsm.ChangeStateToCreatingMbms();
		aFsm.iContextType = SpudMan::EMbms;
		EtelDriverInput (aFsm, EtelDriver::ECreateMbmsPdpContext);
		return KErrNone;
		
	// these stop the default actions which aren't of value in the Initialised state, although we 
	// do not expect to see these here.
	case PdpFsm::EContextDeleted:
		return KErrNone;
	case PdpFsm::EContextStatusChangeNetwork:
		if (aFsm.iContextStatus == RPacketContext::EStatusDeleted)
		{
		return KErrNone;
		}
		break;
	}

	// default error handling
	return TPdpState::Input(aFsm, aOperation, aErrorCode);

}


