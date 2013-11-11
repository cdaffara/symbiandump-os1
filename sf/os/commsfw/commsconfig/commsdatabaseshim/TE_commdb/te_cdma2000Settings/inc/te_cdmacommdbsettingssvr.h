//
//
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
//
//

/**
 @file 
 @internalComponent
 
 
 Standard TestExecute server header:
 */


#ifndef __TE_Cdma2000settingsSVR_H__
#define __TE_Cdma2000settingsSVR_H__

#include <testexecuteserverbase.h>

/**
TestSuite server

@internalComponent
@test
*/
class CTe_Cdma2000CommdbSettingsSvr : public CTestServer
	{
public:
	static CTe_Cdma2000CommdbSettingsSvr* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs(){return iFs;};

private:
	RFs iFs;
	};
	
#endif
