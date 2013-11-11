// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TE_SIM_PHBK_STOREICCLOCKED_TEST__
#define __TE_SIM_PHBK_STOREICCLOCKED_TEST__

#include "Te_SimTestStepBase.h"
#include "Te_SimConstants.h"

class CSimPhBkStoreICCLockedTest : public CPhBkAndPacketTestsTestStepBase
	{
public:
	CSimPhBkStoreICCLockedTest();
 	virtual TVerdict doTestStepL();

private:
	RMobilePhoneBookStore iPhonebook;
	};

#endif // __TE_SIM_PHBK_STOREICCLOCKED_TEST__
