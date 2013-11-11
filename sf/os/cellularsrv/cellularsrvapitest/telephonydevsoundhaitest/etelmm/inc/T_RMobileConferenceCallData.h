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


#ifndef T_RMOBILECONFERENCECALL_DATA_H
#define T_RMOBILECONFERENCECALL_DATA_H

#include "DataWrapperBase.h"
// For ETel API.
#include <etel.h>

// For ETel API.
#include <etelmm.h>

class CT_RMobileConferenceCallData : public CDataWrapperBase
	{
public:
	~CT_RMobileConferenceCallData();
	static	CT_RMobileConferenceCallData*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_RMobileConferenceCallData();
	void	ConstructL();

private:
	void RunL(CActive* aActive, TInt aIndex); 

	void DoCmdOpen(const TTEFFunction& aSection);
	void DoCmdClose();
	void DoCmdCreateConference(const TInt aAsyncErrorIndex);
	void DoCmdEnumerateCalls(const TTEFFunction& aSection);

private:
	CActiveCallback* 	iActiveCallback;

	/**
	 * Mobile conference call interface to ETel.
	 */
	RMobileConferenceCall *iMobileConferenceCall;

	};


#endif // T_RMOBILECONFERENCECALL_DATA_H

	