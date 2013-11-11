// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:Test Adaptation server
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 

#ifndef __TSUS_STARTADAPTATIONSERVER_H__
#define __TSUS_STARTADAPTATIONSERVER_H__

#include <ssm/ssmsuscli.h>

_LIT(KTestAdaptationServerName,	"AdaptationSrvTest");

extern void RunAdaptationTestSrvL();
extern TInt TestAdaptationSrvInitFunction(TAny* /*aNothing*/);
extern TInt StartAdaptationServer(RThread& aServer);


#endif //__TSUS_STARTADAPTATIONSERVER_H__
