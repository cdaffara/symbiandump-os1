// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _CWRAPGETSMSPLIST_H_
#define _CWRAPGETSMSPLIST_H_

#include <etelmm.h>
#include <mmretrieve.h>


class CTestGetSmspList : public CActive
		{
	public:
		static CTestGetSmspList* NewLC(RMobileSmsMessaging& aSmsMessaging);
		~CTestGetSmspList();
		void Start();
		CMobilePhoneSmspList* RetrieveList();
	protected:
		CTestGetSmspList(RMobileSmsMessaging& aSmsMessaging);
		void ConstructL();
		void RunL();
		void DoCancel();
	protected:
		CRetrieveMobilePhoneSmspList* iRetrieve;
		RMobileSmsMessaging& iSmsMessaging;
		};
#endif

