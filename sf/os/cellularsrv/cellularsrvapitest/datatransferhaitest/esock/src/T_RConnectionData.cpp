/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


#include "T_RConnectionData.h"
#include "T_RSocketServData.h"
#include <in_sock.h>

/*@{*/
//LIT's for reading parameters from ini
_LIT( KRSocketServ,		"RSocketServ");
_LIT( KIAPName,      	"IAP" );
_LIT( KRetries,      	"Retries" );
/*@}*/

/*@{*/
//LIT's for Commands
_LIT(KCmdOpen,				"Open");
_LIT(KCmdStart,				"Start");
_LIT(KCmdStop,				"Stop");
_LIT(KCmdClose,				"Close");
_LIT(KCmdSelectOutgoingIAP,	"SelectOutgoingIAP");
/*@}*/


CT_RConnectionData* CT_RConnectionData::NewL()
	{
	CT_RConnectionData * ret = new (ELeave)CT_RConnectionData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CT_RConnectionData::~CT_RConnectionData()
	{
		
	if (iCommDbConnPref)
		{
		delete iCommDbConnPref;
		iCommDbConnPref = NULL;
		}
	if (iConnection)
		{
		delete iConnection;
		iConnection = NULL;
		}
	}

CT_RConnectionData::CT_RConnectionData()
:	iConnection(NULL),
	iCommDbConnPref(NULL),
	iIAP(0)
	{
	
	}
void CT_RConnectionData::ConstructL()
	{
	iConnection = new (ELeave)RConnection();
	}

void CT_RConnectionData::RunL(CActive* aActive, TInt aIndex)
    {
    INFO_PRINTF1(_L("*START* CT_RSocketData::RunL"));
    DecOutstanding(); // One of the async calls has completed 
    
	if(aActive == iActiveCallback)
		{
		INFO_PRINTF1(_L("Asynchronous task has completed. RunL  called"));
		}
	else
		{ 
		ERR_PRINTF1(_L("Stray RunL signal"));
		TInt err = aActive->iStatus.Int(); 
		if( err != KErrNone )
			{
	        ERR_PRINTF2(_L("RunL Error %d"), err); 
	        SetAsyncError( aIndex, err );
			}
		}
	INFO_PRINTF1(_L("*END* CT_RSocketData::RunL"));
	}
    

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RConnectionData::GetObject()
	{
	 return iConnection;
	}


/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 *
 */
TBool CT_RConnectionData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;
	if(aCommand == KCmdOpen)
		{
		DoCmdOpenL(aSection);
		}
	else if(aCommand == KCmdStart)
		{
		DoCmdStart(aSection);
		}
	else if ( aCommand == KCmdStop)
		{
		DoCmdStop();
		}
	else if(aCommand == KCmdClose)
		{
		DoCmdClose();
		}
	else if (aCommand == KCmdSelectOutgoingIAP)
		{
		DoCmdSelectOutgoingIAPL(aSection);		
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret= EFalse;
		}
	
	return ret;
	}

/**
 * 	Opens a RConnection to connect to an Internet Access Point.
 * 	@param aSection - The section in config file to look for the Access Point name and RSocketServ  
 */
void CT_RConnectionData::DoCmdOpenL(const TTEFSectionName& aSection)
    {
    INFO_PRINTF1(_L("*START* CT_RConnectionData::DoCmdOpenL"));
    
    TBool dataOk = ETrue;
    
    TPtrC accessPointName;    
    if(!GetStringFromConfig(aSection, KIAPName, accessPointName))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KIAPName);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    else
    	{
    	INFO_PRINTF2(_L("Connect to IAP \"%S\""), &accessPointName);
        }
        
    TPtrC socketServName;
    // Open RConnection for CommsDat IAP selection and connection information reading.
	if(!GetStringFromConfig(aSection, KRSocketServ, socketServName))
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRSocketServ);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
		}
	
    TInt error = KErrNone;
    
    if (dataOk)
    	{
    	RSocketServ* socketServ=static_cast<RSocketServ*>(GetDataObjectL(socketServName));
    	error = iConnection->Open(*socketServ, KAfInet);
    	if(error == KErrNone)
    		{
    		INFO_PRINTF1(_L("RConnection was opened successfully"));		
    		}
    	else
    		{
    		ERR_PRINTF2(_L("Failed to open RConnection with error [%d]"),error);
    		SetError(error);
    		}
    	}
    INFO_PRINTF1(_L("*END* CT_RConnectionData::DoCmdOpenL"));
	}

/**
 *	Selects an outgoing Internet Access Point.
 * 	@param aSection - Section from config file to read the IAP's name.
 */
void CT_RConnectionData::DoCmdSelectOutgoingIAPL(const TTEFSectionName& aSection)
	{
    INFO_PRINTF1(_L("*START* CT_RConnectionData::DoCmdSelectOutgoingIAPL"));

    TBool dataOk = ETrue;
    
	TPtrC accessPointName;    
    if(!GetStringFromConfig(aSection, KIAPName, accessPointName))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KIAPName);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
    
    TInt error = KErrNone;
    
    if (dataOk)
    	{
    	// Search for IAP from CommsDat.
    	TRAP(error, iCommDbConnPref = SelectOutgoingIAPL(accessPointName))
		if (error != KErrNone)
			{
			ERR_PRINTF2(_L("Failed to select IAP [%d]"), error);
			SetError(error);
			}
    	}	
    INFO_PRINTF1(_L("*END* CT_RConnectionData::DoCmdSelectOutgoingIAPL"));
	}

/**
 * 	Starts RConnection using CActiveCallback.
 * 
 * 	@param aSection 		-	Section in config file to look for number of retries.
 * 
 */
void CT_RConnectionData::DoCmdStart(const TTEFSectionName& aSection)
	{
    INFO_PRINTF1(_L("*START* CT_RConnectionData::DoCmdStart"));
	
    TBool dataOk = ETrue;
    
	TInt retries = 1;
    if(!GetIntFromConfig(aSection, KRetries, retries ))
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRetries);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}
	// If IAP was found, start RConnection with that IAPs preferences (ID and No dialog shown)
	// if no IAP was found return with error code.
	if (dataOk && iCommDbConnPref)
		{
		INFO_PRINTF2(_L("Connecting to the IAP with [%d] tries"), retries);

		// Wait before the connection is really made, this is because
		// the DOS seems to need some time to get ready for data calls.
		// Wait time is 8 seconds.
		const TInt KTimeBeforeConnection = 8000000;
        // In future releases this should be checked on time to time
        // Heavy testing is needed before this pause can be changed.
		User::After(KTimeBeforeConnection);

		TRequestStatus status;

        // Try connection more than once to avoid operator problems
		INFO_PRINTF1(_L("Try connection more than once to avoid operator problems"));
		TInt i = 0;
        do	{
            iConnection->Start(*iCommDbConnPref,status);
            INFO_PRINTF2(_L("Status = [%d]"),status.Int());
            User::WaitForRequest(status);
    	    }
        while(status.Int() != KErrNone && i++ < retries);

		}    
	
    INFO_PRINTF1(_L("*END* CT_RConnectionData::DoCmdStart"));
    }



/**
 * Stops RConnection.
 * Sets TEF error if not successful.
 */
void CT_RConnectionData::DoCmdStop()
    {
    INFO_PRINTF1(_L("*START* CT_RConnectionData::DoCmdStop"));

    TInt error = iConnection->Stop(RConnection::EStopNormal);
    if (error != KErrNone)
        {
        ERR_PRINTF2(_L("Failed to stop RConnection [%d]"), error);
        SetError(error);
        }
    
    INFO_PRINTF1(_L("*END* CT_RConnectionData::DoCmdStop"));
    }

/**
 * Closes RConnection.
 * Sets TEF error if not successful.
 */
void CT_RConnectionData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START* CT_RConnectionData::DoCmdClose"));
	
	iConnection->Close();
	
	INFO_PRINTF1(_L("RConnection is closed"));

	INFO_PRINTF1(_L("*END* CT_RConnectionData::DoCmdClose"));
	}


/**
 *  Select the outgoing IAP
 * 	@param TDesC IAP name
 * 	@return Pointer to TCommDbConnPref
 */
TCommDbConnPref* CT_RConnectionData::SelectOutgoingIAPL(const TDesC& aIAPName)
	{
	const TUint32 KIAPMask = 0xffffffff;
	
	INFO_PRINTF1(_L("Connecting to CommsDat. This will leave with KErrNotSupported if it does not exist"));
	
	// Open a connection to the CommsDat.
	CCommsDatabase* commsDat = CCommsDatabase::NewL(ETrue);
	CleanupStack::PushL(commsDat);
	
	INFO_PRINTF1(_L("Connected to CommsDat"));
	INFO_PRINTF1(_L("Searching for IAPs"));
	
	// Open a view from CommsDat IAP table listing all outgoing IAPs.
	CCommsDbTableView* searchView = commsDat->OpenIAPTableViewMatchingBearerSetLC(KIAPMask,
												ECommDbConnectionDirectionOutgoing);
	
	// Go to first record in the view.
	TInt error = searchView->GotoFirstRecord();
	
	// Buffer for IAP names in the view.
	TBuf<KCommsDbSvrMaxColumnNameLength> iapName;
	TUint32 iapID = 0;
	TBool found = EFalse;
	
	INFO_PRINTF1(_L("Searching through IAPs"));
	
	// Read IAPs until correct one is found or all IAPs are browsed through.
	while (error == KErrNone)
		{
		// Read the IAP name from CommsDat.
		iapName.FillZ();
		searchView->ReadTextL(TPtrC(COMMDB_NAME), iapName);
	    searchView->ReadUintL( TPtrC( COMMDB_ID ), iapID );
	
	    INFO_PRINTF3(_L( "IAP (ID = %d): \"%S\""), iapID, &iapName );
	
		// Try to match the name with user input.
		if (iapName.Match(aIAPName) == KErrNone)
			{
			searchView->ReadUintL(TPtrC(COMMDB_ID), iIAP);
	        found = ETrue;
			}
	
		error = searchView->GotoNextRecord();
		}
	
	// CommsDat and search view.
	CleanupStack::PopAndDestroy(searchView);
	CleanupStack::PopAndDestroy(commsDat);

	// no IAP found.
	if (!found)
		{
		ERR_PRINTF3(_L("No IAP found with name \"%S\" [%d]"), &aIAPName, error);
		User::Leave(error);
		}
	else
	    {
		INFO_PRINTF2(_L("IAP found with ID \"%d\""), iIAP);
	    }
	
	// Construct a CommsDat connection preferences structure.
	TCommDbConnPref* commDbConnPref = new (ELeave) TCommDbConnPref;
	
	// Connection preferences IAP Id, Network Id, Dialog Preference, Direction, Bearer Set
	// can be set through commDbConnPref.
	
	// Set only the values we use and leave rest untouched .
	commDbConnPref->SetIapId(iIAP);
	commDbConnPref->SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	commDbConnPref->SetBearerSet(KCommDbBearerUnknown);
	
	INFO_PRINTF1(_L("Constructed TCommDbConnPref for the IAP ID"));
	
	return commDbConnPref;
	}

