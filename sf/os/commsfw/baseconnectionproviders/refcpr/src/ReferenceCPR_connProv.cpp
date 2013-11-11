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
// Reference (empty) implementation file for a Connection Provider
//
//

/**
 @file
 @internalComponent
*/

#include "ReferenceCPR_connProv.h"

_LIT(KPanicReferenceCPRText,"Reference CPR not filled in properly");

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_RefCprConProv, "RefCprConProv");
#endif

//-=========================================================
// Custom methods
//-=========================================================
CReferenceConnectionProvider* CReferenceConnectionProvider::NewL(CConnectionProviderFactoryBase& aFactory)
	{
	CReferenceConnectionProvider* p = new (ELeave) CReferenceConnectionProvider(aFactory);
	return p;
	}

CReferenceConnectionProvider::CReferenceConnectionProvider(CConnectionProviderFactoryBase& aFactory)
:CConnectionProviderBase(aFactory),
 iNextLayer(NULL)
	{
	__FLOG_OPEN(KReferenceLogTag1, KReferenceLogTag2);
	}

CReferenceConnectionProvider::~CReferenceConnectionProvider()
	{
	__FLOG_CLOSE;
	}



//-=========================================================
// MConnectionControlClient methods
//-=========================================================
void CReferenceConnectionProvider::ConnectionGoingDown(CConnectionProviderBase& aConnProvider)
	{
	__FLOG_2(_L("CReferenceConnectionProvider %08x:\tConnectionGoingDown(aConnProvider %08x)"), this, &aConnProvider);
	if (&aConnProvider == iNextLayer)
		{
		iNextLayer = NULL;
		DeleteMeNow();
		}
	}

void CReferenceConnectionProvider::ProgressNotification(TInt aStage, TInt aError)
	{
	__FLOG_3(_L("CReferenceConnectionProvider %08x:\tProgressNotification(aStage %d aError %d)"), this, aStage, aError);
	TInt max = iControlClients.Count();
	for (TInt i = max - 1 ; i >= 0 ; i--)
   		{
	   	iControlClients[i]->ProgressNotification(aStage, aError);
		}
	}

void CReferenceConnectionProvider::ConnectionError(TInt aStage,  TInt aError)
	{
	__FLOG_3(_L("CReferenceConnectionProvider %08x:\tConnectionError(aStage %d aError %d)"), this, aStage, aError);
	TInt max = iControlClients.Count();
	for (TInt i = max - 1 ; i >= 0 ; i--)
		{
		iControlClients[i]->ConnectionError(aStage, aError);
		}
	max = iDataClients.Count();
	for (TInt i = max - 1 ; i >= 0 ; i--)
		{
		iDataClients[i]->ConnectionError(aStage, aError);
		}
  	}

void CReferenceConnectionProvider::ServiceChangeNotification(TUint32 aId, const TDesC& aType)
	{
	__FLOG_3(_L("CReferenceConnectionProvider %08x:\tServiceChangeNotification(aId %u aType %s)"), this, aId, &aType);
   	TInt max = iControlClients.Count();
	for (TInt i = max - 1 ; i >= 0 ; i--)
   		{
	   	iControlClients[i]->ServiceChangeNotification(aId, aType);
		}
  	}

void CReferenceConnectionProvider::SubConnectionEvent(CSubConnectionProviderBase* aSubConnNextLayerProvider, const TSubConnectionEvent& aSubConnectionEvent)
	{
	TInt max = iControlClients.Count();
	for (TInt i = max - 1 ; i >= 0 ; i--)
		{
	   	iControlClients[i]->SubConnectionEvent(aSubConnNextLayerProvider, aSubConnectionEvent);
		}
	}

void CReferenceConnectionProvider::LayerUp(TInt aError)
	{
	TInt max = iControlClients.Count();
	for (TInt i = max - 1 ; i >= 0 ; --i)
		{
		iControlClients[i]->LayerUp(aError);
		}

	// broadcast the event to the data clients also, sideways
	max = iDataClients.Count();
	for (TInt j = max - 1; j >= 0 ; --j)
		{
		iDataClients[j]->Notify(MConnectionDataClient::ENotifyLayerUp, this, aError, NULL);
		}
   }

MConnectionControlClient::TCtlType CReferenceConnectionProvider::CtlType() const
	{
	return MConnectionControlClient::ENormal;
	}






//-=========================================================
// CConnectionProviderBase methods
//-=========================================================
void CReferenceConnectionProvider::DoDataClientJoiningL(MConnectionDataClient& /*aDataClient*/)
	{
	__FLOG_2(_L("CReferenceConnectionProvider %08x:\tDoDataClientJoiningL number %d"), this, iDataClients.Count());
	//TODO: fill up with handling code if applicable.
	}

void CReferenceConnectionProvider::DoDataClientLeaving(MConnectionDataClient& /*aDataClient*/)
	{
	__FLOG_2(_L("CReferenceConnectionProvider %08x:\tDoDataClientLeaving number %d"), this, iDataClients.Count());
	//TODO: fill up with handling code if applicable.
	}

void CReferenceConnectionProvider::DoControlClientJoiningL(MConnectionControlClient& /*aControlClient*/)
	{
	__FLOG_2(_L("CReferenceConnectionProvider %08x:\tDoControlClientJoiningL number %d"), this, iControlClients.Count());
	//TODO: fill up with handling code if applicable.
	}

void CReferenceConnectionProvider::DoControlClientLeaving(MConnectionControlClient& /*aControlClient*/)
	{
	__FLOG_2(_L("CReferenceConnectionProvider %08x:\tDoControlClientLeaving number %d"), this, iControlClients.Count());
	//TODO: fill up with handling code if applicable.
  	}

void CReferenceConnectionProvider::DoStartL(Meta::SMetaData& aParams, const RMessagePtr2* aMessage)
	{
	//TODO: start this connection
	if (NULL != iNextLayer)
		{
		iNextLayer->StartL(aParams, aMessage);
		}
  	}

TInt CReferenceConnectionProvider::DoStop(TInt aError, const RMessagePtr2* aMessage)
	{
	if (NULL != iNextLayer)
		{
		//The next layer present. We'll now stop it here and anticipate ConnectionGoingDown() called on 'this'
		//when indeed, the connection is stopped.
		return iNextLayer->Stop(aError, aMessage);
		}
	else
    	{
    	//No next layer present, we're just need to delete this connection to complete the Stop message.
    	DeleteMeNow();
    	}
	return KErrNotReady;
  	}

void CReferenceConnectionProvider::DoProgressL(Meta::SMetaData& aBuffer) const
	{
	//TODO: update the progress info.
	if (NULL != iNextLayer)
		{
		iNextLayer->ProgressL(aBuffer);
		}
  	}

void CReferenceConnectionProvider::DoLastProgressError(Meta::SMetaData& aBuffer)
	{
	//TODO: return the last error actually ocurred.
	if (NULL != iNextLayer)
		{
		return iNextLayer->LastProgressError(aBuffer);
		}
  	}

void CReferenceConnectionProvider::DoRequestServiceChangeNotificationL()
	{
	if (NULL != iNextLayer)
		{
		iNextLayer->RequestServiceChangeNotificationL();
		}
  	}

void CReferenceConnectionProvider::DoCancelServiceChangeNotification()
	{
	if (NULL != iNextLayer)
		{
		iNextLayer->CancelServiceChangeNotification();
		}
  	}

void CReferenceConnectionProvider::DoControlL(TUint aOptionLevel, TUint aOptionName, Meta::SMetaData& aOption, const RMessagePtr2* aMessage)
	{
	if (NULL != iNextLayer)
		{
		return iNextLayer->ControlL(aOptionLevel, aOptionName, aOption, aMessage);
		}
  	}

TInt CReferenceConnectionProvider::DoEnumerateSubConnectionsL(TUint& /*aCount*/)
	{
	return KErrNotReady;
	//TODO: fill up aCount with the number of subconnections active.
	}

TUint CReferenceConnectionProvider::DoEnumerateClientsL(HBufC8*& aClientInfoBuffer, TEnumClients aClientType)
/**
Returns information about the clients of this Interface

@param aCount on return contains the number of clients using this Interface
@param aClientInfoBuffer on return contains a TPckg<> containing information about each client
@exception leaves with KErrNoMemory if memory allocation fails
*/
	{
	const TInt KInfoBufMaxLength = 1024;  //is this large enough?
	TBuf8<KInfoBufMaxLength> infoBuf;

	TUint count = 0;
	STypeId tid(KConnectionClientExtUid,EConnectionClientDesc);
	TInt max = iControlClients.Count();
	for ( TInt n = 0; n < max; n++ )
		{
		MConnectionClientDesc* intf = reinterpret_cast<MConnectionClientDesc*>(iControlClients[n]->FetchInterfaceInstanceL(*this,tid));
		if ( intf )
			{
			TConnectionProcessInfo cinfo;
			cinfo.GetInfoL(aClientType, count, *intf, infoBuf);
			}
		}
	STypeId tid2(KConnectionClientExtUid,EConnectionEnumerateClients);
	max = iDataClients.Count();
	for ( TInt n = 0; n < max; n++ )
		{
		MConnectionEnumerateClients* intf = reinterpret_cast<MConnectionEnumerateClients*>(iDataClients[n]->FetchInterfaceInstanceL(*this,tid2));
		if ( intf )
			{
			intf->EnumerateClientsL(count, infoBuf, aClientType);
			}
		}

	aClientInfoBuffer = infoBuf.AllocL();
	return count;
	}

void CReferenceConnectionProvider::DoConnectionControlActivityL( CConnectionProviderBase::TControlActivity /*aControlActivity*/, const Meta::SMetaData* /*aData*/, const RMessagePtr2* /*aMessage*/ )
	{
	//TODO:
	}

CConnectionSettings& CReferenceConnectionProvider::DoSettingsAccessL()
	{
	//TODO: If no lower layer, derive your own CConnectionSettings
	//class and return a reference to an instance of it here.
	if (NULL != iNextLayer)
		{
		return iNextLayer->SettingsAccessL();
		}
	User::Panic(KPanicReferenceCPRText, KErrNotFound);

	CConnectionSettings* null = NULL;
	return *static_cast<CConnectionSettings*>(null);
	}

TInt CReferenceConnectionProvider::DoAllSubConnectionNotificationEnable()
	{
	if (NULL != iNextLayer)
		{
		return iNextLayer->AllSubConnectionNotificationEnable();
		}
	return KErrNotReady;
  	}

TInt CReferenceConnectionProvider::DoCancelAllSubConnectionNotification()
	{
	if (NULL != iNextLayer)
		{
		return iNextLayer->CancelAllSubConnectionNotification();
		}
	return KErrNotReady;
  	}

void CReferenceConnectionProvider::DoSendIoctlMessageL(const RMessage2& aMessage)
	{
	if (NULL != iNextLayer)
		{
		iNextLayer->SendIoctlMessageL(aMessage);
		}
  	}

void CReferenceConnectionProvider::DoSendCancelIoctl()
	{
	if (NULL != iNextLayer)
		{
		iNextLayer->SendCancelIoctl();
		}
  	}

TInt CReferenceConnectionProvider::DoCanDoSubConnection(RSubConnection::TSubConnType /*aSubConnType*/) const
	{
	return ETrue;
  	}

void CReferenceConnectionProvider::DoJoinNextLayerL(CConnectionProviderBase* aNextLayer)
	{
	__ASSERT_DEBUG(( !iNextLayer && aNextLayer), User::Panic(KSpecAssert_RefCprConProv, 1));
	iNextLayer = aNextLayer;
    SetConnectionInfo(iNextLayer->ConnectionInfo());
    // join ourselves as a connection control client to the lower provider
    iNextLayer->JoinL(*this);
	}

CConnectionProviderBase* CReferenceConnectionProvider::DoNextLayer() const
	{
  	return iNextLayer;
  	}


