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


#ifndef T_RMOBILELINE_DATA_H
#define T_RMOBILELINE_DATA_H

#include "datawrapperbase.h"
// For ETel API.
#include <etel.h>

// For ETel API.
#include <etelmm.h>

class CT_RMobileLineData : public CDataWrapperBase
	{
public:
	~CT_RMobileLineData();
	static	CT_RMobileLineData* NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	TName* GetCallNameL(TInt aCall);

protected:
	CT_RMobileLineData();
	void	ConstructL();
	void 	InitialiseL();
private:
	void RunL(CActive* aActive, TInt aIndex); 
	
	void DoCmdOpen(const TDesC& aSection);
	void DoCmdClose();
	void DoCmdNotifyIncomingCall(const TDesC& aSection, const TInt aAsyncErrorIndex);
	
private:
	CActiveCallback* 	iActiveCallback;

	TInt iMaxPhoneCalls;
	/**
	 * Access to functionality assosiated with a specific mobile line.
	 */
	RMobileLine *iMobileLine; //Wrapped object

	/**
	* Array of Call names
	*/
	RArray< TName > iCallNames; //Pointer
	};

#endif // T_RMOBILELINE_DATA_H

	