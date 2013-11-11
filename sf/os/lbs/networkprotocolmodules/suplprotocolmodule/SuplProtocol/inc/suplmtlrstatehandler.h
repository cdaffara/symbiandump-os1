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
// This file defines the class for the state handler of a SUPL MO-LR procedure 
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef __CSUPLMTLRSTATEHANDLER_H__
#define __CSUPLMTLRSTATEHANDLER_H__

#include <e32base.h>
#include "suplstatehandlerbase.h"
#include <lbs/lbsnetcommon.h>

class CSuplFsmSessionBase;

/** Class that defines the MT-LR procedure state handler
This class implements pure virtual methods defined by CSuplStateHandlerBase
@see CSuplStateHandlerBase
*/
NONSHARABLE_CLASS(CSuplMtLrStateHandler) : public CSuplStateHandlerBase
	{
public:

	static CSuplMtLrStateHandler* NewL(CSuplFsmSessionBase& aMachine);
	virtual ~CSuplMtLrStateHandler();
	
	// CSuplStateHandlerBase methods
	
	TBool EntryActionsL();
	
protected:

	CSuplMtLrStateHandler(CSuplFsmSessionBase& aMachine);
	
private:
	
	TBool EntryActionsFromSuplInitReceived();
	TBool EntryActionsFromNetConnectionStarted();
	TBool EntryActionsFromPosInitSentStateL();

private:

	TBool BuildCapabilitiesL(TLbsNetPosCapabilities& aCapabilities, TLbsNetPosRequestQuality& aQuality);
	};

#endif // __CSUPLMTLRSTATEHANDLER_H__
