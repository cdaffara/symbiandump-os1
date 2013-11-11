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
//

#ifndef CMOCKPOWERSTATESY_H
#define CMOCKPOWERSTATESY_H

#include <hwrm/hwrmpowerstateplugin.h>

class CMockPowerStateEngine;

class CMockPowerStateSY : public CHWRMPowerStatePlugin
	{
public:
	static CMockPowerStateSY* NewL();
	virtual ~CMockPowerStateSY();

 	void DoProcessCallback(TUint aKey, TInt aValue);

protected:
	CMockPowerStateSY();
	void ConstructL();

private:
	CServer2* 				iServer;
	CMockPowerStateEngine* 	iEngine;
	};
	
#endif
