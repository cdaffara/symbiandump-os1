// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of rawconduit
// 
//

#include "RawConduit.h"
#include "ProxySAP.h"
#include "physicallinksmanager.h"
#include "ACLSAP.h"
#include "linkutil.h"

// a bit like a "signaller" for raw data

CACLRawConduit* CACLRawConduit::NewL(CBTProxySAP& aParent)
	{
	CACLRawConduit* s = new (ELeave) CACLRawConduit(aParent);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}

void CACLRawConduit::ConstructL()
	{
	// get our link
	iACLLink = CACLLink::NewL(iParent.iLinksMan,
							  iParent.iPhysicalLink,
							  iParent.iLinksMan.LinkManagerProtocol());
	
	TPckgBuf<TInt> buf;
	TInt err = iACLLink->GetOption(KSolBtACL, ELMInboundACLSize, buf);
	__ASSERT_ALWAYS(!err, Panic(EBTACLRawConduitError));

	TInt MRU = buf();

	iReceiveBuf = HBufC8::NewL(MRU);

	iACLLink->SetNotify(this);

	TACLSockAddr rawSockAddr;
	rawSockAddr.SetPort(EACLPortRaw);
	iACLLink->SetLocalName(rawSockAddr);

	TBTSockAddr remoteAddr;
	remoteAddr.SetBTAddr(iParent.iPhysicalLink->BDAddr());
	iACLLink->SetRemName(remoteAddr);

	iACLLink->ActiveOpen(); // to attach us - should be there
	__ASSERT_DEBUG(iReady, Panic(EBTACLRawConduitError));
	
	if (!iReady)
		{
		User::Leave(KErrProxyWriteNotAvailable);
		}
	}

CACLRawConduit::CACLRawConduit(CBTProxySAP& aParent)
: iParent(aParent), iReady(EFalse)
	{
	}

CACLRawConduit::~CACLRawConduit()
	{
	delete iReceiveBuf;
 	//Don't know the state of the iACLLink. We assume it will have been closed by a call from the
 	//PHY before our owner deletes us, but if our owner performed an immediate shutdown that might
 	//not be the case. If we try to delete the iACLLink while it's still open it'll panic. So to be
 	//sure, do an immediate shutdown on it.
 	iACLLink->Shutdown(CServProviderBase::EImmediate);
	delete iACLLink;
	}

TInt CACLRawConduit::Write(const TDesC8& aData, TUint aOptions)
	{
	return iACLLink->Write(aData, aOptions, NULL);
	}

void CACLRawConduit::NewData(TUint aCount)
	{
	// get it out of the SAP
	// try our best - but it's unreliable tranport
	TPtr8 ptr = iReceiveBuf->Des();

	while (aCount--)
		{
		iACLLink->GetData(ptr, 0); // no options
		iParent.DataReceived();
		}
	}

void CACLRawConduit::GetData(TDes8& aData)
	{
	aData = *iReceiveBuf;
	}

void CACLRawConduit::CanSend()
	{
	// don't care - unreliable, so haven't stored anything to send
	}

void CACLRawConduit::ConnectComplete()
	{
	// as expected!
	iReady = ETrue;
	}

void CACLRawConduit::ConnectComplete(const TDesC8& /*aConnectData*/)
	{
	// don't care
	}

void CACLRawConduit::ConnectComplete(CServProviderBase& /*aSSP*/)
	{
	// don't care
	}

void CACLRawConduit::ConnectComplete(CServProviderBase& /*aSSP*/,const TDesC8& /*aConnectData*/)
	{
	// don't care
	}

void CACLRawConduit::CanClose(TDelete /*aDelete*/)
	{
	}

void CACLRawConduit::CanClose(const TDesC8& /*aDisconnectData*/,TDelete /*aDelete*/)
	{
	}

void CACLRawConduit::Error(TInt /*aError*/,TUint /*aOperationMask*/)
	{
	// our parent will be told by other means - this is just a data path
	}

void CACLRawConduit::Disconnect()
	{
	// our parent will be told by other means - this is just a data path
	}

void CACLRawConduit::Disconnect(TDesC8& /*aDisconnectData*/)
	{
	// don't care
	}

void CACLRawConduit::IoctlComplete(TDesC8* /*aBuf*/)
	{
	// not used
	}

void CACLRawConduit::NoBearer(const TDesC8& /*aConnectionInfo*/)
	{
	// don't care
	}

void CACLRawConduit::Bearer(const TDesC8& /*aConnectionInfo*/)
	{
	// don't care
	}

