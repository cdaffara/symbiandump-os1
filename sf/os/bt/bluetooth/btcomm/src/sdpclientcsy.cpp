// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// sdpclientcsy.cpp - all the sdp parser code
// 
//

#include <bluetooth/logger.h>
#include <cs_port.h>
#include "btcomm.h"
#include "sdpclientcsy.h"
#include <es_sock.h>
#include <e32cons.h>
#include <bt_sock.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_COMM);
#endif

// RFComm specific attribute 'snatcher' objects for the SDP attribute query
// CRFCommAttribs
CRFCommAttribs* CRFCommAttribs::NewL(MSdpElementBuilder* /*aParent*/)
	{
	CRFCommAttribs* cl=new (ELeave) CRFCommAttribs;
	return cl;
	}

CRFCommAttribs::CRFCommAttribs()
	{
	}

CRFCommAttribs::~CRFCommAttribs()
	{
	delete iPortAttrib;
	}

MSdpElementBuilder* CRFCommAttribs::StartListL()
	{
	return this;
	}

MSdpElementBuilder* CRFCommAttribs::EndListL()
	{
	return this;
	}

MSdpElementBuilder* CRFCommAttribs::BuildUintL(const TDesC8& /*aInt*/)
	{
	return this;
	}

MSdpElementBuilder* CRFCommAttribs::BuildDESL()
	{
	return this;
	}


MSdpElementBuilder* CRFCommAttribs::BuildUUIDL(const TUUID& aUUID)
/**
	Go and get RFComm UUID info.
*/
	{
#ifdef _DEBUG
	TBuf8<40> desUUID;
	desUUID = aUUID.ShortestForm();
	TBuf<40> readableUUID;
	readableUUID.Copy(desUUID);

	LOG1(_L("CRFCommAttribs::BuildUUIDL(%S) main entry\n"), &readableUUID);
#endif
	if (iPortAttrib!=NULL)
		{
		LOG(_L("iPortAttrib already exists, returning it."));
		return iPortAttrib; // i.e go to the get RFComm port 'snatching' state
		// although this will not always be correct, but suits us for now
		}
	if(aUUID==TUUID(TUint16(KSDPRFCommUUID)))  
		{		
#ifdef _DEBUG
		LOG1(_L("CRFCommAttribs::BuildUUIDL(%S) if =RFCOMMUUID\n"), &readableUUID);
#endif
		iRfcommUUID=TUUID(aUUID);
		LOG(_L("CRFCommPortAttribs NewL"));
		iPortAttrib = CRFCommPortAttrib::NewL(this);
		LOG(_L("CRFCommPortAttribs - NewL survived"));
		return iPortAttrib;
		}

	LOG2(_L("CRFCommAttribs - return this = 0x%08x, iPortAttrib = 0x%08x"), this, iPortAttrib);
	return this;
	}

void CRFCommAttribs::Print(TUint /*aIndent*/)
	{
	}

TInt CRFCommAttribs::GetRFCommPort(TUint8& aServerChannel)
	{
	if (!iPortAttrib)
		{
		// the SDP response was syntactically correct, but semantically wrong
		// so we'd never have created the iPortAttrib object
		return KErrNotFound;
		}
	else
		{
		aServerChannel = iPortAttrib->GetRFCommPort();
		return KErrNone;
		}
	}

// CRFCommPortAttrib

CRFCommPortAttrib* CRFCommPortAttrib::NewL(MSdpElementBuilder* aParent)
	{
	CRFCommPortAttrib* cl=new (ELeave) CRFCommPortAttrib(aParent);
	return cl;
	}

CRFCommPortAttrib::CRFCommPortAttrib(MSdpElementBuilder* aParent) : iParent(aParent)
	{
	}

CRFCommPortAttrib::~CRFCommPortAttrib()
	{
	}


MSdpElementBuilder* CRFCommPortAttrib::BuildUintL(const TDesC8& aUint)
	{
	if(aUint.Length() > 4) // 4 being the 32bit TUint byte size
		User::Leave(KErrArgument);
	iPort=static_cast<TUint8>(GetUint(aUint));

	if(iPort<(TUint)KMinRfcommServerChannel || iPort>(TUint)KMaxRfcommServerChannel) //30 is max RFComm port
		{
		User::Leave(KErrArgument);
		}
	
	return iParent;
	}

TUint CRFCommPortAttrib::GetUint(const TDesC8& aData)
	{
	switch(aData.Length())
		{
	case 0:
		return 0;
	case 1:
		return aData[0];
	case 2:
		return BigEndian::Get16(aData.Ptr());
	case 4:
		return BigEndian::Get32(aData.Ptr());
	default:
		//ParsePanic(EGetUintBadDescriptorLength); //FIXME
		return 0;
		}
	}

TUint8 CRFCommPortAttrib::GetRFCommPort()
	{
	return iPort;
	}

//this builder checks to see if the UUID is in the Service Class ID list attribute
CRFCommClass* CRFCommClass::NewL(const TUUID& aUUID)
	{
	CRFCommClass* cl=new (ELeave) CRFCommClass(aUUID);
	return cl;
	}

CRFCommClass::CRFCommClass(const TUUID& aUUID)
	: iUUID(aUUID), iNService(EFalse)
	{
	}

CRFCommClass::~CRFCommClass()
	{
	}

MSdpElementBuilder* CRFCommClass::StartListL()
	{
	return this;
	}

MSdpElementBuilder* CRFCommClass::EndListL()
	{
	return this;
	}

MSdpElementBuilder* CRFCommClass::BuildUintL(const TDesC8& /*aInt*/)
	{
	return this;
	}

MSdpElementBuilder* CRFCommClass::BuildDESL()
	{
	return this;
	}


MSdpElementBuilder* CRFCommClass::BuildUUIDL(const TUUID& aUUID)
/**
	The meat, if the UUID is in the service class list then this is the record we want.
*/
	{
	if(aUUID==iUUID)  
		{		
		iNService=ETrue;
		}
	return this;
	}

TBool CRFCommClass::InService()
	{
	return iNService;
	}
