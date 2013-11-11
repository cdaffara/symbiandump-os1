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


#if (!defined __ACTIVE_CALLBACK_BASE__)
#define __ACTIVE_CALLBACK_BASE__

//	USER includes
#include "DataWrapperBase.h"

class CActiveCallbackBase : public CActiveCallback
	{
private:
	class CActiveCallbackTimer : public CTimer
		{
	public:
		static CActiveCallbackTimer*	NewL(CActive& aActive, TInt aPriority=EPriorityStandard);

		//	CActive implementation
		virtual void RunL();

	protected:
		CActiveCallbackTimer(CActive& aActive, TInt aPriority);

	private:
		CActive&	iActive;
		};

public:
	virtual ~CActiveCallbackBase();
	static CActiveCallbackBase*	NewL(CDataWrapperBase& aDataWrapperBase, TInt aPriority=EPriorityStandard);
	static CActiveCallbackBase*	NewLC(CDataWrapperBase& aDataWrapperBase, TInt aPriority=EPriorityStandard);

	void	Activate(TInt aIndex, TInt aTimeout=0);
	void	KillTimer();

protected:
	CActiveCallbackBase(CDataWrapperBase& aDataWrapperBase, TInt aPriority);
	void	ConstructL();

public:
	CDataWrapperBase&		iDataWrapperBase;

private:
	CActiveCallbackTimer*	iTimer;
	};

#endif /* __ACTIVE_CALLBACK_BASE__ */
