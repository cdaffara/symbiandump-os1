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

/**
 @file
 @internalAll 
*/

#ifndef CCTSYDISPATCHERFACTORY_H
#define CCTSYDISPATCHERFACTORY_H

#include <ctsy/mltsyfactory.h>

class CMessageRouter;

class CCtsyDispatcherFactoryV1 : public CBase, public MLtsyFactoryV1
	{
public:
	static CCtsyDispatcherFactoryV1* NewL();
	virtual ~CCtsyDispatcherFactoryV1();
		
public:	// from MLtsyFactoryBase
	TCtsyInterfaceVersion Version();
	void Release();

public: // from MLtsyFactoryV1
	MMessageRouter* GetMessageRouter(MmMessageManagerCallback& aMessageManager);
	CTsySatMessagingBase* GetSatMessagingService(MCtsySatService& aSatMessagingService);
	
protected:
	CTsySatMessagingBase* GetSatMessagingServiceL(MCtsySatService& aSatMessagingService);

protected:
	CCtsyDispatcherFactoryV1();
	void ConstructL();
	
private:

	CMessageRouter* iMessageRouter;	
	RLibrary iloadedLib;
	}; // class CCtsyDispatcherFactoryV1

#endif // CCTSYDISPATCHERFACTORY_H
