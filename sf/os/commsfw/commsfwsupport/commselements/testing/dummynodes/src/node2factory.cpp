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

#include "Node2Factory.h"
#include "Node2.h"
   
using namespace Dummy;

//-=========================================================
//
// CNode2Factory methods
//
//-=========================================================	
CNode2Factory* CNode2Factory::NewL(TAny* aParentContainer)
    {
    return new (ELeave) CNode2Factory(TUid::Uid(CNode2Factory::EUid), *reinterpret_cast<CTestFactoryContainer*>(aParentContainer));
    }
    
CNode2Factory::CNode2Factory(TUid aFactoryId, CTestFactoryContainer& aParentContainer)
:	CTestFactoryBase(aFactoryId, aParentContainer)
    {
    }

CTestNodeBase& CNode2Factory::DoCreateL()
    {    
    CNode2* provider = CNode2::NewL(*this);
    return *provider;
    }






