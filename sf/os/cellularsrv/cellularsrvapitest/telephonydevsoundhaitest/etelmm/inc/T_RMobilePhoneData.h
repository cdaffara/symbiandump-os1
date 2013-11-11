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


#ifndef T_RMOBILEPHONE_DATA_H
#define T_RMOBILEPHONE_DATA_H

#include "DataWrapperBase.h"
// For ETel API.
#include <etel.h>

// For ETel API.
#include <etelmm.h>

class CT_RMobilePhoneData : public CDataWrapperBase
	{
public:
	~CT_RMobilePhoneData();
	static	CT_RMobilePhoneData* NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_RMobilePhoneData();
	void	ConstructL();

private:
	void DoCmdOpen(const TDesC& aSection);
	void DoCmdClose();
	void DoCmdInitialise();
	
private:
	/**
	 * Mobile phone for SIM initialisation.
	 */
	RMobilePhone *iMobilePhone; //Wrapped object

	};

#endif // T_RMOBILEPHONEDATA_H
	
	
	