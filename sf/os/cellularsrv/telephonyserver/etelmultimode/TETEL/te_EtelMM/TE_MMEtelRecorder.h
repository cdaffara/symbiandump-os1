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
// TE_ETELPHONEINFO.H
// 
//

#ifndef _TE_ETELRECORDER_H_
#define _TE_ETELRECORDER_H_

#include <etel.h>

class CTestRecorder : public CEtelMMTestStepBase
	{
public:
	CTestRecorder() ;
	~CTestRecorder(){} ;

	virtual enum TVerdict doTestStepL();
	};

#endif
