// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __TCMD_STEP_PUBLISHSIMOWNEDANDCHANGED_H__
#define __TCMD_STEP_PUBLISHSIMOWNEDANDCHANGED_H__

#include <test/testexecutestepbase.h>
#include "cmdpublishsimownedandchanged.h"

_LIT(KTCCustomCmdTestPublishSimOwnedAndChanged, "CCustomCmdTestPublishSimOwnedAndChanged");

class CCustomCmdTestPublishSimOwnedAndChanged: public CTestStep
	{
public:
	CCustomCmdTestPublishSimOwnedAndChanged();
	~CCustomCmdTestPublishSimOwnedAndChanged();

	//from CTestStep
	TVerdict doTestStepL();

private:
	void doTestRetrieveSubscriberIdFromCentrepL();

private:
    CCustomCmdPublishSimOwnedAndChanged* iSimCustomCmd;

	};

#endif	// __TCMD_STEP_PUBLISHSIMOWNEDANDCHANGED_H__
