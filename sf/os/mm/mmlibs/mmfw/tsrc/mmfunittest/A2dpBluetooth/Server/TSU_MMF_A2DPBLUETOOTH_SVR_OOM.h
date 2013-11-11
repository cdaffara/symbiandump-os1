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

#ifndef __TSU_MMF_A2DPBLUETOOTH_SVR_OOM_H__
#define __TSU_MMF_A2DPBLUETOOTH_SVR_OOM_H__


#include "TSU_MMF_A2DPBLUETOOTH_SVR_Step.h"


class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0400 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0400() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0400(){} ;
	virtual TVerdict DoTestStepL();
	} ;

class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0401 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0401() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0401(){} ;
	virtual TVerdict DoTestStepL();
	} ;
	
class CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0402 : public CTestStep_MMF_A2DPBLUETOOTH_SVR
	{
public:
	CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0402() ;
	~CTestStep_MMF_A2DPBLUETOOTH_SVR_U_0402(){} ;
	virtual TVerdict DoTestStepL();
	} ;

#endif /* __TSU_MMF_A2DPBLUETOOTH_SVR_OOM_H__ */
