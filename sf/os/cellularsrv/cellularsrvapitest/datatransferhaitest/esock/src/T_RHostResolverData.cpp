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


#include "T_RHostResolverData.h"


/*@{*/
//LIT's for reading from ini file
_LIT(KRSocketServ,				"RSocketServ");
_LIT(KRConnection,				"RConnection");
_LIT(KAddress,      			"Address");
/*@}*/

/*@{*/
//LIT's for Commands
_LIT(KCmdDestructor,			"~");
_LIT(KCmdOpenHostResolver,		"Open");
_LIT(KCmdGetByName,				"GetByName");
_LIT(KCmdCloseHostResolver,		"Close");
/*@}*/

CT_RHostResolverData* CT_RHostResolverData::NewLC()
	{
	CT_RHostResolverData* self = new (ELeave) CT_RHostResolverData();
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}

CT_RHostResolverData* CT_RHostResolverData::NewL()
	{
	CT_RHostResolverData* self = CT_RHostResolverData::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CT_RHostResolverData::~CT_RHostResolverData()
	{
	if(iActiveCallback)
		{
		delete iActiveCallback;
		iActiveCallback = NULL;
		}
	
	DestroyData();
	}

CT_RHostResolverData::CT_RHostResolverData()
:	iHostResolver(NULL),
	iActiveCallback(NULL)
	{
	}

void CT_RHostResolverData::ConstructL()
	{
	iActiveCallback = CActiveCallback::NewL(*this);
	iHostResolver = new (ELeave)RHostResolver();
	}


void CT_RHostResolverData::RunL(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("*START* CT_RHostResolverData::RunL"));
	DecOutstanding(); // One of the async calls has completed 
    
    if( aActive == iActiveCallback )
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
    INFO_PRINTF1(_L("*END* CT_RHostResolverData::RunL"));
    }

/**
 * Return a pointer to the object that the data wraps
 *
 * @return	pointer to the object that the data wraps
 */
TAny* CT_RHostResolverData::GetObject()
	{
	return iHostResolver;
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
 * @leave					System wide error
 */
TBool CT_RHostResolverData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;
	if( aCommand == KCmdOpenHostResolver() )
		{
		DoCmdOpen(aSection);
		}
	else if( aCommand == KCmdGetByName() )
		{
		DoCmdGetByName(aSection,aAsyncErrorIndex);
		}
	else if( aCommand == KCmdCloseHostResolver())
		{
		DoCmdClose();
		}
	else if( aCommand == KCmdDestructor())
		{
		DestroyData();
		}
	else
		{
		ERR_PRINTF1(_L("Unknown command"));
		ret=EFalse;
		}
	
	return ret;
	}



/**
 * Opens RHostResolver
 * @param aSection -The section in config file to look for the RSocketServ and RConnection.
 * Sets TEF error if not successful.
 */
void CT_RHostResolverData::DoCmdOpen( const TTEFFunction& aSection)
    {
    INFO_PRINTF1(_L("*START* CT_RHostResolverData::DoCmdOpen"));
   
    // Start host resolver for DNS queries.
	INFO_PRINTF1(_L("Starting host resolver"));
	
	TPtrC socketServerName;
	TPtrC connectionName;
	TBool dataOk = ETrue;
	TInt error = KErrNone;
	
	if( !GetStringFromConfig(aSection, KRSocketServ(), socketServerName) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRSocketServ);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	if( !GetStringFromConfig(aSection, KRConnection(), connectionName) )
		{
		ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KRConnection);
		SetBlockResult(EFail);
		dataOk = EFalse;
		}
	
	if( dataOk )
		{
		INFO_PRINTF1(_L("Create a RSocketServ for iHostResolver"));
		RSocketServ* socketServ = static_cast<RSocketServ*>(GetDataObjectL(socketServerName));
		
		INFO_PRINTF1(_L("Create a RConnection for iHostResolver"));
		RConnection* connection = static_cast<RConnection*>(GetDataObjectL(connectionName));
		
		error = iHostResolver->Open(*socketServ, KAfInet, KProtocolInetTcp, *connection);
		}
	
	if ( error != KErrNone )
        {
        ERR_PRINTF2(_L("HostResolver could not be opened. Host resolver start failed [%d]"), error);
        SetError(error);
        }

    INFO_PRINTF1(_L("*END* CT_RHostResolverData::DoCmdOpen"));
    }

/**
 * Resolves a machine name to a TSockAddress asynchronously and saves it in iNameEntry
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 */
void CT_RHostResolverData::DoCmdGetByName(const TTEFFunction& aSection, const TInt aAsyncErrorIndex)
	{
	INFO_PRINTF1(_L("*START* CT_RHostResolverData::DoCmdGetByName"));
	
	TPtrC address;
	TBool dataOk = ETrue;
	
    if( !GetStringFromConfig( aSection, KAddress, address ) )
    	{
    	ERR_PRINTF2(_L("Error in getting parameter %S from INI file"), &KAddress);
    	SetBlockResult(EFail);
    	dataOk = EFalse;
    	}	

    if( dataOk )
    	{
    	// Resolve Internet address of our host.
    	iHostResolver->GetByName(address, iNameEntry, iActiveCallback->iStatus);
    	iActiveCallback->Activate(aAsyncErrorIndex);
    	IncOutstanding();
    	}
    
    INFO_PRINTF1(_L("*END* CT_RHostResolverData::DoCmdGetByName"));
	}

/**
 * Closes RHostResolver
 */
void CT_RHostResolverData::DoCmdClose()
	{
	INFO_PRINTF1(_L("*START* CT_RHostResolverData::DoCmdClose"));
    // DNS host resolver can be closed
    iHostResolver->Close();
    INFO_PRINTF1(_L("*END* CT_RHostResolverData::DoCmdClose"));
	}

/**
 * Destroys Wrapper
 */
void CT_RHostResolverData::DoCmdDestructor()
	{
	INFO_PRINTF1(_L("*START* CT_RHostResolverData::DoCmdDestructor"));
	DestroyData();
	INFO_PRINTF1(_L("*END* CT_RHostResolverData::DoCmdDestructor"));
	}

/**
 * Liberates allocated memory and closes RHostResolver
 */
void CT_RHostResolverData::DestroyData()
	{
	INFO_PRINTF1(_L("*START* CT_RHostResolverData::DestroyData"));
	if(iHostResolver)
		{
		iHostResolver->Close();
		delete iHostResolver;
		iHostResolver = NULL;
		}
	INFO_PRINTF1(_L("*END* CT_RHostResolverData::DestroyData"));
	}

/**
 * Getter method that returns iNameEntry (the result of name resolution).
 * @iNameEntry - Current name resolution result 
 */
TNameEntry CT_RHostResolverData::GetNameEntry()
	{
	return iNameEntry;
	}

