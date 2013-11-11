/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file Te_LbsBtGpsPsySuiteServer.h
 @internalTechnology
*/

#ifndef __TE_LBSBTGPSPSY_SERVER_H__
#define __TE_LBSBTGPSPSY_SERVER_H__
#include <test/TestExecuteServerBase.h>


class CTe_LbsBtGpsPsy : public CTestServer
	{
public:
	static CTe_LbsBtGpsPsy* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);

protected:
	void ConstructL();
	};

#endif
