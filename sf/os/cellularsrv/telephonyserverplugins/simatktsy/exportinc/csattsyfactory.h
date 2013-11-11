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

#ifndef CSATTSYFACTORY_H
#define CSATTSYFACTORY_H

#include <e32std.h>
#include <e32base.h>

class MMessageRouter;
class MCtsySatService;
class MmMessageManagerCallback;
class CTsySatMessagingBase;

//IMPORT_C

class CSatTsyFactory : public CBase
	{
public:
	struct TSatTsyPtrContainer
		{
		CTsySatMessagingBase* iSatTsy;
		MmMessageManagerCallback* iSatTsyMessageManagerCallback;
		};
public:
	static CSatTsyFactory* NewL();
	IMPORT_C static CSatTsyFactory* NewLC();
	virtual ~CSatTsyFactory();
	virtual TSatTsyPtrContainer CreateSatTsyL(MMessageRouter& aMmMessageRouter, MCtsySatService& aSatService);
	
protected:
	CSatTsyFactory();
	};
	
#endif
