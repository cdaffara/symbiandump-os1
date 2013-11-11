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


#ifndef T_RPOSITIONSERVERDATA_H
#define T_RPOSITIONSERVERDATA_H

//User Includes
#include "datawrapperbase.h"

//Epoc Includes
#include <lbs.h>

class CT_RPositionServerData : public CDataWrapperBase
	{
public:
	~CT_RPositionServerData();
	static	CT_RPositionServerData* NewL();
	virtual TAny* GetObject();
	virtual TBool DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	CT_RPositionServerData();
	
private:
	void ConstructL();
	void DestroyData();
	
protected:
	void DoCmdConnect();
	void DoCmdClose();
	void DoCmdGetModuleInfoByIndex();
	void DoCmdGetModuleStatus(const TTEFSectionName& aSection);

private:
	RPositionServer iPositionServer;
	TPositionModuleId iPositionModuleId;
	};


#endif //T_RPOSITIONSERVERDATA_H

