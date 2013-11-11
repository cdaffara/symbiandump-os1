// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation file for the Default Reference SubConnection Provider
// 
//

/**
 @file
 @internalComponent
*/

#include "ReferenceSCPR_defaultSubconnProv.h"

	
//-=========================================================
// Custom methods
//-=========================================================	
CReferenceDefaultSubConnectionProvider::CReferenceDefaultSubConnectionProvider(
                                 CSubConnectionProviderFactoryBase& aFactory, 
	                             CConnectionProviderBase& aConnProvider)
:CSubConnectionProviderBase(aFactory, aConnProvider)
/** 

@param aFactory CSubConnectionProviderFactoryBase that should own the reference to 'this'
@param aConnProvider a connection that 'this' subconnection belongs to
*/
	{
	__FLOG_OPEN(KReferenceLogTag1, KReferenceLogTag2);
	}
	

/** NewL for the two stage construction. Calls the CReferenceDefaultSubConnectionProvider constructor
 and ConstructL on the constructed instance.

@param aFactory      - the subconnection provider factory the new CReferenceDefaultSubConnectionProvider 
                       will belong to and is to be instantiated by.
@param aConnProvider - the connection (represented by a connection provieder) the new 
                       CReferenceSubConnectionProvider will belong to.
@return              - a new, constructed instance of CReferenceDefaultSubConnectionProvider.
*/
CReferenceDefaultSubConnectionProvider* CReferenceDefaultSubConnectionProvider::NewL(
										CSubConnectionProviderFactoryBase& aFactory, 
	                                    CConnectionProviderBase& aConnProvider)
	{
    CReferenceDefaultSubConnectionProvider* provider = 
    		new(ELeave)CReferenceDefaultSubConnectionProvider(aFactory, aConnProvider);
	CleanupStack::PushL(provider);
    provider->ConstructL();
    CleanupStack::Pop();
    return provider;
    }
    
	                             
/** ConstructL for the two stage construction
*/
void CReferenceDefaultSubConnectionProvider::ConstructL()
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::ConstructL()"), this);	
	//TODO: perform 2nd stage construction, if needed.
	}


CReferenceDefaultSubConnectionProvider::~CReferenceDefaultSubConnectionProvider()
/** 
Destroys 'this'
*/
    {
    __FLOG_CLOSE;
    }



//-=========================================================
// MConnectionDataClient methods
//-=========================================================	

/** Called by the connection provider, of which 'this' is a data client and to which
 'this' belongs. The connection provider will call this method to notify its data clients.

@param aNotifyType   - one of MConnectionDataClient::TNotify enums indicating the notification type.
@param aConnProvider - the connection provider originating this notification.
@param aError        - the error indication
@param aConNotificationEvent - the actual event. 
*/
void CReferenceDefaultSubConnectionProvider::Notify(MConnectionDataClient::TNotify /*aNotifyType*/,  CConnectionProviderBase* /*aConnProvider*/, TInt /*aError*/, const CConNotificationEvent* /*aConNotificationEvent*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::Notify()"), this);
	//TODO:		
	}


/** Called to associate 'this' with a lower layer subconnection provider. 

@param aSubConnProvider to be the lower layer for 'this'
*/
void CReferenceDefaultSubConnectionProvider::AttachToNext(CSubConnectionProviderBase* /*aSubConnProvider*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::AttachToNext()"), this);
	//TODO:		
	}



/** Called to inform 'this' that the connection, to which 'this' belongs, is going down.

@param aConnProvider - the connection going down.
*/
void CReferenceDefaultSubConnectionProvider::ConnectionGoingDown(CConnectionProviderBase& /*aConnProvider*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::ConnectionGoingDown()"), this);
	//TODO:		
	}

/** Called to inform 'this' about a generic connection error.

@param aStage - stage at which the error has occured (currently as specified by NIFVAR.H)
@param aError - the error indication.
*/
void CReferenceDefaultSubConnectionProvider::ConnectionError(TInt /*aStage*/, TInt /*aError*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::ConnectionError()"), this);
	//TODO:		
	}







//-=========================================================
// CSubConnectionProviderBase methods
//-=========================================================



/** Override this if you observe data clients joining the sub-connection
    provider. This method is called before the data client is added to
	the lists, so if you wish to stop it being added, leave with an
	appropriate System wide error

@param aDataClient Data client wishing to join the sub-connection provider
@exception If the specific sub-connection provider does not want the data client to join
*/
void CReferenceDefaultSubConnectionProvider::DoDataClientJoiningL(MSubConnectionDataClient& /*aDataClient*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoDataClientJoiningL()"), this);
	//TODO:		
	}
	
	
	
/** Override this if you observe data clients leaving the sub-connection
    provider. This method is called before the data client leaves the
	the. Please note that it is not possible to to stop a data client
	from leaving.

@param aDataClient Data client about to leave the sub-connection provider
*/
void CReferenceDefaultSubConnectionProvider::DoDataClientLeaving(MSubConnectionDataClient& /*aDataClient*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoDataClientLeaving()"), this);
	//TODO:		
	}
	
	
	
/** Override this if you observe control clients leaving the sub-connection
    provider. This method is called before the control client leaves the
	the. Please note that it is not possible to to stop a control client
	from leaving.

@param aDataClient Data client about to leave the sub-connection provider
*/
void CReferenceDefaultSubConnectionProvider::DoControlClientJoiningL(MSubConnectionControlClient& /*aControlClient*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoControlClientJoiningL()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe control clients leaving the sub-connection
    provider. This method is called before the control client leaves the
	the. Please note that it is not possible to to stop a control client
	from leaving.

@param aDataClient Data client about to leave the sub-connection provider
*/
void CReferenceDefaultSubConnectionProvider::DoControlClientLeaving(MSubConnectionControlClient& /*aControlClient*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoControlClientLeaving()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe a data client receiving a new source
    address.

@param aDataClient Data client about to leave the sub-connection provider
@param aSource New source address
*/
void CReferenceDefaultSubConnectionProvider::DoSourceAddressUpdate(MSubConnectionDataClient& /*aDataClient*/, const TSockAddr& /*aSource*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoSourceAddressUpdate()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe a data client receiving a new destination
    address.

@param aDataClient Data client about to leave the sub-connection provider
@param aDestination New destination address
*/
void CReferenceDefaultSubConnectionProvider::DoDestinationAddressUpdate(MSubConnectionDataClient& /*aDataClient*/, const TSockAddr& /*aDestination*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoDestinationAddressUpdate()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe a data client receiving all its address
    address information.

@param aDataClient Data client about to leave the sub-connection provider
@param aSource New source address
@param aDestination New destination address
@param aConnectionInfo Connection Information (including IAP)
*/
void CReferenceDefaultSubConnectionProvider::DoDataClientRouted(MSubConnectionDataClient& /*aDataClient*/, const TSockAddr& /*aSource*/, const TSockAddr& /*aDestination*/, const TDesC8& /*aConnectionInfo*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoDataClientRouted()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe a data client about to receive new paramaters.
    This method is called before the parameters are associated with the sub-connection
	provider, so it is possible to abort this by leaving from this method with
	an appropriate system wide error code.

@param aParameterBundle Parameters received from the control client
@exception If the specific sub-connection provider does not want the parameters to be set
*/
void CReferenceDefaultSubConnectionProvider::DoParametersAboutToBeSetL(CSubConParameterBundle& /*aParameterBundle*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoParametersAboutToBeSetL()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this to implement specific control behaviour request by the client

@param aOptionLevel Control Option level
@param aOptionName Control Option name
@param aOption Control Option value
@return KErrNone if successful, otherwise system error code
*/
TInt CReferenceDefaultSubConnectionProvider::DoControl(TUint /*aOptionLevel*/, TUint /*aOptionName*/, TDes8& /*aOption*/)
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoControl()"), this);
	//TODO:
	
	return KErrNotSupported;
	}
	
	
	
	
/** DoDataTransferL
*/
CConnDataTransfer& CReferenceDefaultSubConnectionProvider::DoDataTransferL()
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoDataTransferL()"), this);
	//TODO: 
	User::Leave(KErrNotSupported);
	CConnDataTransfer* null = NULL;
	return *static_cast<CConnDataTransfer*>(null);
	}
	
	
	
	
/** DoSelfConnectionDataClient
*/
MConnectionDataClient* CReferenceDefaultSubConnectionProvider::DoSelfConnectionDataClient()
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoSelfConnectionDataClient()"), this);
	//CReferenceDefaultSubConnectionProvider is a MConnectionDataClient, hence returning 'this'
	return this;
	}
	

	
/** Called to start 'this'.
	   
*/
void CReferenceDefaultSubConnectionProvider::DoStartL()
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoStartL()"), this);
	//TODO:		
	}
	
	
/** Called to stop 'this'.
   
*/
void CReferenceDefaultSubConnectionProvider::DoStop()
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoStop()"), this);
	//TODO:		
	}
	
	
/** Called to retrieve the lower subconnection provider of 'this'
  
 @return the subconnection provider below 'this'
*/	
CSubConnectionProviderBase* CReferenceDefaultSubConnectionProvider::DoNextLayer()
	{
	__FLOG_1(_L("CReferenceDefaultSubConnectionProvider[%x]::DoNextLayer()"), this);
	return iNextLayer;
	}
    
