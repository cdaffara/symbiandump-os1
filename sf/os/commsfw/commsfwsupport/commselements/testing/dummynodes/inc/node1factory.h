// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_NODE1FACTORY_H
#define SYMBIAN_NODE1FACTORY_H

#include <dummyfactory.h>

class CNode1Factory : public Dummy::CTestFactoryBase
	{
public:
    enum { EUid = 0x10285A7F }; //UID allocated on 30/11/2007
	static CNode1Factory* NewL(TAny* aParentContainer);

protected:
	CNode1Factory(TUid aFactoryId, Dummy::CTestFactoryContainer& aParentContainer);
	virtual Dummy::CTestNodeBase& DoCreateL();
	};

#endif
//SYMBIAN_NODE1FACTORY_H

