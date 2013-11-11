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

#include "Node1Factory.h"
#include "Node1.h"
   
using namespace Dummy;

//-=========================================================
//
// CNode1Factory methods
//
//-=========================================================	
CNode1Factory* CNode1Factory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CNode1Factory(TUid::Uid(CNode1Factory::EUid), *reinterpret_cast<CTestFactoryContainer*>(aParentContainer));
    }
    
CNode1Factory::CNode1Factory(TUid aFactoryId, CTestFactoryContainer& aParentContainer)
:	CTestFactoryBase(aFactoryId, aParentContainer)
    {
    }

CTestNodeBase& CNode1Factory::DoCreateL()
    {    
    CNode1* provider = CNode1::NewL(*this);
    return *provider;
    }






