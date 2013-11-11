/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#if (!defined __T_ACTIVE_R_CONSOLE_READ_H__)
#define __T_ACTIVE_R_CONSOLE_READ_H__

//	EPOC includes
#include <e32twin.h>
#include <e32keys.h> 

//	User Includes
#include "DataWrapperBase.h"
#include "ActiveCallbackBase.h"
#include "DrawUtils.h"
 
class CT_ActiveRConsoleRead : public CActiveCallbackBase

/**
* Test Active Notification class
@test
@publishedPartner
@see CActiveCallbackBase
*/
	{
protected:
	enum TEventOccurance
		{
		EEventOccuranceOnce,
		EEventOccuranceZeroOrMore,
		EEventOccuranceOneOrMore,
		};

	struct TEventConfig
		{
		TInt			iEventType;
		TEventOccurance	iEventOccurance;
		TBool			iDataVerify;
		TBool			iDataDraw;
		};

public:
	/**
	* Public  destructor
	*/
	static	CT_ActiveRConsoleRead*	NewL(CDataWrapperBase& aDataWrapperBase);
	~CT_ActiveRConsoleRead();
	TBool	KickStartL (const TDesC& aSection, const TInt aAsyncErrorIndex,RConsole& aConsole);
	TBool	VerifyDataAndKick(const TInt aAsyncErrorIndex, RConsole& aConsole);
	TConsoleKey	GetConsoleKey();
	
	
	 		
protected:
	CT_ActiveRConsoleRead(CDataWrapperBase& aDataWrapperBase);	
	void 	DestroyData();
	TBool	ExitEvent();
	void	DrawData();

private:
	TBool	KickNext(const TInt aAsyncErrorIndex, RConsole& aConsole);
	void	Kick (const TInt aAsyncErrorIndex, RConsole& aConsole);

private:
   	TPtrC	iSection;
    TUint8	iColourValueBlack;
    TUint8	iColourValueWhite;
    TInt	iEventIndex;
    TInt	iEventCount;
    TInt	iTimeOut;
    TInt	iErrorMargin;
    TBool	iHasExitKeyCode;
    TBool	iHasExitRectangle;
    TInt	iExitKeyCode;
    TRect	iExitRectangle;
	TInt	iTestIndex;
	TInt	iNumberOfTests;

	TBool	iHasExpectedKeyCode;
	TBool	iHasExpectedRectangle;
    TInt	iExpectedKeyCode;
    TRect	iExpectedRectangle;

    RArray<TEventConfig>	iEvent;
    TEventConfig 			iExpectedConfig;
    TConsoleKey             iKey;
 

   
    static const CDataWrapperBase::TEnumEntryTable	iEnumRawEventTable[];
	static const CDataWrapperBase::TEnumEntryTable	iEnumKeyCodeTable[];
	static const CDataWrapperBase::TEnumEntryTable	iEnumEventOccuranceTable[];
	};
 
#endif /*__T_ACTIVE_R_CONSOLE_READ_H__ */
