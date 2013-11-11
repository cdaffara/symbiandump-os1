// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name     : ctlbs_testPsy.cpp
// Part of  : Poison
// Origin   : Poison/TietoEnator
// Created  : 2003-09-12 by Mikael Berg
// Version  :
// $Revision: 1.1 $, $Date: 2003/10/07 13:28:22 $ by $Author: mikael.berg $
// Version history:
// Template version: 1.01, 1.3.2000 by maparnan
// <ccm_history>
// $Log: ctlbs_testPsy1Main.cpp,v $
// Revision 1.1  2003/10/07 13:28:22  mikael.berg
// Added
// </ccm_history>
//



// INCLUDES
#include <e32base.h>
#include <ecom/implementationproxy.h>
#include "ctlbstestpsy1positioner.h"
#include "testpsy1.hrh"


// ================= OTHER EXPORTED FUNCTIONS ==============
#ifndef EKA2
// ---------------------------------------------------------
// E32Dll
// This function is required by all EPOC32 DLLs.
// (other items were commented in a header).
// ---------------------------------------------------------
//
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif

const TImplementationProxy KFactoryPtr = 
{{KPosImplementationUid}, (TProxyNewLPtr)CT_LbsTestPsy1Positioner::NewL};

//
// ---------------------------------------------------------
// ImplementationGroupProxy
// Required PSY interface.
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
    aTableCount = 1;
    return &KFactoryPtr;
	}

//  End of File
