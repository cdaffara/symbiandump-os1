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


#if (!defined __T_KEYPAD_DRIVER_DATA_H__)
#define __T_KEYPAD_DRIVER_DATA_H__

//	User Includes
#include "T_HALData.h"
#include "ActiveCallbackBase.h"

//	Epoc Includes
#include <e32twin.h>

/**
 * Test Active Notification class
 *
 */
class CT_KeypadDriverData : public CT_HALData
	{
public:
	/**
	* Public destructor
	*/
	~CT_KeypadDriverData();

	/**
	* Two phase constructor
	*
	* @leave	system wide error
	*/
	static	CT_KeypadDriverData*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
   
protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_KeypadDriverData();

	void	ConstructL();

	static TBool	GetPrepareNothing(CDataWrapperBase* aThis, const TDesC& aSection, TInt& aValueStart, TInt& aValueEnd);

	static void		SetKeyboardState(CDataWrapperBase* aThis, TInt aValue);
	static void		SetKeyboard(CDataWrapperBase* aThis, TInt aValue);
	static void		SetKeyboardDeviceKeys(CDataWrapperBase* aThis, TInt aValue);
	static void		SetKeyboardAppKeys(CDataWrapperBase* aThis, TInt aValue);
	static void		SetKeyboardClick(CDataWrapperBase* aThis, TInt aValue);
	static void		SetKeyboardClickVolumeMax(CDataWrapperBase* aThis, TInt aValue);
	static void		SetKeyboardClickState(CDataWrapperBase* aThis, TInt aValue);
	static void		SetKeyboardClickVolume(CDataWrapperBase* aThis, TInt aValue);

private: 

	void			DoCmdGetScanCodesL(const TDesC& aSection);
	void			DoCmdGetModifiersL(const TDesC& aSection);
	void			DoCmdTestAllKeysL(const TInt aAsyncErrorIndex);
	void			DoCmdTestOneModifierOneOtherKeyL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void			DoCmdTestTwoKeysL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void			DoCmdTestMultipleKeysL(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void			DoCmdVerifyKeypadState(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void			DoCmdPromptUser(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void 			DoCancel(CActive* aActive, TInt aIndex); 			// cancel the request
	void 			RunL(CActive* aActive, TInt aIndex);				// handle completed request

	//utility functions
 	void			ProcessResults();

protected:
	TInt							iKeyboardState;
	TInt							iKeyboard;
	TInt							iKeyboardDeviceKeys;
	TInt							iKeyboardAppKeys;
	TInt							iKeyboardClick;
	TInt							iKeyboardClickVolumeMax;
	TInt							iKeyboardClickState;
	TInt							iKeyboardClickVolume;
	CActiveCallback*				iActiveKey;
	CActiveCallback*				iActiveCombination;
	CActiveCallbackBase*			iActiveKeypadState;
	CActiveCallback*				iActivePrompt;
	TInt							iExpectedState;
	TInt							iKeyCount;
	TInt							iCombinationCount;
	TInt							iCombinationKeyCount;
	TBool							iFailTest;
	TPtrC							iUserPromptStr;
	TPtrC							iPassKeyStr;
	TInt							iPassKey;


private:	
	RConsole						iConsole;
	TConsoleKey						iKey;
	static const TEnumEntryTable	iEnumTableScanCodes[];
	static const TEnumEntryTable	iEnumTableCoverage[];
	static const THalTableLookup	iTableLookup[];
	RArray<TPtrC>					iPossibleScanCodeStrStore;
	RArray<TInt>					iPossibleScanCodeStore;
	RArray<TPtrC>					iPossibleModifierStrStore;
	RArray<TInt>					iPossibleModifierStore;
	RArray<TPtrC>					iAllPossibleKeysStrStore;
	RArray<TInt>					iAllPossibleKeysStore;
	RArray<TIniDataName>			iCombinationStrStore;
	RArray<TInt>					iExpectedStore;
	RArray<TInt>					iActualStore;
	RArray<TInt>					iEventStore;
	};

#endif /* __T_KEYPAD_DRIVER_DATA_H__ */
