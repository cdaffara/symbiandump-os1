// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// te_eteldialcallcontrol.h
// This file contains class declarations of TestExecute test steps
// which test that the RMobileCall::DialCallControl API can only
// be called if the application has the correct SID and capabilities
// to call it.
// 
//

/**
 @file
*/

#ifndef TE_ETELDIALCALLCONTROL_H_
#define TE_ETELDIALCALLCONTROL_H_

#include <etelmm.h>
#include "TE_EtelMMTestStepBase.h"

class CTestDialCallControlCorrectSIDCorrectCap : public CEtelMMTestStepBase
	{
public:
	CTestDialCallControlCorrectSIDCorrectCap() ;
	~CTestDialCallControlCorrectSIDCorrectCap(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestDialCallControlIncorrectSIDCorrectCap : public CEtelMMTestStepBase
	{
public:
	CTestDialCallControlIncorrectSIDCorrectCap() ;
	~CTestDialCallControlIncorrectSIDCorrectCap(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestDialCallControlCorrectSIDIncorrectCap : public CEtelMMTestStepBase
	{
public:
	CTestDialCallControlCorrectSIDIncorrectCap() ;
	~CTestDialCallControlCorrectSIDIncorrectCap(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestDialCallControlIncorrectSIDIncorrectCap : public CEtelMMTestStepBase
	{
public:
	CTestDialCallControlIncorrectSIDIncorrectCap() ;
	~CTestDialCallControlIncorrectSIDIncorrectCap(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif //TE_ETELDIALCALLCONTROL_H_
