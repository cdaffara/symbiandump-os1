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


#if (!defined __T_R_CONSOLE_DATA_H__)
#define __T_R_CONSOLE_DATA_H__

//	EPOC includes
#include <e32twin.h>


//	User Includes
#include "T_RSessionBaseData.h"
#include "T_ActiveRConsoleRead.h"


class CT_RConsoleData : public CT_RSessionBaseData

/**
* Test Active Notification class
@test
@publishedPartner
@see CT_RSessionBaseData
*/
	{
public:
	/**
	* Public  destructor
	*/
	~CT_RConsoleData();
	static	CT_RConsoleData*	NewL();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	virtual TAny*	GetObject();

protected:
	CT_RConsoleData();
	void	ConstructL();
	TInt	DoCmdRead(const TDesC& aSection , const TInt aAsyncErrorIndex);
	TInt	DoCmdConstructor();
	void	DoCmdDestructor();
	TInt	DoCmdControl(const TDesC& aSection);
	TInt	DoCmdInit(const TDesC& aSection);
	TInt	DoCmdCreate();
	TInt	DoCmdDestroy();
	TInt	DoCmdWrite(const TDesC& aSection);
	TInt	DoCmdClearScreen();

	//	CT_RSessionBaseData implementation
	virtual RSessionBase*	GetSessionBase();

	//	CT_RHandleBaseData implementation
	virtual RHandleBase*	GetHandleBase();

private:
	void 	DestroyData();
	void	RunL(CActive* aActive, TInt aIndex);
	virtual void	DoCancel(CActive* aActive, TInt aIndex);

protected:
	CT_ActiveRConsoleRead*	iActiveRConsoleRead;
	RConsole*				iConsole;
	};

#endif /*__T_R_CONSOLE_DATA_H__ */
