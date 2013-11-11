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


#ifndef T_RTELSERVER_DATA_H
#define T_RTELSERVER_DATA_H

#include "DataWrapperBase.h"
// For ETel API.
#include <etel.h>
// For ETel API.
#include <etelmm.h>

class CT_RTelServerData : public CDataWrapperBase
	{
public:
	~CT_RTelServerData();
	static	CT_RTelServerData* NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_RTelServerData();
	void ConstructL();

private:
	//== Mobile Line functions
	void DoCmdConnect();
	void DoCmdClose();
	
private:
	/**
	 * Telephony server
	 */
	RTelServer *iTelServer; //Wrapped object

	};

#endif // T_RTELSERVER_DATA_H

