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

#ifndef TE_SMSMONDSK_H
#define TE_SMSMONDSK_H

#include "Te_SmsTestSteps.h"

class CSetDiskMonitorLimits : public CSmsBaseTestStep
	{
public:
    CSetDiskMonitorLimits();
	virtual TVerdict doTestStepL();	
	};

class CSetDiskSpace : public CSmsBaseTestStep
	{
public:
	CSetDiskSpace();
	virtual TVerdict doTestStepL();	
	};

class CFreeDiskSpace : public CSmsBaseTestStep
	{
public:
    CFreeDiskSpace();
	virtual TVerdict doTestStepL();
	};

#endif // TE_SMSMONDSK_H
