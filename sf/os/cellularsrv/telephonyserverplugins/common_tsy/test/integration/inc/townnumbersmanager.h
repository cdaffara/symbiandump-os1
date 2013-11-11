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
// The declaration of the TRPSOwnNumbersManager.
// 
//

/**
 @internalTechnology
*/
#ifndef TOWNNUMBERSMANAGER_H
#define TOWNNUMBERSMANAGER_H
 
#include "cetelsessionmgr.h"
#include "rpsownnumbers.h"
#include "cctsyintegrationtestsuitepanics.h"

//this holds the Own Numbers cache. It also holds state to say whether cache has been retrieved.
//This object is passed to all CCTSYIntegrationTestSuiteStepBase derived classes.
class TOwnNumbersManager
	{
public:

	inline void SetEtelSessMgr(CEtelSessionMgr* aMgr)
		{
		iEtelSessMgr = aMgr;
		}
		
	inline TRPSOwnNumbers& OwnNumbers()
		{
		return iOwnNumbers;
		};
		
	void CacheOwnNumbersL();
protected:
	void CacheOwnNumbersAO();
	static TInt AsyncCallback(TAny* aTOwnNumbersManagerInstance);
private:
	TBool iGotNumbers;
	TRPSOwnNumbers iOwnNumbers;
	CEtelSessionMgr* iEtelSessMgr;
	TInt iRetrieveError;
	};
	
//This a wrapper around TOwnNumbersManager that is used within TestStepL bodies. 
class TOwnNumbersCache
	{
public:
	inline TRPSOwnNumbers& OwnNumbers()
		{
		__ASSERT_ALWAYS(iOwnNumbersManager, User::Panic(KPanic,KCTSYPanic_badhandle));
		return iOwnNumbersManager->OwnNumbers();
		};
	
	inline void SetOwnNumbersManager(TOwnNumbersManager* aOwnNumbersManager)
		{
		iOwnNumbersManager = aOwnNumbersManager;
		}
		
	inline void CacheOwnNumbersL()
		{
		__ASSERT_ALWAYS(iOwnNumbersManager, User::Panic(KPanic,KCTSYPanic_badhandle));
		return iOwnNumbersManager->CacheOwnNumbersL();
		}
		
private:
	TOwnNumbersManager* iOwnNumbersManager;
	};
	
#endif //TOWNNUMBERSMANAGER_H
