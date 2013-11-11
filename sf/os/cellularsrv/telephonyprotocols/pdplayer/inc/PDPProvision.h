/**
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Header file declaring the RawIP Provisioning structures.
* 
*
*/



/**
 @file
 @internalTechnology
*/

#ifndef PDPPROVISION_H_
#define PDPPROVISION_H_

#include <etelqos.h>
#include <comms-infras/metadata.h>
#include <metadatabase.h>		// TMDBElementId
#include "pdp.hrh"

namespace ESock 
	{
	class CCommsDatIapView;
	}

const TInt KScratchContextConfigSize = 1024;
	
class CGPRSProvision : public CBase, public Meta::SMetaData
/**
*/
	{
public:
    enum 
    {
    EUid = PDPScprImplUid,  //Same as PDPSCPR, who's gonna use it.
    ETypeId = 1,
    };
	
	static CGPRSProvision* NewLC(ESock::CCommsDatIapView* aIapView);
	TInt UmtsGprsRelease() const;
protected:
	void InitialiseConfigL(ESock::CCommsDatIapView* aIapView);
    void RetrieveGprsCompression(TUint& aCompression, ESock::CCommsDatIapView* aIapView) const;
    void RetrieveGprsAnonymousAccess(RPacketContext::TAnonymousAccess& aAnonymous, ESock::CCommsDatIapView* aIapView) const;
	
    void RetrieveApnNameL(TDes8& aApnName, ESock::CCommsDatIapView* aIapView) const;
	void RetrievePdpTypeL(RPacketContext::TProtocolType& aPdpType, ESock::CCommsDatIapView* aIapView) const;
	void RetrieveIPAndDnsSettingsL(TDes8& aPdpAddress, TDes8& aDns1, TDes8& aDns2, ESock::CCommsDatIapView* aIapView) const;
	void RetrieveUmtsCompressionL(RPacketContext::THeaderCompression& aHeaderCompression, RPacketContext::TDataCompression& aDataCompression, ESock::CCommsDatIapView* aIapView) const;
	void RetrieveUmtsPacketFlowIdentifierL(RPacketContext::TPacketFlowIdentifier& aPfi, ESock::CCommsDatIapView* aIapView) const;

private:
    TInt InitialiseScratchContext(TUint32 aConfigRel) const;
    void RetrieveAuthenticationInfoL(RPacketContext::TProtocolConfigOptionV2& aProtocolConfigOption, ESock::CCommsDatIapView* aIapView);

public:
    //RPacketContext::TContextConfigGPRS iGprsConfig;
	/** Temporary storage of GPRS, or R4, or R5 context configuration, here to avoid stack overflow */
	TBuf8<KScratchContextConfigSize> iScratchContext;
	TInt iUmtsGprsRelease;
	template<class CONTEXTTYPE>
	CONTEXTTYPE& GetScratchContextAs() const
	    {
	    __ASSERT_COMPILE(KScratchContextConfigSize >= sizeof(CONTEXTTYPE));
	    return *const_cast<CONTEXTTYPE*>(reinterpret_cast<const CONTEXTTYPE*>(iScratchContext.Ptr()));
	    }	
DATA_VTABLE
	};
	
class CTSYProvision : public CBase, public Meta::SMetaData
/**
*/
	{
public:
    enum 
    {
    EUid = PDPScprImplUid,  //Same as PDPSCPR, who's gonna use it.
    ETypeId = 2,
    };
    
    static CTSYProvision* NewLC(ESock::CCommsDatIapView* aIapView);

protected:
	void InitialiseConfigL(ESock::CCommsDatIapView* aIapView);

public:
    TName iTsyName;
	DATA_VTABLE
	};
	

class CDefaultPacketQoSProvision : public CBase, public Meta::SMetaData
/**

*/
	{
public:
    enum 
    {
    EUid = PDPScprImplUid,  //Same as PDPSCPR, who's gonna use it.
    ETypeId = 3,
    };
	static CDefaultPacketQoSProvision* NewL(ESock::CCommsDatIapView* aIapView);

protected:
	void InitialiseConfigL(ESock::CCommsDatIapView* aIapView);

public:
    RPacketQoS::TQoSR5Requested iParams;
	DATA_VTABLE
	};

class CImsExtProvision : public CBase, public Meta::SMetaData
/**

*/
	{
public:
    enum 
    {
    EUid = PDPScprImplUid,  //Same as PDPSCPR, who's gonna use it.
    ETypeId = 4,
    };
	static CImsExtProvision* NewLC(ESock::CCommsDatIapView* aIapView);

protected:
	void InitialiseConfigL(ESock::CCommsDatIapView* aIapView);

private:
	CImsExtProvision() : iImsSignalIndicator(EFalse) {}
	
public:
	TBool iImsSignalIndicator;
	DATA_VTABLE
	};



#endif // PDPPROVISION_H_
