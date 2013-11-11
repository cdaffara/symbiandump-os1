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
//

/**
 @file
 @internalComponent 
*/

#ifndef _TE_MMMEMSMS_H_
#define _TE_MMMEMSMS_H_

#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"
#include "TE_mmsms.h"

class CTestMemSmsMessaging : public CTestSmsMessaging
	{
public:
	TInt iTestCount;
	CTestMemSmsMessaging() ;
	~CTestMemSmsMessaging(){} ;
	virtual enum TVerdict doTestStepL();
	CTestSmsMessaging* iTestSmsMessaging;


private:
	class CMemTestGetSmspList : public CTestSmsMessaging::CTestGetSmspList
		{
	public:
		static CMemTestGetSmspList* NewLC(RMobileSmsMessaging& aMessaging, CTestMemSmsMessaging* aTestMemSmsMessaging);
		void Start();
		CTestMemSmsMessaging* iTestMemSmsMessaging;
	protected:
		CMemTestGetSmspList(RMobileSmsMessaging& aMessaging, CTestMemSmsMessaging* aTestMemSmsMessaging);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};

friend class CMemTestGetSmspList;

	};

#endif
