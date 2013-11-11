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
//

#ifndef _CodMan_H
#define _CodMan_H

#include <btdevice.h>

	
static const TUint16 KBTCodBitsRegdFlag = 0x8000;
static const TUint KBTMaxCodServices = 11;
static const TUint KBTCodServiceOffset = 13;	// require left shift for processing   
static const TUint KBTCodServiceLSB = 1;
static const TUint KBTCodValidServiceBits = 0x7F9;	// the valid Service class, when shifted down
static const TUint KBTCodDeviceField = 0x1FFF;	// the lower 13 bits are Device class
static const TUint32 KBTCodMask = 0x00FFFFFF;	// mask for lower 3 bytes


class CLinkMgrProtocol;

/***************************************************************************
CoD Service Manager.
Manages the Service bit settings in the composite CoD word.
***************************************************************************/
NONSHARABLE_CLASS(CBTCodServiceMan) : public CBase
	{
public:
	static CBTCodServiceMan* NewL();
	~CBTCodServiceMan();
	void Initialise();
	TInt RegisterCodService(TUint16 aCodService);
	TInt RemoveCodService(TUint16 aCodService);
	TInt PandSCodHandler(TUint32 aCod);
	CLinkMgrProtocol* iLinkMgr;
	
private:
	CBTCodServiceMan();
	void ConstructL();
	TUint16 ResolveServiceBits();
	void WriteCoDL();

	TUint32	iPandSCod;			// P&S record of Cod 
	TFixedArray <TUint,KBTMaxCodServices> iServRefCount;	// Each Service bit has a counter
	};

_LIT(KBTCodPanic, "BT CoD Service Manager");
enum TBTCodPanic
	{
	EBTCodBadCount,
	EBTCodServiceBits,
	EBTCodBadLinkmgr,
	EBTCodBadDeregister,
	};


#endif //_CodMan_H
