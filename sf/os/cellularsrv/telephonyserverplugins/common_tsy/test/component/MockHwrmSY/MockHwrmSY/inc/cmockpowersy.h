// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef CMOCKPOWERSY_H_
#define CMOCKPOWERSY_H_

#include <hwrm/hwrmpluginservice.h>

class CMockPowerEngine;

class CMockPowerSY : public CHWRMPluginService
	{
public:
	static CMockPowerSY* NewL();
	virtual ~CMockPowerSY();
	
	virtual void ProcessCommandL(const TInt aCommandId, const TUint8 aTransId, TDesC8& aData);
	virtual  void CancelCommandL(const TUint8 aTransId, const TInt aCommandId);
	void DoProcessResponse(const TInt aCommandId,const TUint8 aTransId, const TDesC8& aData);

protected:
	CMockPowerSY();
	void ConstructL();
	//virtual const TDesC& ServerName();
	
private:
	CServer2* 			iServer;
	CMockPowerEngine* 	iEngine;

	};
	
#endif /*CMOCKPOWERSY_H_*/
