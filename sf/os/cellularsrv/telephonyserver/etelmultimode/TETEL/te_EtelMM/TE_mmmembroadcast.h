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

#ifndef _TE_MMMEMBROADCAST_H_
#define _TE_MMMEMBROADCAST_H_

#include <etelmm.h>
#include <e32base.h>
#include <mmretrieve.h>
#include "TE_mmcanceller.h"
#include "TE_mmbroadcast.h"


class CTestMemBroadcastMessaging : public CTestBroadcastMessaging//CEtelMMTestStepBase
	{
public:
	TInt iTestCount;
	CTestMemBroadcastMessaging() ;
	~CTestMemBroadcastMessaging(){} ;
	virtual enum TVerdict doTestStepL();

private:

	class CMemTestGetCbmiList : public CTestBroadcastMessaging::CTestGetCbmiList
		{
	public:
		static CMemTestGetCbmiList* NewLC(RMobileBroadcastMessaging &aMessaging, CTestMemBroadcastMessaging* aTestMemBroadcastMessaging);
		void Start();
		CTestMemBroadcastMessaging* iTestMemBroadcastMessaging;
	protected:
		CMemTestGetCbmiList(RMobileBroadcastMessaging &aMessaging, CTestMemBroadcastMessaging* aTestMemBroadcastMessaging);
		TInt RunError(TInt aError);
		void StartMemTest();
		virtual void RunL();
	protected:
		TInt iFailEvery;
		};

friend class CMemTestGetCbmiList;
	};

#endif
