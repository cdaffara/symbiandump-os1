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
// AVDTPSERVICEINTERFACEH.h
// Service interfaces for AVDTP
// 
//

#ifndef AVDTPSERVICEINTERFACEH
#define AVDTPSERVICEINTERFACEH

#include "avdtpSignallingMessages.h"

#define UNEXPECTED_SERVICE_UPCALL __DEBUGGER()
#define UNIMPLEMENTED_INDICATION __DEBUGGER(); // chenge to flog later


/**
@note this class is effectively M class with a single dblquelink as member data
as it must contain member data, the name is chosen (as per Programming DB)
to show the semi-unusual case
Can be queued, but only one one queue at a time.
to avoid having to enumerate a set of possibly unknown friends
the queuelink is made public
The link member is given a nonobvious name to avoid clashing
*/
NONSHARABLE_CLASS(XAvdtpSignalReceiver)
	{
public:
// for indications that necessarily involve interaction with higher entities
// we pass the transaction label
// for those that the signalling channel can process "locally" with signalling session
// it does not need to forward the label

	virtual TInt DiscoverIndication(TAvdtpTransactionLabel /*aLabel*/, CAvdtpOutboundSignallingMessage& /*aDiscoverResponsePacket*/) 
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
		
	virtual TInt GetCapsIndication(TAvdtpTransactionLabel /*aLabel*/,
									TSEID /*aSEID*/,
									CAvdtpOutboundSignallingMessage& /*aGetCapsResponseMessage*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
	
	// if the implementor finds a bad service category the caller expects that this is returned in aConfigData
	virtual TInt SetConfigIndication(TAvdtpTransactionLabel /*aLabel*/,
									  TSEID /*aACPSEID*/,
									  TSEID /*aINTSEID*/,
									  RBuf8& /*aConfigData*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
		
	virtual TInt GetConfigIndication(TAvdtpTransactionLabel /*aLabel*/,
									 TSEID /*aSEID*/,
									 CAvdtpOutboundSignallingMessage& /*aGetConfigResponseMessage*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
		
	// if the implementor finds a bad service category the caller expects that this is returned in aConfigData
	virtual TInt ReconfigIndication(TAvdtpTransactionLabel /*aLabel*/,
									 TSEID /*aSEID*/,
									 RBuf8& /*aConfigData*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
		
	virtual TInt OpenIndication(TSEID /*aSEID*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}

	virtual TInt StartIndication(TAvdtpTransactionLabel /*aLabel*/, TSEID /*aACPSEID*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
		
	virtual TInt ReleaseIndication(TAvdtpTransactionLabel /*aLabel*/, TSEID /*aACPSEID*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
		
	virtual TInt SuspendIndication(TAvdtpTransactionLabel /*aLabel*/, TSEID /*aSEID*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
		
	virtual TInt AbortIndication(TAvdtpTransactionLabel /*aLabel*/, TSEID /*aACPSEID*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
		
	virtual TInt SecurityControlIndication(TAvdtpTransactionLabel /*aLabel*/, TSEID /*aACPSEID*/, const HBufC8* /*aSecurityData*/)
		{
		UNIMPLEMENTED_INDICATION
		return KErrNotSupported;
		}
		
	virtual void SignallingChannelReady(CSignallingChannel& /*aNewSignallingChannel*/)
		{
		UNIMPLEMENTED_INDICATION		
		}
		
	virtual void SignallingChannelError(TInt /*aError*/)
		{
		UNIMPLEMENTED_INDICATION		
		}
		
// protocol confirms
// no labels needed
	virtual void DiscoverConfirm(TInt /*aResult*/, const TAvdtpInternalDiscoverConfirm* const /*aConfirm*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}
		
	virtual void GetCapsConfirm(TInt /*aResult*/, TSEID /*aRemoteSEID*/, TAvdtpServiceCatBitMask /*aSeen*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}
		
	virtual void SetConfigConfirm(TInt /*aResult*/,TSEID /*aRemoteSEID*/, TAvdtpServiceCategory /*aFailedCategory*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}
		
	virtual void GetConfigConfirmL(TSEID /*aRemoteSEID*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}
	
	virtual void ReconfigConfirm(TInt /*aResult*/,TSEID /*aRemoteSEID*/, TAvdtpServiceCategory /*aFailedCategory*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}

	virtual void OpenConfirm(TInt /*aResult*/, TSEID /*aRemoteSEID*/)
		{
		UNEXPECTED_SERVICE_UPCALL
		}
			
	virtual void StartConfirm(TInt /*aResult*/, /*RSEIDArray&  aRemoteSEIDs*/TSEID /*aRemoteSEID*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}

	virtual void ReleaseConfirm(TInt /*aResult*/, TSEID /*aRemoteSEID*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}

	virtual void SuspendConfirm(TInt /*aResult*/, /*RSEIDArray&  aRemoteSEIDs*/TSEID /*aRemoteSEID*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}

	virtual void AbortConfirm(TSEID /*aRemoteSEID*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}

	virtual void SecurityControlConfirm(TInt /*aResult*/, TSEID /*aRemoteSEID*/, const TDesC8& /*aResponseData*/)
		{
		UNEXPECTED_SERVICE_UPCALL		
		}

public:	// see above
	TDblQueLink	iSignalReceiverEmbeddedLink;
	};


#endif //AVDTPSERVICEINTERFACEH
