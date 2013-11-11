// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests for ECam
// 
//

#ifndef TSU_ECM_API_TESTSUITE_H
#define TSU_ECM_API_TESTSUITE_H

#include <testframework.h>

_LIT(KTestCameraPluginName, "C:\\102070cc.txt");
#ifdef SYMBIAN_MULTIMEDIA_THREEPLANEARCH
_LIT(KMMCameraPluginName, "C:\\mmcameraclientplugin.txt");
#endif //SYMBIAN_MULTIMEDIA_THREEPLANEARCH

class  CTestSuite_ECam_API : public CTestSuite
{
public:
	virtual ~CTestSuite_ECam_API();
	void 	InitialiseL();
	TPtrC 	GetVersion();
	RFs     ifsSession;
};

#endif // TSU_ECM_API_TESTSUITE_H

