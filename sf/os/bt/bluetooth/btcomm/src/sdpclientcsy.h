// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef SDPCLIENT_H__
#define SDPCLIENT_H__

#include <e32std.h>
#include <e32base.h>
#include <bt_sock.h>
#include <btsdp.h>

enum TSDPParsePanics
	{
	EGetUintBadDescriptorLength,
	EFrameOverrun,
	EListOverrun
	};

static const TInt KSDPRFCommUUID=0x03;
static const TInt KCsySearchUUID=0x1102;// 1102 = LAN, 1103 = DUN

class CRFCommPortAttrib;

/**
	SDP Query RFCommm specific attributes class. 
	This class will be used for the first BT implementation to retrieve 
	the RFComm specific attributes only, from the parsed SDP attributes query.
*/
NONSHARABLE_CLASS(CRFCommAttribs) : public CBase, public MSdpElementBuilder
	{
public:
	static CRFCommAttribs* NewL(MSdpElementBuilder *aParent);
	~CRFCommAttribs();
	MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	MSdpElementBuilder* StartListL(/*TBuilderListType aType*/);
	MSdpElementBuilder* EndListL();
	MSdpElementBuilder* BuildDESL();
	MSdpElementBuilder* BuildDEA();
	void Print(TUint aIndent);
	TInt GetRFCommPort(TUint8& aServerChannel);
private:
	CRFCommAttribs();
private:
	CRFCommPortAttrib*	iPortAttrib; // this is also an MSdpElementBuilder
	TUUID				iRfcommUUID;
	};

/**
	CRFCommPortAttrib holds the attrinute that shows the port on which RFComm is bound.
	This object will be hanged of the CRFCommAttribs object when the correct
	UUID has been received (by the latter).
*/
NONSHARABLE_CLASS(CRFCommPortAttrib) : public CBase, public MSdpElementBuilder
	{
public:
	static CRFCommPortAttrib* NewL(MSdpElementBuilder *aParent);
	virtual ~CRFCommPortAttrib();
	MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	TUint8 GetRFCommPort();
private:
	CRFCommPortAttrib(MSdpElementBuilder* aParent);
	TUint GetUint(const TDesC8& aData);
private:
	MSdpElementBuilder*			iParent; 
	TUint8				iPort;
	TBool				iGotThePortNo;
	};

/**
	SDP Query service class list attributes
	find the KCsySearchUUID in the list
*/
NONSHARABLE_CLASS(CRFCommClass) : public CBase, public MSdpElementBuilder
	{
public:
	static CRFCommClass* NewL(const TUUID& aUUID);
	~CRFCommClass();
	MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	MSdpElementBuilder* StartListL(/*TBuilderListType aType*/);
	MSdpElementBuilder* EndListL();
	MSdpElementBuilder* BuildDESL();
	MSdpElementBuilder* BuildDEA();
	TBool InService();
private:
	CRFCommClass(const TUUID& aUUID);
private:
	const TUUID&		iUUID;
	TBool				iNService;
	};


#endif
