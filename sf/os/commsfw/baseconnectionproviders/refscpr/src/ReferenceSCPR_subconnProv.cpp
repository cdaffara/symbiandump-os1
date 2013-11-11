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
// Reference (empty) implementation file for a SubConnection Provider
// 
//

/**
 @file
 @internalComponent
*/


#include "ReferenceSCPR_subconnProv.h"


	

//-=========================================================
// Custom methods
//-=========================================================	
CReferenceSubConnectionProvider::CReferenceSubConnectionProvider(
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

/** NewL for the two stage construction. Calls the CReferenceSubConnectionProvider constructor
 and ConstructL on the constructed instance.

@param aFactory      - the subconnection provider factory the new CReferenceSubConnectionProvider 
                       will belong to and is to be instantiated by.
@param aConnProvider - the connection (represented by a connection provieder) the new 
                       CReferenceSubConnectionProvider will belong to.
@return              - a new, constructed instance of CReferenceSubConnectionProvider.
*/
CReferenceSubConnectionProvider* CReferenceSubConnectionProvider::NewL(
										CSubConnectionProviderFactoryBase& aFactory, 
	                                    CConnectionProviderBase& aConnProvider)
	{
    CReferenceSubConnectionProvider* provider = 
    		new(ELeave)CReferenceSubConnectionProvider(aFactory, aConnProvider);
	CleanupStack::PushL(provider);
    provider->ConstructL();
    CleanupStack::Pop();
    return provider;
    }
    
	                             
/** ConstructL for the two stage construction
*/
void CReferenceSubConnectionProvider::ConstructL()
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::ConstructL()"), this);	
	//TODO: perform 2nd stage construction, if needed.
	}
	

CReferenceSubConnectionProvider::~CReferenceSubConnectionProvider()
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
void CReferenceSubConnectionProvider::Notify(MConnectionDataClient::TNotify /*aNotifyType*/,  CConnectionProviderBase* /*aConnProvider*/, TInt /*aError*/, const CConNotificationEvent* /*aConNotificationEvent*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::Notify()"), this);
	//TODO:		
	}


/** Called to associate 'this' with a lower layer subconnection provider. 

@param aSubConnProvider to be the lower layer for 'this'
*/
void CReferenceSubConnectionProvider::AttachToNext(CSubConnectionProviderBase* /*aSubConnProvider*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::AttachToNext()"), this);
	//TODO:		
	}



/** Called to inform 'this' that the connection, to which 'this' belongs, is going down.

@param aConnProvider - the connection going down.
*/
void CReferenceSubConnectionProvider::ConnectionGoingDown(CConnectionProviderBase& /*aConnProvider*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::ConnectionGoingDown()"), this);
	//TODO:		
	}

/** Called to inform 'this' about a generic connection error.

@param aStage - stage at which the error has occured (currently as specified by NIFVAR.H)
@param aError - the error indication.
*/
void CReferenceSubConnectionProvider::ConnectionError(TInt /*aStage*/, TInt /*aError*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::ConnectionError()"), this);
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
void CReferenceSubConnectionProvider::DoDataClientJoiningL(MSubConnectionDataClient& /*aDataClient*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoDataClientJoiningL()"), this);
	//TODO:		
	}
	
	
	
/** Override this if you observe data clients leaving the sub-connection
    provider. This method is called before the data client leaves the
	the. Please note that it is not possible to to stop a data client
	from leaving.

@param aDataClient Data client about to leave the sub-connection provider
*/
void CReferenceSubConnectionProvider::DoDataClientLeaving(MSubConnectionDataClient& /*aDataClient*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoDataClientLeaving()"), this);
	//TODO:		
	}
	
	
	
/** Override this if you observe control clients leaving the sub-connection
    provider. This method is called before the control client leaves the
	the. Please note that it is not possible to to stop a control client
	from leaving.

@param aDataClient Data client about to leave the sub-connection provider
*/
void CReferenceSubConnectionProvider::DoControlClientJoiningL(MSubConnectionControlClient& /*aControlClient*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoControlClientJoiningL()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe control clients leaving the sub-connection
    provider. This method is called before the control client leaves the
	the. Please note that it is not possible to to stop a control client
	from leaving.

@param aDataClient Data client about to leave the sub-connection provider
*/
void CReferenceSubConnectionProvider::DoControlClientLeaving(MSubConnectionControlClient& /*aControlClient*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoControlClientLeaving()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe a data client receiving a new source
    address.

@param aDataClient Data client about to leave the sub-connection provider
@param aSource New source address
*/
void CReferenceSubConnectionProvider::DoSourceAddressUpdate(MSubConnectionDataClient& /*aDataClient*/, const TSockAddr& /*aSource*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoSourceAddressUpdate()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe a data client receiving a new destination
    address.

@param aDataClient Data client about to leave the sub-connection provider
@param aDestination New destination address
*/
void CReferenceSubConnectionProvider::DoDestinationAddressUpdate(MSubConnectionDataClient& /*aDataClient*/, const TSockAddr& /*aDestination*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoDestinationAddressUpdate()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe a data client receiving all its address
    address information.

@param aDataClient Data client about to leave the sub-connection provider
@param aSource New source address
@param aDestination New destination address
@param aConnectionInfo Connection Information (including IAP)
*/
void CReferenceSubConnectionProvider::DoDataClientRouted(MSubConnectionDataClient& /*aDataClient*/, const TSockAddr& /*aSource*/, const TSockAddr& /*aDestination*/, const TDesC8& /*aConnectionInfo*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoDataClientRouted()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this if you observe a data client about to receive new paramaters.
    This method is called before the parameters are associated with the sub-connection
	provider, so it is possible to abort this by leaving from this method with
	an appropriate system wide error code.

@param aParameterBundle Parameters received from the control client
@exception If the specific sub-connection provider does not want the parameters to be set
*/
void CReferenceSubConnectionProvider::DoParametersAboutToBeSetL(CSubConParameterBundle& /*aParameterBundle*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoParametersAboutToBeSetL()"), this);
	//TODO:		
	}
	
	
	
	
/** Override this to implement specific control behaviour request by the client

@param aOptionLevel Control Option level
@param aOptionName Control Option name
@param aOption Control Option value
@return KErrNone if successful, otherwise system error code
*/
TInt CReferenceSubConnectionProvider::DoControl(TUint /*aOptionLevel*/, TUint /*aOptionName*/, TDes8& /*aOption*/)
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoControl()"), this);
	//TODO:
	
	return KErrNotSupported;
	}
	
	
	
	
/** DoDataTransferL
*/
CConnDataTransfer& CReferenceSubConnectionProvider::DoDataTransferL()
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoDataTransferL()"), this);
	//TODO: 
	User::Leave(KErrNotSupported);
	CConnDataTransfer* null = NULL;
	return *static_cast<CConnDataTransfer*>(null);
	}
	
	
	
	
/** DoSelfConnectionDataClient
*/
MConnectionDataClient* CReferenceSubConnectionProvider::DoSelfConnectionDataClient()
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoSelfConnectionDataClient()"), this);
	//CReferenceSubConnectionProvider is a MConnectionDataClient, hence returning 'this'
	return this;
	}
	

	
/** Called to start 'this'.
	   
*/
void CReferenceSubConnectionProvider::DoStartL()
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoStartL()"), this);
	//TODO:		
	}
	
	
/** Called to stop 'this'.
   
*/
void CReferenceSubConnectionProvider::DoStop()
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoStop()"), this);
	//TODO:		
	}
	
	
/** Called to retrieve the lower subconnection provider of 'this'
  
 @return the subconnection provider below 'this'
*/	
CSubConnectionProviderBase* CReferenceSubConnectionProvider::DoNextLayer()
	{
	__FLOG_1(_L("CReferenceSubConnectionProvider[%x]::DoNextLayer()"), this);
	return iNextLayer;
	}
