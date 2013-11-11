// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains wrapper helper functions for test code to control the test wrappers
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef STARTUPADAPTATIONWRAPPER_H_
#define STARTUPADAPTATIONWRAPPER_H_

#include <e32std.h>
#include <ssm/startupadaptation.h>
#include <ssm/startupadaptationobserver.h>

class CStartupAdaptationWrapper : public CStartupAdaptation
{
public:
	static void SetLoaded(TBool aLoaded);
	static TBool IsLoaded();
	
	static CStartupAdaptation* NewStartupAdaptation(TAny* aParams);
	static void DeleteStartupAdaptation();
	
	static void SetLastCommandId(StartupAdaptation::TCommand aCommandId);
	static StartupAdaptation::TCommand LastCommandId();
	
	static void SetLastCommandDataL(TDesC8& aData);
	static HBufC8* LastCommandData();
	static void DeleteLastCommandData();
	
	static void TriggerObserverEventCallbackL(const StartupAdaptation::TEvent aEventId, TDesC8& aData);
	static void TriggerResponseCallbackL(const StartupAdaptation::TCommand aCommandId, TDesC8& aData);
    
private:

	static StartupAdaptation::TCommand iLastCommandId;
	static HBufC8* iLastCommandData;
	static CStartupAdaptationWrapper* iStartupAdaptation;
	static TBool iIsLoaded;
	
public:
	CStartupAdaptationWrapper(MStartupAdaptationObserver* aObserver);
	
    virtual void CommandL(const StartupAdaptation::TCommand aCommandId, TDesC8& aData);

    virtual void CancelCommandL(const StartupAdaptation::TCommand aCommandId);

    virtual TVersion Version() const;
	
private:
	MStartupAdaptationObserver* iObserver;
	TBool iResponding;
};

#endif /*STARTUPADAPTATIONWRAPPER_H_*/
