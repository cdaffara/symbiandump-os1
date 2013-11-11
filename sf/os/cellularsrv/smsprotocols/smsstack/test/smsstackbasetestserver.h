// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// @file
// @test

#ifndef SMSSTACKBASETESTSERVER_H
#define SMSSTACKBASETESTSERVER_H

#include <test\testexecuteserverbase.h>
#include <etelmm.h>

class CSmsStackTestServer : public CTestServer
/**
    Describes the base test server for the SMS stack test harness.
 */
    {
public:
    IMPORT_C virtual ~CSmsStackTestServer();
    
protected:
    IMPORT_C CSmsStackTestServer();
    IMPORT_C void InitializeTsyAndPhonesL();

private:
    void GetTsyNamesFromCommsdatL(RArray<TName>& aTsyNameList);
    void LoadTsyL(RTelServer& aServer, RPhone& aPhone, const TDesC& aTsyName);
    
private:
    /**
     * SMS stack first interrogates ETel server to get information about available SIMs,
     * and then opens phone sessions by using the retrieved available SIM info.
     * ETel returns the SIM info based on the loaded TSYs. If there is no connection to a phone,
     * ETel unloads the corresponding TSY. To keep TSYs required by SMS tests loaded, the test code
     * opens connection to the required phones and keep them alive throughout all test steps.
     * In a real device, this is done as a part of the system start.
     */
    RTelServer iTelServer;
    RMobilePhone iPhone1;
    RMobilePhone iPhone2;
    };
 
#endif // SMSSTACKBASETESTSERVER_H
