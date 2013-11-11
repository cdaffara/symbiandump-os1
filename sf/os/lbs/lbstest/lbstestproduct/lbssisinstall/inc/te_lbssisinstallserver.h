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
 @file te_settingsmanagerserver.h
 @internalTechnology
*/

#ifndef TE_LBSSISINSTALL_SERVER_H
#define TE_LBSSISINSTALL_SERVER_H

#include <test/TestExecuteServerBase.h>
#include "te_lbssisinstalldefs.h"

class CTe_LbsSisInstall : public CTestServer
	{
public:
	static CTe_LbsSisInstall* NewL();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	void AddHeader();
private:
	};

#endif // TE_LBSSISINSTALL_SERVER_H
