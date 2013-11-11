// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Base SY engine for Hwrm plugin (Light and Vibra)
// 
//

#ifndef CMOCKHWRMPLUGINSY_H
#define CMOCKHWRMPLUGINSY_H

#include <hwrm/hwrmpluginservice.h>

class CMockHwrmPluginEngine;

class CMockHwrmPluginSY : public CHWRMPluginService
	{
public:
	virtual ~CMockHwrmPluginSY();
	
	virtual void ProcessCommandL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData);
 	virtual void CancelCommandL(const TUint8 aTransId, const TInt aCommandId);

 	void DoProcessResponse(const TInt aCommandId,const TUint8 aTransId, const TDesC8& aData);

protected:
	CMockHwrmPluginSY();
	void ConstructL();
	virtual const TDesC& ServerName() =0;
	virtual CMockHwrmPluginEngine* CreateEngine() =0;
	
private:
	CServer2* 				iServer;
	CMockHwrmPluginEngine* 	iEngine;
	};
	
#endif
