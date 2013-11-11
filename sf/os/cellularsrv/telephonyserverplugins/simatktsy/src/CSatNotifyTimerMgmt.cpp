// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : CSatNotifyTimerMgmt.cpp
// Part of     : Common SIM ATK TSY / commonsimatktsy
// Timer management notification functionality of Sat Tsy
// Version     : 1.0
//



//INCLUDES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSatNotifyTimerMgmtTraces.h"
#endif

#include <satcs.h>                  // Etel SAT IPC definitions
#include "CSatTsy.h"                // Tsy class header
#include "CSatNotifyTimerMgmt.h"    // Class header
#include "CSatNotificationsTsy.h"   // Tsy class header
#include "CBerTlv.h"                // Ber Tlv data handling
#include "TTlv.h"					// TTlv class
#include "CSatDataPackage.h"        // Parameter packing 
#include "TSatUtility.h"            // Utilities
#include "cmmmessagemanagerbase.h"  // Message manager class for forwarding req.
#include "MSatTsy_IPCDefs.h"        // Sat Tsy internal request types


// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//  
CSatNotifyTimerMgmt* CSatNotifyTimerMgmt::NewL
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_NEWL_1, "CSAT: CSatNotifyTimerMgmt::NewL");
   	CSatNotifyTimerMgmt* const satNotifyTimerMgmt = 
        new ( ELeave ) CSatNotifyTimerMgmt( aNotificationsTsy );
    CleanupStack::PushL( satNotifyTimerMgmt );
    satNotifyTimerMgmt->ConstructL();
    CleanupStack::Pop( satNotifyTimerMgmt );
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_NEWL_2, "CSAT: CSatNotifyTimerMgmt::NewL, end of method");
    return satNotifyTimerMgmt;
    }

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::~CSatNotifyTimerMgmt
// Destructor
// -----------------------------------------------------------------------------
//  
CSatNotifyTimerMgmt::~CSatNotifyTimerMgmt
        ( 
		// None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_DTOR_1, "CSAT: CSatNotifyTimerMgmt::~CSatNotifyTimerMgmt");
    }
    
// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::CSatNotifyTimerMgmt
// Default C++ constructor
// -----------------------------------------------------------------------------
//  
CSatNotifyTimerMgmt::CSatNotifyTimerMgmt
        ( 
        CSatNotificationsTsy* aNotificationsTsy 
        ) : iNotificationsTsy ( aNotificationsTsy )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//  
void CSatNotifyTimerMgmt::ConstructL
        (
        // None
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_CONSTRUCTL_1, "CSAT: CSatNotifyTimerMgmt::ConstructL, does nothing");
    }

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::CompleteNotifyL
// This method completes a NotifyTimerMgmt request by sending terminal response. 
// -----------------------------------------------------------------------------
//
TInt CSatNotifyTimerMgmt::CompleteNotifyL
        (
        CSatDataPackage* aDataPackage,   
        TInt aErrorCode                  
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_COMPLETENOTIFYL_1, "CSAT: CSatNotifyTimerMgmt::CompleteNotifyL");

    TInt ret( KErrNone );
    TInt returnValue( KErrNone );
    TUint8 generalResult ( RSat::KSuccess );
	
    // Unpack parameters
    TPtrC8* data;
    aDataPackage->UnPackData( &data );
    // Get ber tlv 
    CBerTlv berTlv;
    berTlv.SetData( *data );

    // Get command details tlv
    CTlv commandDetails;
    berTlv.TlvByTagValue( &commandDetails, KTlvCommandDetailsTag );
    // Store command details tlv
    iNotificationsTsy->iTerminalRespData.iCommandDetails.Copy( 
        commandDetails.Data() );

    // Get command qualifier
    TUint8 cmdQualifier( commandDetails.GetShortInfo( 
        ETLV_CommandQualifier ) );
    
    TUint8 pCmdNumber( commandDetails.GetShortInfo( ETLV_CommandNumber ) );
       
    // Don't do anything if error has occured, otherwise continue..
    if ( KErrNone == aErrorCode )
        {
        // Get timer id tlv
        CTlv timerId;
        returnValue = berTlv.TlvByTagValue( &timerId, KTlvTimerIdentifierTag );

        // Initalize
        iTimerId = 0;
        for( TInt i = 0; i < 3; i++ )
            {
            iTimerValue[i] = 0;
            }

        if ( KErrNone == returnValue )
            {
            iTimerId = timerId.GetShortInfo( ETLV_TimerIdentifier );
            // The Timer Identifier can take 8 different values 
            if ( ( RSat::KTimer8 < iTimerId  ) 
                 || ( RSat::KTimer1 > iTimerId ) )
                {
                generalResult = RSat::KCmdDataNotUnderstood;
                }
            else
                {
                // if operation type is startTimer = 0x00;
                if ( !cmdQualifier ) 
                    {
                     // Get timer value tlv
                    CTlv timerValue;
                    returnValue = berTlv.TlvByTagValue( &timerValue, 
                        KTlvTimerValueTag );

                    if ( KErrNotFound != returnValue )
                        {
                        TPtrC8 temp;
                        TUint32 timeValue( 0 );

                        // Value of a timer, expressed using 
                        // the format hour, minute, second
                        // Semi-octec presentation used (23.040)
                        temp.Set( timerValue.GetData( ETLV_TimerValue ) );
                
                        // Convert to seconds
                        timeValue = ConvertToSeconds( temp );

                        // The possible duration of a timer is 
                        // between 1 second and 24 hours
                        if ( KMinTimerValue < timeValue && 
                             KMaxTimerValueInSeconds >= timeValue )
                            {
                            ret = iNotificationsTsy->GetSatTimer()->Start( 
                                iTimerId, timeValue );

                            if ( KErrNone != ret )
                                {
                                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_COMPLETENOTIFYL_2, "CSAT: CSatNotifyTimerMgmt::CompleteNotifyL, Unable to process cmd");
                                generalResult = RSat::KMeUnableToProcessCmd;
                                }
                            }
                        else
                            {
                            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_COMPLETENOTIFYL_3, "CSAT: CSatNotifyTimerMgmt::CompleteNotifyL, Data not understood");
                            generalResult = RSat::KCmdDataNotUnderstood;
                            }
                        }
                    else
                        {
                        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_COMPLETENOTIFYL_4, "CSAT: CSatNotifyTimerMgmt::CompleteNotifyL, Timer value missing");
                        generalResult = RSat::KErrorRequiredValuesMissing;
                        }
                    }
                // If operation type is KDeactivateTimer or KGetTimerValue
                else if ( ( KDeactivateTimer == cmdQualifier ) ||
                          ( KGetTimerValue == cmdQualifier ) )
                    {
                    TInt retValue( KErrNone );

                    // Get current value of the timer 
                    ret = iNotificationsTsy->GetSatTimer()->
                        CurrentValueOfTimerById( iTimerId );
            
                    if ( ( KErrNotFound != ret ) && ( KDeactivateTimer == 
                    		cmdQualifier ) )
                        {
                        // Deactivate timer
                        retValue = iNotificationsTsy->GetSatTimer()->
                            DeleteTimerById( iTimerId );
                        }

                    // If timer is found convert timer value to semi-octets
                    if ( ( KErrNotFound != ret ) && ( KErrNotFound != 
                    	retValue ) )
                        {
                        generalResult = RSat::KSuccess;

                        TInt num[3];
                        num[0] = ret/3600;         //hours
                        num[1] = ( ret%3600 )/60;  //minutes
                        num[2] = ( ret%3600 )%60;  //seconds
                
                        TInt i( 0 );
                        for ( i = 0; i < 3; i++ )
                            { 
                            //convert to semi-octet
                            iTimerValue[i] = 
                                TSatUtility::ConvertToSemiOctet( num[i] );
                            }
                        }
                    else
                        {
                        OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_COMPLETENOTIFYL_5, "CSAT: CSatNotifyTimerMgmt::CompleteNotifyL, Contradiction with timer state");
                        generalResult = RSat::KContradictionWithTimerState;
                        }
                    }
                else
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_COMPLETENOTIFYL_6, "CSAT: CSatNotifyTimerMgmt::CompleteNotifyL, Cmd type not understood");
                    generalResult = RSat::KCmdTypeNotUnderstood;
                    }
                }
            }
        else
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_COMPLETENOTIFYL_7, "CSAT: CSatNotifyTimerMgmt::CompleteNotifyL, Required values missing");
            generalResult = RSat::KErrorRequiredValuesMissing;
            }

        CreateTerminalRespL( pCmdNumber, generalResult );  

        } // End of if (KErrNone)
 
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::CreateTerminalRespL
// Constructs LocalInfo specific part of terminal response and calls 
// DOS to send the actual message.
// -----------------------------------------------------------------------------
//
TInt CSatNotifyTimerMgmt::CreateTerminalRespL
        ( 
        TUint8 aPCmdNumber,             
        TUint8 aGeneralResult	
		)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_CREATETERMINALRESPL_1, "CSAT: CSatNotifyTimerMgmt::CreateTerminalRespL");

    // Create and append response data
    TTlv tlvSpecificData;
    // Create General Result TLV here
    tlvSpecificData.AddTag( KTlvResultTag );

    // General result
    tlvSpecificData.AddByte( aGeneralResult );

    // Include timer value and id only when command is performed successfully
    if( RSat::KSuccess == aGeneralResult )
        {
        // Timer Id
        // Value KTlvTimerIdentifierTagCRBitOn ( 0xA4 ) is added,
        // because used KTlvTimerIdentifierTag ( 0x24 ) doesn't
        // include comprehension bit in tag the value and it is expected
        // to be present in indications and terminal responses.
        tlvSpecificData.AddTag( KTlvTimerIdentifierTagCRBitOn );
        
        tlvSpecificData.AddByte( iTimerId );

        // Timer value is included when the Command Qualifier is indicating
        // "deactivate" or "get the current value of the timer"
        if ( iNotificationsTsy->iTerminalRespData.iCommandDetails[4] )
            {
            tlvSpecificData.AddTag( KTlvTimerValueTag );
            // Timer value in semi-octets
            tlvSpecificData.AddByte( iTimerValue[0] ); // Hours
            tlvSpecificData.AddByte( iTimerValue[1] ); // Minutes
            tlvSpecificData.AddByte( iTimerValue[2] ); // Seconds
            }
        }
    else if( ( RSat::KContradictionWithTimerState != aGeneralResult ) &&
             ( RSat::KErrorRequiredValuesMissing != aGeneralResult ) &&
             ( RSat::KCmdDataNotUnderstood != aGeneralResult ) &&
             ( RSat::KCmdTypeNotUnderstood != aGeneralResult ) )
        {
        tlvSpecificData.AddByte( KNoCause );
        }
	else
        {
        // None.
        }
    
    // Prepare data
    iNotificationsTsy->iTerminalRespData.iPCmdNumber = aPCmdNumber;
    TPtrC8 data = tlvSpecificData.GetDataWithoutTopLevelTag();

    // Pack data
    CSatDataPackage dataPackage;
	dataPackage.PackData( &iNotificationsTsy->iTerminalRespData, &data );

    // Forward request to the DOS
    return iNotificationsTsy->iSatTsy->MessageManager()->HandleRequestL( 
		ESatTerminalRsp, &dataPackage );
    }

// -----------------------------------------------------------------------------
// CSatNotifyTimerMgmt::ConvertToSeconds
// Converts to seconds semi-octet presented value of timer, expressed using 
// the format hour, minute, second
// -----------------------------------------------------------------------------
//
TUint32 CSatNotifyTimerMgmt::ConvertToSeconds
         (
         TPtrC8 time
         )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSATNOTIFYTIMERMGMT_CONVERTTOSECONDS_1, "CSAT: CSatNotifyTimerMgmt::ConvertToSeconds");
    TUint32 timeValue( 0 );
    // Value of a timer, expressed using 
    // the format hour, minute, second
    // Semi-octec presentation used (23.040)
    // time[0] is hours
    // time[1] is minutes
    // time[2] is seconds
     TInt i( 0 );
    for ( i = 0; i < 3; i++ )
        {
        TUint value = ( time[i] & 0x0F )*10 + ( ( time[i] & 0xF0 ) >> 4 );
        timeValue = timeValue * 60 + value;
        }
    return timeValue;
    }

// End of file









