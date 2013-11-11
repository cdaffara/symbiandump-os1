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


#ifndef T_RMMCUSTOMAPI_DATA_H
#define T_RMMCUSTOMAPI_DATA_H

#include "datawrapperbase.h"

// CBase
#include <e32base.h>
// TDesC
#include <e32cmn.h>
// For ETel API.
#include <etel.h>
#include <etelmm.h>
#include <rmmcustomapi.h>

class CT_RMmCustomApiData : public CDataWrapperBase
	{
public:
	~CT_RMmCustomApiData();
	static	CT_RMmCustomApiData* NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_RMmCustomApiData();
	void	ConstructL();

private:
	void RunL(CActive* aActive, TInt aIndex); 
	
	void DoCmdGetCurrentSystemNetworkModes(const TInt aAsyncErrorIndex);
	void DoCmdUtilityModeCheck(const TTEFSectionName& aSection);
	void DoCmdSetSystemNetworkMode(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	void DoCmdOpen(const TTEFSectionName& aSection);
	void DoCmdClose();

	TUint32 GetPhoneNetworkMode();
	TBool CheckNetworkMode(TInt aNetworkMode);
	
private:
	CActiveCallback* 	iActiveCallback;
	TUint32 iNetworkMode;

	/**
	 * Access to functionality assosiated with a specific mobile line.
	 */
	RMmCustomAPI *iCustomPhone; //Wrapped object
	
	static const TEnumEntryTable iEnumTestCaseMode[];

	};

#endif // T_RMMCUSTOMAPI_DATA_H
	
	
