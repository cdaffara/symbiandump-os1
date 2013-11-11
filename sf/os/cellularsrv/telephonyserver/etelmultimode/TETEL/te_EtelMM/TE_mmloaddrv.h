// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TE_MMCLILOADDRV.H
// 
//

/**
 @file
 @internalComponent 
*/


#ifndef _TE_ETELMM_LOADDRV_H_
#define _TE_ETELMM_LOADDRV_H_

#include <etelmm.h>
#include <mmlist.h>
#include "TE_EtelMMTestStepBase.h"

class CTestMMLoadDrv : public CEtelMMTestStepBase
	{
public:
	CTestMMLoadDrv() ;
	~CTestMMLoadDrv(){} ;
	virtual enum TVerdict doTestStepL();
	};

class CTestMMUnLoadDrv : public CEtelMMTestStepBase
	{
public:
	CTestMMUnLoadDrv() ;
	~CTestMMUnLoadDrv(){} ;
	virtual enum TVerdict doTestStepL();
	};

#endif
