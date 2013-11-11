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

#include "smsstackbasetestserver.h"
#include <test/TestExecuteStepBase.h>
#include <e32def.h>
#include <commsdattypesv1_1.h>
#include <commsdat_partner.h>

using namespace CommsDat;

EXPORT_C CSmsStackTestServer::CSmsStackTestServer()
    {
    }

EXPORT_C CSmsStackTestServer::~CSmsStackTestServer()
    {
    iPhone1.Close();
    iPhone2.Close();
    iTelServer.Close();
    }

EXPORT_C void CSmsStackTestServer::InitializeTsyAndPhonesL()
/**
 * This functions first gets the list of TSYs from Commsdat and then load each TSY in order.
 * Having loaded the TSY, the function opens a phone session to this TSY. 
 * Maximum two TSYs are loaded. If there is only one TSY, aPhone2 parameter is not changed.
 */
    {
    TInt err = iTelServer.Connect();
        
    if ( KErrNone != err )
        {
        ERR_PRINTF2(_L("Couldn't connect to ETel server. Error:%d"), err);
        User::Leave( err );
        }
        
    RArray<TName> tsyNameList;
    CleanupClosePushL(tsyNameList);
        
    GetTsyNamesFromCommsdatL(tsyNameList);
        
    TInt tsyCount = tsyNameList.Count();
        
    if( tsyCount < 1 )
        {
        ERR_PRINTF1(_L("Couldn't find any TSY record in Commsdat Baseband table."));
        User::Leave( KErrNotFound );
        }
    else if ( tsyCount > 2)
        {
        ERR_PRINTF2(_L("Found %d baseband record. First two TSY will be loaded."), tsyCount);
        }
           
    LoadTsyL(iTelServer, iPhone1, tsyNameList[0]);
    if(--tsyCount)
        {
        LoadTsyL(iTelServer, iPhone2, tsyNameList[1]);
        }
        
    CleanupStack::PopAndDestroy(&tsyNameList);
    }

void CSmsStackTestServer::GetTsyNamesFromCommsdatL(RArray<TName>& aTsyNameList)
    {
    CMDBSession* db = CMDBSession::NewL(KCDVersion1_2);
    CleanupStack::PushL(db);

    TName tsy;
    TUint32 modemId = 0;

    CMDBField<TUint32>* globalSettingsField = new(ELeave) CMDBField<TUint32>(KCDTIdModemPhoneServicesSMS);
    CleanupStack::PushL(globalSettingsField);
    globalSettingsField->SetRecordId(1);
    globalSettingsField->LoadL(*db);
    modemId = *globalSettingsField;
    CleanupStack::PopAndDestroy(globalSettingsField);

    CMDBField<TDesC>* tsyField = new(ELeave) CMDBField<TDesC>(KCDTIdTsyName);
    CleanupStack::PushL(tsyField);
    tsyField->SetRecordId(modemId);
    tsyField->SetMaxLengthL(KMaxTextLength);
    tsyField->LoadL(*db);
    tsy = *tsyField;
    aTsyNameList.AppendL(tsy);

    CleanupStack::PopAndDestroy(2, db); // db, tsyField
    }

void CSmsStackTestServer::LoadTsyL(RTelServer& aServer, RPhone& aPhone, const TDesC& aTsyName)
    {
    INFO_PRINTF2(_L("Using TSY \"%S\"Loading RTelServer..."), &aTsyName);

    TInt ret = aServer.LoadPhoneModule(aTsyName);
    if (ret!=KErrNone)
        {
        ERR_PRINTF2(_L("Loading Phone Module returned %d"), ret);
        User::Leave(ret);
        }
    
    // Find the phone corresponding to this TSY and open a number of handles on it
    TInt numPhones;
    ret = aServer.EnumeratePhones(numPhones);
    if (ret!=KErrNone)
        {
        ERR_PRINTF2(_L("Enumerate Phones  returned %d"), ret);
        User::Leave(ret);
        }
    
    TBool found=EFalse;

    while (numPhones--)
        {
        TName phoneTsy;
        ret = aServer.GetTsyName(numPhones,phoneTsy);
        if (ret!=KErrNone)
            {
            ERR_PRINTF2(_L("GetTsyName returned %d"), ret);
            User::Leave(ret);
            }
        
        if (phoneTsy.CompareF(aTsyName)==KErrNone)
            {
            INFO_PRINTF1(_L("Found RPhone..."));
            found = ETrue;
            RTelServer::TPhoneInfo info;
            ret = aServer.GetPhoneInfo(numPhones,info);
            if (ret!=KErrNone)
                {
                ERR_PRINTF2(_L("GetPhoneInfo returned %d"), ret);
                User::Leave(ret);
                }
            ret = aPhone.Open(aServer,info.iName);
            if (ret!=KErrNone)
                {
                ERR_PRINTF2(_L("Opening phone returned %d"), ret);
                User::Leave(ret);
                }
 
            INFO_PRINTF1(_L("Initializing..."));
            ret = aPhone.Initialise();
            TTimeIntervalMicroSeconds32 InitPause=9000000;  //Required Pause to Allow SMSStack to Complete its Async Init
            User::After(InitPause);                         //call to the TSY and finish its StartUp.
            if (ret!=KErrNone)
                {
                ERR_PRINTF2(_L("Completed Initialize returned %d"), ret);
                User::Leave(ret);
                }
            break;
            }
        }
    
    if(!found)
        {
        ERR_PRINTF2(_L("Couldn't find the phone for TSY %S"), &aTsyName);
        }
    }
