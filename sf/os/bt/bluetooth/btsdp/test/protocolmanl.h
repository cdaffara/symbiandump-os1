// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// a test harness for SDP protocol to send manual PDUs into the server
// and onto the wire.
// 
//


#ifndef PROTOCOLMANL_H
#define PROTOCOLMANL_H

#include "SDPDatabase.h"


GLREF_D CSdpDatabase *gSdpDatabase;

// #ifdef __FLOGGING__
// GLREF_D TInt sdp_debug_level = 5;
// #endif

HBufC8* InjectLC(TInt aReqID, const TPtr8& aReqBuf);
void TestBL(RTest& atest);
void TestCL(RTest& atest, CSdpDatabase *aDb);


#endif
