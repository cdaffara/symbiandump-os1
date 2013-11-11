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


#ifndef T_RCONNECTIONDATA_H
#define T_RCONNECTIONDATA_H

#include "DataWrapperBase.h"

#include <es_sock.h>
#include <commdbconnpref.h>
#include <commdb.h>
#include <mmlist.h>
#include <mmretrieve.h>

class CT_RConnectionData: public CDataWrapperBase
	{
public:
	static CT_RConnectionData* NewL();
	virtual ~CT_RConnectionData();

	void RunL(CActive* aActive, TInt aIndex);
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);	

protected:
	CT_RConnectionData();
	void ConstructL();
	
private:
	void DoCmdOpenL(const TTEFSectionName& aSection);
	void DoCmdStart(const TTEFSectionName& aSection);
	void DoCmdStop();
	void DoCmdClose();
	void DoCmdSelectOutgoingIAPL(const TTEFSectionName& aSection);

private:
	TCommDbConnPref* CT_RConnectionData::SelectOutgoingIAPL(const TDesC& aIAPName);
   
private:
	RConnection* 		iConnection;
	CActiveCallback*	iActiveCallback;
	/**
	 * Connection preferences to setup the data connection with
	 * RConnection class.
	 */
	TCommDbConnPref*	iCommDbConnPref;
	/**
	 * IAP ID for used IAP.
	 */
	TUint32				iIAP;
	};
	
#endif // T_RCONNECTIONDATA_H

	