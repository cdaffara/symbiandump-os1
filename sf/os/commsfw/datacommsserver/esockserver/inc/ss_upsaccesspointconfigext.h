// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// UPS Server Access Point Config Extension.
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef SYMBIAN_UPSACCESSPOINTCONFIGEXT_H
#define SYMBIAN_UPSACCESSPOINTCONFIGEXT_H

#define SYMBIAN_NETWORKING_UPS

#ifdef SYMBIAN_NETWORKING_UPS

#include <es_sock.h>

typedef TBuf8<KMaxSockAddrSize> TUpsDestinationAddr;

enum TUpsDestinationAddrType
/**
Type of information contained in TUpsDestinationAddr
*/
	{
	ETNone,
	ETSockAddress
	};

class CUPSAccessPointConfigExt : public CBase, public Meta::SMetaData
/**
Provisioning information for UPS
*/
	{
public:
    enum 
    	{
    	EUPSAccessPointConfigUid = 0x10285837, // UID of cs dll
    	ETypeId = 1
    	};
public:

    inline static CUPSAccessPointConfigExt* NewL(TUpsDestinationAddrType aDestinationAddrType);
	
	inline void  SetProcessId(const TProcessId& aProcessId);
    inline const TProcessId& GetProcessId() const;

	inline void  SetThreadId(const TThreadId& aThreadId);
    inline const TThreadId& GetThreadId() const;

    inline void SetPolicyCheckResult(TInt aPolicyCheckResult);
    inline TInt GetPolicyCheckResult() const;

    inline const TUpsDestinationAddr& GetDestinationAddr() const;
    inline void  SetDestinationAddr(const TUpsDestinationAddr& aDestinationAddr);
    inline void  ResetDestinationAddr();  

    inline TUpsDestinationAddrType GetDestinationAddrType() const;
    inline void SetDestinationAddrType(TUpsDestinationAddrType aDestinationAddrType);

    inline void SetServiceId(TInt aServiceId);
    inline TUid GetServiceId() const;
    inline virtual ~CUPSAccessPointConfigExt();

protected:
	inline CUPSAccessPointConfigExt(TUpsDestinationAddrType aDestinationAddrType);

private:
	TProcessId				iProcessId;
	TThreadId        		iThreadId;
	TInt             		iPolicyCheckResult;		// result of platsec check
	TUpsDestinationAddrType	iDestinationAddrType;	// format of destination address
	TUpsDestinationAddr 	iDestinationAddr;		// destination address
	TUid					iServiceId;				// Implementation Id of Tier Manager
	
public:
    EXPORT_DATA_VTABLE_AND_FN
	};    

//

CUPSAccessPointConfigExt::CUPSAccessPointConfigExt(TUpsDestinationAddrType aDestinationAddrType)
  : iDestinationAddrType(aDestinationAddrType) 
	{ }

CUPSAccessPointConfigExt::~CUPSAccessPointConfigExt()
	{ }

CUPSAccessPointConfigExt* CUPSAccessPointConfigExt::NewL(TUpsDestinationAddrType aDestinationAddrType)
	{ return new (ELeave) CUPSAccessPointConfigExt(aDestinationAddrType); }

void CUPSAccessPointConfigExt::SetProcessId(const TProcessId& aProcessId)
	{ iProcessId = aProcessId; }

const TProcessId& CUPSAccessPointConfigExt::GetProcessId() const
	{ return iProcessId; }

void  CUPSAccessPointConfigExt::SetThreadId(const TThreadId& aThreadId)
	{ iThreadId = aThreadId; }
	
const TThreadId& CUPSAccessPointConfigExt::GetThreadId() const
	{ return iThreadId; }

void CUPSAccessPointConfigExt::SetPolicyCheckResult(TInt aPolicyCheckResult)
	{ iPolicyCheckResult = aPolicyCheckResult; }

TInt CUPSAccessPointConfigExt::GetPolicyCheckResult() const
	{ return iPolicyCheckResult; }

const TUpsDestinationAddr& CUPSAccessPointConfigExt::GetDestinationAddr() const
    { return iDestinationAddr; }
    
void CUPSAccessPointConfigExt::SetDestinationAddr(const TUpsDestinationAddr& aDestinationAddr)
    { iDestinationAddr = aDestinationAddr; }
    
TUpsDestinationAddrType CUPSAccessPointConfigExt::GetDestinationAddrType() const
    { return iDestinationAddrType; }

void CUPSAccessPointConfigExt::SetDestinationAddrType(TUpsDestinationAddrType aDestinationAddrType)
    { iDestinationAddrType = aDestinationAddrType; }

void CUPSAccessPointConfigExt::ResetDestinationAddr()
    { iDestinationAddr.SetLength(0); }
    
void CUPSAccessPointConfigExt::SetServiceId(TInt aServiceId)
	{ iServiceId.iUid = aServiceId; }

TUid CUPSAccessPointConfigExt::GetServiceId() const
	{ return iServiceId; }
#endif // SYMBIAN_NETWORKING_UPS

#endif // SYMBIAN_UPSACCESSPOINTCONFIGEXT_H

