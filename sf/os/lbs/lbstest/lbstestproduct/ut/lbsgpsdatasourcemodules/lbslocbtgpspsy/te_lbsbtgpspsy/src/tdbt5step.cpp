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
// Example CTestStep derived implementation
//



/**
 @file TDBT5Step.cpp
 @internalTechnology
*/
#include "TDBT5Step.h"
#include "Te_LbsBtGpsPsyDefs.h"
#include <lbs.h>

// CONSTANTS

// Requestor name
_LIT(KTD1RequestorName, "BTGPSPSY TD5");
_LIT(KTD1RequestorName2, "BTGPSPSY TD5_2");

//BTGPSPSY PSY UID
const TUid KBtGpsPsyImplUid = 
    {
    0x101FE999
    };


CTDBT5Step::~CTDBT5Step()
/**
 * Destructor
 */
	{
	}

CTDBT5Step::CTDBT5Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KTDBT5Step);
	}

TVerdict CTDBT5Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsBtGpsPsyStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return   TestStepResult();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTDBT5Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TInt test;
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteInt,test)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		SetTestStepResult(EPass);
		StartL(test);
		}
	  return TestStepResult();
	}



TVerdict CTDBT5Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


    
CAsyncWait* CAsyncWait::NewL()
    {
    CAsyncWait* self = new (ELeave) CAsyncWait;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CAsyncWait::CAsyncWait()
    {
    }

CAsyncWait::~CAsyncWait()
    {
    if(iIdle!=NULL)
        {
        iIdle->Cancel();
        }
    delete iIdle;
    delete iWait;
    }
    
void CAsyncWait::ConstructL()
    {
    iIdle = CIdle::NewL(CActive::EPriorityStandard);
    iWait = new (ELeave) CActiveSchedulerWait;
    }

TInt CAsyncWait::IdleCallBack(TAny *aAny)
    {
    return reinterpret_cast<CAsyncWait*>(aAny)->IdleCall();
    }
    
TInt CAsyncWait::IdleCall()
    {
    User::After(K1Second); //wait 1 seconds. Give time for other threads
    if(iStatus->Int()!=KRequestPending)
        {
        //eat the stray event
        User::WaitForRequest(*iStatus);
        
        iWait->AsyncStop();
        return 0;
        }
    return 1;
    }
    
void CAsyncWait::WaitL(TRequestStatus& aStatus)
    {
	//User::WaitForRequest(aStatus);
    CAsyncWait* self = new (ELeave) CAsyncWait;
    CleanupStack::PushL(self);
    self->ConstructL();

    self->iIdle->Start(TCallBack(IdleCallBack, self));
    self->iStatus = &aStatus;
    self->iWait->Start();

    CleanupStack::PopAndDestroy(self);
    }


// ---------------------------------------------------------
// CTDBT5Step::StartL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CTDBT5Step::StartL(const TInt aIndex)
    {
    switch(aIndex)
        {
        case 0:
        	_LIT(KIndexName0, "**** Display fix on screen ****");
        	INFO_PRINTF1(KIndexName0);
        	BasicLocationRequestL();
            break;
        case 1:
        	_LIT(KIndexName1, "**** HGenericInfo test ****");
        	INFO_PRINTF1(KIndexName1);
        	HGenericInfoTestL();
            break;
        case 2:
        	_LIT(KIndexName2, "**** NMEA basic test ****");
        	INFO_PRINTF1(KIndexName2);
        	NMEALogTestL();
            break;
        case 3:
        	_LIT(KIndexName3, "**** NMEA Partial log long ****");
        	INFO_PRINTF1(KIndexName3);
        	NMEALogPartialLongTestL();
            break;
        case 4:
        	_LIT(KIndexName4, "**** NMEA multiple clients ****");
        	INFO_PRINTF1(KIndexName4);
        	NMEALogMultipleClientsL();
            break;
        } 
    }


            
void CTDBT5Step::LocationRequestTestL(TBool aShowResult)
    {
    // connect to server
    RPositionServer posServer;
    User::LeaveIfError(posServer.Connect());
    CleanupClosePushL(posServer);
    
    // open positioner
    RPositioner positioner;
    User::LeaveIfError(positioner.Open(posServer, KBtGpsPsyImplUid));
    CleanupClosePushL(positioner);
    
    // Set requestor
    positioner.SetRequestor(
        CRequestor::ERequestorService, 
        CRequestor::EFormatApplication, 
        KTD1RequestorName);
    
    // make location request
    TRequestStatus reqStatus;
    TPositionInfo posInfo;
    
    positioner.NotifyPositionUpdate(posInfo, reqStatus);
    CAsyncWait::WaitL(reqStatus);
    
    // Check request status and display it on screen
    ShowDisplayResult(reqStatus.Int(), posInfo, aShowResult);
    
    User::LeaveIfError(reqStatus.Int());
    
    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);
    }

// ---------------------------------------------------------
// CTDBT5Step::BasicLocationRequestL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CTDBT5Step::BasicLocationRequestL()
    {
    LocationRequestTestL(ETrue);
	_LIT(KIndexName0, "Ended: **** Display fix on screen ****");
	INFO_PRINTF1(KIndexName0);
    }

void CTDBT5Step::HGenericInfoTestL()
    {
    // connect to server
    RPositionServer posServer;
    User::LeaveIfError(posServer.Connect());
    CleanupClosePushL(posServer);
    
    // open positioner
    RPositioner positioner;
    User::LeaveIfError(positioner.Open(posServer, KBtGpsPsyImplUid));
    CleanupClosePushL(positioner);
    
    // Set requestor
    positioner.SetRequestor(
        CRequestor::ERequestorService, 
        CRequestor::EFormatApplication, 
        KTD1RequestorName);
    
    //_____________________________________________________________
    // make location request
    TRequestStatus reqStatus;
    HPositionGenericInfo* posInfo = HPositionGenericInfo::NewL(2); //12bytes
    CleanupStack::PushL(posInfo);
    posInfo->SetRequestedField(EPositionFieldHorizontalSpeed);
    posInfo->SetRequestedField(EPositionFieldHorizontalSpeedError);
    posInfo->SetRequestedField(EPositionFieldVerticalSpeed);
    posInfo->SetRequestedField(EPositionFieldVerticalSpeedError);
    
    positioner.NotifyPositionUpdate(*posInfo, reqStatus);
    CAsyncWait::WaitL(reqStatus);
    
    // Check request status and display it on screen
    ShowDisplayResult(reqStatus.Int(), *posInfo);
    
    //Client shall not allocate enough buffer to hold the fix
    if(reqStatus.Int()!=KErrPositionBufferOverflow )
        {
        User::Leave(KErrGeneral);
        }
        
    //_____________________________________________________________
    //increase the buffer
    CleanupStack::PopAndDestroy(posInfo);
    posInfo=HPositionGenericInfo::NewL();//4k buffer
    CleanupStack::PushL(posInfo);
    posInfo->SetRequestedField(EPositionFieldHorizontalSpeed);
    posInfo->SetRequestedField(EPositionFieldHorizontalSpeedError);
    posInfo->SetRequestedField(EPositionFieldVerticalSpeed);
    posInfo->SetRequestedField(EPositionFieldVerticalSpeedError);
    
    positioner.NotifyPositionUpdate(*posInfo, reqStatus);
    CAsyncWait::WaitL(reqStatus);
    
    // Check request status and display it on screen
    ShowDisplayResult(reqStatus.Int(), *posInfo);
    
    User::LeaveIfError(reqStatus.Int());


    //_____________________________________________________________
    positioner.NotifyPositionUpdate(*posInfo, reqStatus);
    CAsyncWait::WaitL(reqStatus);
    
    // Check request status and display it on screen
    ShowDisplayResult(reqStatus.Int(), *posInfo);
    
    User::LeaveIfError(reqStatus.Int());
    
    CleanupStack::PopAndDestroy(posInfo);
    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);
	_LIT(KIndexName1, "Ended: **** HGenericInfo test ****");
	INFO_PRINTF1(KIndexName1);
    }

void CTDBT5Step::NMEALogTestL()
    {
    // connect to server
    RPositionServer posServer;
    User::LeaveIfError(posServer.Connect());
    CleanupClosePushL(posServer);
    
    // open positioner
    RPositioner positioner;
    User::LeaveIfError(positioner.Open(posServer, KBtGpsPsyImplUid));
    CleanupClosePushL(positioner);
    
    // Set requestor
    positioner.SetRequestor(
        CRequestor::ERequestorService, 
        CRequestor::EFormatApplication, 
        KTD1RequestorName);
    
    //_____________________________________________________________
    // make location request
    TRequestStatus reqStatus;
    HPositionGenericInfo* posInfo = HPositionGenericInfo::NewL(); //4K
    CleanupStack::PushL(posInfo);
    posInfo->SetRequestedField(EPositionFieldNMEASentences);
    
    positioner.NotifyPositionUpdate(*posInfo, reqStatus);
    CAsyncWait::WaitL(reqStatus);
    
    // Check request status and display it on screen
    ShowDisplayResult(reqStatus.Int(), *posInfo);
    
    User::LeaveIfError(reqStatus.Int());
    
    CleanupStack::PopAndDestroy(posInfo);
    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);
	_LIT(KIndexName2, "Ended: **** NMEA basic test ****");
	INFO_PRINTF1(KIndexName2);
    }

void CTDBT5Step::NMEALogPartialLongTestL()
    {
    // connect to server
    RPositionServer posServer;
    User::LeaveIfError(posServer.Connect());
    CleanupClosePushL(posServer);
    
    // open positioner
    RPositioner positioner;
    User::LeaveIfError(positioner.Open(posServer, KBtGpsPsyImplUid));
    CleanupClosePushL(positioner);
    
    // Set requestor
    positioner.SetRequestor(
        CRequestor::ERequestorService, 
        CRequestor::EFormatApplication, 
        KTD1RequestorName);
        
    // Set options
    TPositionUpdateOptions options;
    options.SetAcceptPartialUpdates(ETrue); //partial update
    options.SetUpdateInterval(5000000); //5 second
    positioner.SetUpdateOptions(options);
    
    //_____________________________________________________________
    // make location request
    TRequestStatus reqStatus;
    HPositionGenericInfo* posInfo = HPositionGenericInfo::NewL(10*1024, 512); //10K, max requested fields
    CleanupStack::PushL(posInfo);
    posInfo->SetRequestedField(EPositionFieldNMEASentences);
    
    HBufC* nmeaDataBuf = HBufC::NewLC(5120);//5k

    for(TInt i=0; i<10; i++)
        {
        positioner.NotifyPositionUpdate(*posInfo, reqStatus);
        CAsyncWait::WaitL(reqStatus);
    
        User::LeaveIfError(reqStatus.Int());
    
        //Log NMEA data
        TPtr nmeaData(nmeaDataBuf->Des());
        GetNmeaData(nmeaData, *posInfo);
        
        LogL(_L("fix: %d"), i);
        INFO_PRINTF1(nmeaData);
        
        }
        
    CleanupStack::PopAndDestroy(nmeaDataBuf);
    
    CleanupStack::PopAndDestroy(posInfo);
    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);
	_LIT(KIndexName3, "Ended: **** NMEA Partial log long ****");
	INFO_PRINTF1(KIndexName3);
    }

void CTDBT5Step::NMEALogMultipleClientsL()
    {
    // connect to server
    RPositionServer posServer;
    User::LeaveIfError(posServer.Connect());
    CleanupClosePushL(posServer);
    
    // open positioner
    RPositioner positioner;
    User::LeaveIfError(positioner.Open(posServer, KBtGpsPsyImplUid));
    CleanupClosePushL(positioner);
    
    // Set requestor
    positioner.SetRequestor(
        CRequestor::ERequestorService, 
        CRequestor::EFormatApplication, 
        KTD1RequestorName);
        
    // Set options
    TPositionUpdateOptions options;
    options.SetAcceptPartialUpdates(ETrue); //partial update
    options.SetUpdateInterval(1000000); //1 second
    positioner.SetUpdateOptions(options);
    

    // Open sencond positioner
    RPositioner positioner2;
    User::LeaveIfError(positioner2.Open(posServer, KBtGpsPsyImplUid));
    CleanupClosePushL(positioner2);
    
    // Set requestor
    positioner2.SetRequestor(
        CRequestor::ERequestorService, 
        CRequestor::EFormatApplication, 
        KTD1RequestorName2);
        
    // Set options
    options.SetUpdateInterval(1000000); //1 second
    positioner2.SetUpdateOptions(options);

    //_____________________________________________________________
    // make location request
    TRequestStatus reqStatus;
    HPositionGenericInfo* posInfo = HPositionGenericInfo::NewL(10*1024, 512); //10K, max requested fields
    CleanupStack::PushL(posInfo);
    posInfo->SetRequestedField(EPositionFieldNMEASentences);
    
    TRequestStatus reqStatus2;
    HPositionGenericInfo* posInfo2 = HPositionGenericInfo::NewL(10*1024, 512); //10K, max requested fields
    CleanupStack::PushL(posInfo2);
    posInfo2->SetRequestedField(EPositionFieldNMEASentences);


    HBufC* nmeaDataBuf = HBufC::NewLC(5120);//5k

    for(TInt i=0; i<10; i++)
        {
        //First positioner
        positioner.NotifyPositionUpdate(*posInfo, reqStatus);
        CAsyncWait::WaitL(reqStatus);
    
        User::LeaveIfError(reqStatus.Int());
    
        //Log NMEA data
        TPtr nmeaData(nmeaDataBuf->Des());
        GetNmeaData(nmeaData, *posInfo);
        
        LogL(_L("Positioner1 fix: %d"), i);
        INFO_PRINTF1(nmeaData);
        
        //Second positioner
        positioner2.NotifyPositionUpdate(*posInfo2, reqStatus2);
        CAsyncWait::WaitL(reqStatus2);
        User::LeaveIfError(reqStatus2.Int());
    
        //Log NMEA data
        GetNmeaData(nmeaData, *posInfo2);
        
        LogL(_L("Positioner2 fix: %d"), i);
        INFO_PRINTF1(nmeaData);
        
        }
        
    CleanupStack::PopAndDestroy(nmeaDataBuf);
    
    CleanupStack::PopAndDestroy(posInfo2);
    CleanupStack::PopAndDestroy(posInfo);
    CleanupStack::PopAndDestroy(&positioner2);
    CleanupStack::PopAndDestroy(&positioner);
    CleanupStack::PopAndDestroy(&posServer);
	_LIT(KIndexName4, "Ended: **** NMEA multiple clients ****");
	INFO_PRINTF1(KIndexName4);
    }



void CTDBT5Step::LogL(TRefByValue<const TDesC> aMsg, ...)
    {
    VA_LIST list;
    VA_START(list, aMsg);
    TBuf<512> msg;
    msg.FormatList(aMsg, list);
    
    INFO_PRINTF1(msg);
    }

// Get NMEA data from HGenericPositionInfo
void CTDBT5Step::GetNmeaData(TDes&aDes, const TPositionInfo& aPosInfo)
    {
    aDes.Zero();
    
    TInt classType = aPosInfo.PositionClassType();
    if (!(classType & EPositionGenericInfoClass))
        {
        return;
        }

    const HPositionGenericInfo* genInfo =
        reinterpret_cast<const HPositionGenericInfo*> ( &aPosInfo );
        
        
    if(genInfo->IsFieldAvailable(EPositionFieldNMEASentences))
        {
        TUint8 numOfSentences;
        if(genInfo->GetValue(EPositionFieldNMEASentences, numOfSentences)==KErrNone)
            {
            for(TInt i=0; i<numOfSentences; i++)
                {
                TBuf8<128> buf1;
                if(genInfo->GetValue(EPositionFieldNMEASentences+i+1, buf1)==KErrNone)
                    {
                    TBuf<128> buf2;
                    buf2.Copy(buf1);
                    aDes.Append(buf2);
                    }
                }
            }
        }
    }

// Format latitude, longitude and horizontal accuracy to given
// descriptor.
//
void CTDBT5Step::FormatCoordinates(TDes& aDes, const TPositionInfo& aPosInfo)
    {
    TPosition position;

    aPosInfo.GetPosition(position);

    // Add latitude to buffer
    TReal64 tmp = position.Latitude();

    aDes.Append(_L("Latitude: "));

    FormatToDegrees(aDes, tmp);

    aDes.Append(_L(" "));

    // Add longitude to buffer

    tmp = position.Longitude();

    aDes.Append(_L("\nLongitude: "));

    FormatToDegrees(aDes, tmp);
    
    // Add altitude
    
    tmp = position.Altitude();

    aDes.AppendFormat(_L("\r\nAltitude: %f\r\n"), tmp);

    // Add Horizontal accuracy to buffer
    aDes.AppendFormat(_L("Horizontal accuracy: %f\r\n"), position.HorizontalAccuracy());
    
    // Add Vertical accuracy to buffer
    aDes.AppendFormat(_L("Vertical accuracy: %f\r\n"), position.VerticalAccuracy());
    
    //Check if it's HPositionGenericInfo
    TInt classType = aPosInfo.PositionClassType();
    if (classType & EPositionGenericInfoClass)
        {
        // HPositionGenericInfo
        const HPositionGenericInfo* genInfo =
            reinterpret_cast<const HPositionGenericInfo*> ( &aPosInfo );
        
        TInt fieldId = genInfo->FirstRequestedFieldId();
        while(fieldId!=EPositionFieldNone)
            {
            if(genInfo->IsFieldAvailable(fieldId))
                {
                if(fieldId!=EPositionFieldNMEASentences)
                    {
                    TReal32 real;
                    if(genInfo->GetValue(fieldId, real) == KErrNone)
                        {
                        aDes.AppendFormat(_L("FieldId:%d   Value:%f\n"), fieldId, real);
                        }
                    }
                }
            fieldId = genInfo->NextRequestedFieldId(fieldId);
            }
        GetNmeaData(aDes, *genInfo);
        }
    }


// Format TReal64 to TDes, i.e. display the coordinates
// in HH'MM'SS form. For example:
// 61.119977 --> 61'7'11.916701
//
void CTDBT5Step::FormatToDegrees(TDes& aDes, TReal64 aCoordinate)
    {
    TInt tmp = TInt(aCoordinate);

    aDes.AppendNum(tmp);
    aDes.Append(_L("'"));

    
    TReal64 minutes = ((aCoordinate -tmp) *60.0);

    tmp = TInt(minutes);

    aDes.AppendNum(tmp);
    aDes.Append(_L("'"));

    TReal64 seconds = ((minutes -tmp) *60.0);

    aDes.AppendFormat(_L("%f"), seconds);
    }

void CTDBT5Step::ShowDisplayResult(
            TInt aErr, 
            TPositionInfo& aPosInfo,
            TBool aShowDialog)
    {
    TBuf<1024> message;
    if (aErr == KErrNone)
        {
        INFO_PRINTF1(_L("Location request succeedeed."));
        FormatCoordinates(message, aPosInfo);

        INFO_PRINTF1(KNullDesC);
        INFO_PRINTF1(message);
        INFO_PRINTF1(KNullDesC);

        if(aShowDialog)
            {
            //iUserInfo->ShowDialog(message, EUtfwDialogTypeOk, EFalse);
            }
        
        }
    else
        {
        message.Format(_L("ERROR: Location request failed with error code %d."), aErr);
        INFO_PRINTF1(message);

        if(aShowDialog)
            {
            //iUserInfo->ShowDialog(message, EUtfwDialogTypeOk, EFalse);      
            }
        }    

    }


void CTDBT5Step::LogL(const TDesC& aLog)
    {
    INFO_PRINTF1(aLog);
    }

// End of File


