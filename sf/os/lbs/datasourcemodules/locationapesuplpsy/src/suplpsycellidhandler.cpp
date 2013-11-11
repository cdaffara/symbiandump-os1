/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Cell Id implementaion in SUPL PSY.
*
*/


// INCLUDE FILES

#include <centralrepository.h> 
#include <commdb.h>

#include "suplpsylogging.h"
#include "suplpsycellidhandler.h"


// ============================ MEMBER FUNCTIONS OF CSuplPsyCellIdHandler ===============================

// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::CSuplPsyCellIdHandler,constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

CSuplPsyCellIdHandler::CSuplPsyCellIdHandler( MBasicCellIDListener& aObserver ) 
					    : CActive( EPriorityStandard ),
					      iNetworkInfoPkg( iNetworkInfo ),
					      iObserver( aObserver ),
					      iNetworkType( EUnknown ),
					      iCellIdInfo( NULL )
	{
	CActiveScheduler::Add( this );            
	}

// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::ConstructL,two phase constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSuplPsyCellIdHandler::ConstructL()
    {
    TRACESTRING( "CSuplPsyCellIdHandler::ConstructL start" )
    iIsInitilized = EFalse;
    User::LeaveIfError( ConnectToETelL() );
    iIsInitilized = ETrue;	
    TRACESTRING( "CSuplPsyCellIdHandler::ConstructL end" )
    }

// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::NewL,two phase constuctor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSuplPsyCellIdHandler* CSuplPsyCellIdHandler::NewL( MBasicCellIDListener& aObserver )
    {
    CSuplPsyCellIdHandler* self = new( ELeave ) CSuplPsyCellIdHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSuplPsyCellIdHandler::DoCancel()
    {
	TRACESTRING( "CSuplPsyCellIdHandler::DoCancel start" )
	iPhone.CancelAsyncRequest( EMobilePhoneGetCurrentNetwork ); 
	TRACESTRING( "CSuplPsyCellIdHandler::DoCancel end" )           
    }            

// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::~CSuplPsyCellIdHandler,Destructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSuplPsyCellIdHandler::~CSuplPsyCellIdHandler()
    {
	TRACESTRING( "CSuplPsyCellIdHandler::~CSuplPsyCellIdHandler start" )
	Cancel();
	iPhone.Close(); 
	if ( iIsInitilized )
		{
		iTelServer.UnloadPhoneModule(iTsyName);	
		}
			
	iTelServer.Close();
	
	TRACESTRING( "CSuplPsyCellIdHandler::~CSuplPsyCellIdHandler end" )
    }
            
// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::GetCellID,For retriving the cell id information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSuplPsyCellIdHandler::GetCellID( TCellIdInfo& aCellIdInfo )
    {
	TRACESTRING("CSuplPsyCellIdHandler::GetCellID start")
	iCellIdInfo = &aCellIdInfo;
	iPhone.GetCurrentNetwork( iStatus, iNetworkInfoPkg, iLocationInfo );
	SetActive();            
    TRACESTRING("CSuplPsyCellIdHandler::GetCellID end")
    }  
      

// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::NetworkType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSuplPsyCellIdHandler::TNetworkTypeInfo CSuplPsyCellIdHandler::NetworkType() const
	{
	return iNetworkType;
	}

// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSuplPsyCellIdHandler::RunL()
    {
	TRACESTRING("CSuplPsyCellIdHandler::RunL start")
    
    TInt err = iStatus.Int();
    TRACESTRING2("Request Completed with Error Code: %d",err )
    	
	if ( err == KErrNone )
		{       
		HandleCellInfoL();
		}
	else
		{
		iObserver.BasicCellIDRequestCompletedL( KErrNotFound );    
		}
    TRACESTRING("CSuplPsyCellIdHandler::RunL end")
    }

// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::HandleCellInfoL(),
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSuplPsyCellIdHandler::HandleCellInfoL()
	{
    TRACESTRING("CSuplPsyCellIdHandler::HandleCellInfoL start")
    
        
    switch (iNetworkInfo.iMode) 
    	{
        case RMobilePhone::ENetworkModeGsm:
        	{
        	TRACESTRING("GSM Network")
        	//For GSM Cell Information
			iNetworkType = EGSM;
			
			TInt    MCC=0;
			TInt64  MNC=0;
			TLex LexMCC(iNetworkInfo.iCountryCode);
			TLex LexMNC(iNetworkInfo.iNetworkId);

			LexMCC.Val(MCC);
			LexMNC.Val(MNC);
			TUint   LAC = iLocationInfo.iLocationAreaCode;
			TUint   CellId = iLocationInfo.iCellId;
					
			iCellIdInfo->iMCC = MCC;
			iCellIdInfo->iMNC = MNC;
			iCellIdInfo->iCid = CellId;
			iCellIdInfo->iLac = LAC;
								 
            TRACESTRING("Observers notified for GSM CellId information")
            iObserver.BasicCellIDRequestCompletedL(KErrNone);    
            break;                             
            }                    
        case RMobilePhone::ENetworkModeWcdma:
        	{
			TRACESTRING("WCDMA Network")
			//For WCDMA Cell Information
			iNetworkType = EWCDMA;
			
			TInt    MCC=0;
			TInt64  MNC=0;
			TLex LexMCC(iNetworkInfo.iCountryCode);
			TLex LexMNC(iNetworkInfo.iNetworkId);
			LexMCC.Val(MCC);
			LexMNC.Val(MNC);
			TUint  CellId = iLocationInfo.iCellId;
								
			iCellIdInfo->iMNC = MNC;
			iCellIdInfo->iMCC = MCC;
			iCellIdInfo->iCid = CellId;
														
	        TRACESTRING("Observers notified for WCDMA CellId information")
	        iObserver.BasicCellIDRequestCompletedL(KErrNone);    
            break;                             
            }                    
        default:            
        	{
                 
#ifdef _WINS_ 
            
            iNetworkType = EGSM;
			
			TInt    MCC=0;
			TInt64  MNC=0;
			TLex LexMCC(iNetworkInfo.iCountryCode);
			TLex LexMNC(iNetworkInfo.iNetworkId);

			LexMCC.Val(MCC);
			LexMNC.Val(MNC);
			TUint   LAC = iLocationInfo.iLocationAreaCode;
			TUint   CellId = iLocationInfo.iCellId;
					
			iCellIdInfo->iMCC = MCC;
			iCellIdInfo->iMNC = MNC;
			iCellIdInfo->iCid = CellId;
			iCellIdInfo->iLac = LAC;
								 
            TRACESTRING("Observers notified for GSM CellId information")
            iObserver.BasicCellIDRequestCompletedL(KErrNone);
#else             
            
            TRACESTRING("Unknown Network")
            iNetworkType = EUnknown;
            
            TRACESTRING("Observers notified with error for unknown network")
            iObserver.BasicCellIDRequestCompletedL(KErrNotFound);    
#endif	        
	        break;        	
            }                     
	    }
	TRACESTRING("CSuplPsyCellIdHandler::HandleCellInfoL end")
	}

// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::RunError, Will be called by framework,if RunL leaves.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSuplPsyCellIdHandler::RunError(TInt /*aError*/)
	{
   	TRAP_IGNORE(iObserver.BasicCellIDRequestCompletedL(KErrNotFound));    
	return KErrNone;
	}
	
// -----------------------------------------------------------------------------
// CSuplPsyCellIdHandler::ConnectToETelL
// Connects to Telephony server & RMobilePhone
// Returns System wide error,if any occurs
// -----------------------------------------------------------------------------
//
TInt CSuplPsyCellIdHandler::ConnectToETelL()
	{
	TInt err = iTelServer.Connect(); 
	if(err != KErrNone)
		{
	    TRACESTRING2("Failed to connect with Error Code: %d",err )
        return err;
		}

	CCommsDatabase* const db = CCommsDatabase::NewL(EDatabaseTypeUnspecified); 
	CleanupStack::PushL(db); 

	TUint32 modemId = 0; 
	db->GetGlobalSettingL(TPtrC(MODEM_PHONE_SERVICES_SMS), modemId); 
	CCommsDbTableView* const view = db->OpenViewMatchingUintLC(TPtrC(MODEM), TPtrC(COMMDB_ID), 
																modemId); 
	err = view->GotoFirstRecord(); 
	if(err != KErrNone)
		{
		CleanupStack::PopAndDestroy(2,db);			
		return err;	
		}
	
	view->ReadTextL(TPtrC(MODEM_TSY_NAME), iTsyName); 
	err = iTelServer.LoadPhoneModule(iTsyName);
	if(err != KErrNone)
		{
		CleanupStack::PopAndDestroy(2,db);		
		return err;
		}
		

	// For the phone information
	RTelServer::TPhoneInfo info;

	iTelServer.GetPhoneInfo(0, info); 
	err = iPhone.Open(iTelServer, info.iName);
	CleanupStack::PopAndDestroy(2,db);					
	if(err != KErrNone)
		{
		return err;	
		}
	return err;
	}	
//  End of File  
