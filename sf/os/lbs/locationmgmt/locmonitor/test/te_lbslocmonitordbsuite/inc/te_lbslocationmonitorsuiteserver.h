/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TE_LBSLOCATIONMONITOR_SERVER_H__
#define __TE_LBSLOCATIONMONITOR_SERVER_H__
#include <test/testexecuteserverbase.h>


class CTe_LbsLocationMonitorSuite : public CTestServer
	{
public:
	static CTe_LbsLocationMonitorSuite* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	IMPORT_C virtual void ConstructL(const TDesC& aName);
	~CTe_LbsLocationMonitorSuite();
	
private:
	};

#endif
