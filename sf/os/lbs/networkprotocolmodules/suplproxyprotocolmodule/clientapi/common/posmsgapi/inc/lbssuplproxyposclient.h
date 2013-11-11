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
//

/**
 @file
 @publishedPartner   
 @prototype
*/

#ifndef LBS_SUPLPROXY_POSCLIENT_H
#define LBS_SUPLPROXY_POSCLIENT_H

//  INCLUDES
#include <e32base.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsextendedsatellite.h>

_LIT(KSuplProxyPosPanicCategory, "Supl Proxy Positioning");


/**
Panic codes used by the supl proxy positioning msg api
*/
enum TSuplProxyPositioningPanic
    {
    // 
    ESuplProxyPosBadHandle = 0,
    };

const TInt KMaxOutgoingPosPayloadLength = 512; //max size of an outgoing asn1-encoded pospayload!

/**
Base class for the Supl Info Request Data class. It provides functionality for finding out 
the derived type of the instantiated object.
*/
class TSuplInfoRequestDataBase
	{
public:
	/**
	 Enumeration for the types of the derived classes holding information for Supl Info Request Data. 
	 Could be extended later.
	*/
	enum TSuplInfoRequestDataClassType
		{
		ESuplInfoRequestDataUnknownClass		= 0,
		ESuplInfoRequestDataDataClass			= 0x01,
		};

public:

	IMPORT_C TUint32 SuplInfoRequestDataClassType() const;

protected:

	IMPORT_C TSuplInfoRequestDataBase(TUint32 aSuplInfoRequestDataClassType);

private:
	/** The type of the derived class */
	TUint32 iSuplInfoRequestDataClassType;
	/** Reserved for future use*/
	TUint8 iReserved[8];
	};

/**
 Class that holds data for the Supl Info Request. At the moment it holds the assistance data mask, 
 the capabilities, the reference location and the pos payload information.
*/
class TSuplInfoRequestData : public TSuplInfoRequestDataBase
	{
public:
	/**
	 Enumeration for the types of items requested for GetSuplInfo. There could be more 
	 items added at a later point.
	*/
	enum TLbsRequestItem
		{
		ERequestItemNone 				= 0x00000000,
		ERequestItemAssistance			= 0x00000001,
		ERequestItemCapabilities		= 0x00000002,
		ERequestItemPosition			= 0x00000004,
		ERequestItemPosPayload			= 0x00000008,	// note that the pospayload ptr is passed in a different param
		};	
	
public:
	IMPORT_C TSuplInfoRequestData();
	IMPORT_C void InitializeRequestMask();
	IMPORT_C void AddRequestItemToMask(const TLbsRequestItem aRequestItem);
	IMPORT_C void RemoveRequestItemFromMask(const TLbsRequestItem aRequestItem);
	IMPORT_C void SetAssistanceDataMask(TLbsAssistanceDataGroup aDataMask);
	IMPORT_C TInt RequestMask() const;
	IMPORT_C TLbsAssistanceDataGroup AssistanceDataMask() const;
	IMPORT_C void SetCapabilities(const TLbsNetPosCapabilities& aCaps);
	IMPORT_C TLbsNetPosCapabilities Capabilities() const;
	IMPORT_C TInt AssistanceMaskError() const;
	IMPORT_C void SetAssistanceMaskError(TInt aError);
	IMPORT_C TInt CapabilitiesError() const;
	IMPORT_C void SetCapabilitiesError(TInt aError);
	IMPORT_C TInt PosPayloadError() const;
	IMPORT_C void SetPosPayloadError(TInt aError);
	IMPORT_C TInt PositionError() const;
	IMPORT_C void SetPositionError(TInt aError);
	IMPORT_C const TPositionInfoBase& Position() const; 
	IMPORT_C void SetPosition(const TPositionInfoBase& aPosition);
	IMPORT_C void SetIsLastPosMsg(TBool aStatus);
	IMPORT_C TBool IsLastPosMsg() const;
private:
	// Items requested for GetSuplInfo. See TLbsRequestItem
	TInt	iRequestItemMask;
	
	// Assistance data mask used to fill in when GetSuplInfo gets completed.
	TLbsAssistanceDataGroup iDataMask;
	
	// receives capabilities
	TLbsNetPosCapabilities	iPosCaps;
	
	// receives position - not yet supported
	TPositionExtendedSatelliteInfo	iPosition;
	
	//inform the SUPL FW that this will be the last pospayload for this rrlp session
	TBool iLastPosMsg;
	
	// errors relating to each of the possible requests
	TInt 	iAssistanceMaskError;
	TInt	iCapabilitiesError;
	TInt	iPosPayloadError;
	TInt	iPositionError;
	/** Reserved for future use*/
	TUint8 iReserved[16];
	};



// CLASS DECLARATION
NONSHARABLE_CLASS(RSuplProxyPosSession): public RSessionBase
    {
    
    public: 
        
        IMPORT_C RSuplProxyPosSession();      
        
    	// called by COMASuplPosHandler::InitializeL(TRequestStatus &aStatus)  	
    	IMPORT_C TInt Connect();
   
    	// called by ~COMASuplPosHandlerBase()
        /**
        * From RSessionBase. 
        */
    	IMPORT_C void Close();  

    private:
   
        };

// CLASS DECLARATION
NONSHARABLE_CLASS(RSuplProxyPosSubSession): public RSubSessionBase
    {
    
    public: 
        
        IMPORT_C RSuplProxyPosSubSession();
                
        // called from COMASuplPosSession::InitializeL( const TInt aRequestID, TRequestStatus &aStatus )
        IMPORT_C TInt Open(RSuplProxyPosSession& aSession, TInt aRequestId);
        
        // called from COMASuplPosSession::void SessionEnd()!?
        IMPORT_C void Close();
        
        // Provides position estimates to the SUPL FW
        IMPORT_C void GetPositionL( TRequestStatus &aStatus, 
                                TPositionInfoBase& aPosition );  
        
        //Cancels an outstanding request to GetPositionL
        IMPORT_C void CancelGetPosition();
               
        //Called from COMASuplPosSession::InitializeL to reset the requestid held in the subsession
        IMPORT_C TInt ResetSubsessionRequestId(const TInt aRequestId );
        
		//Called from the POS Message plugin to notify that a SUPL session has ended.
        IMPORT_C void SessionComplete();
        
        // gets requested supl info
        IMPORT_C void GetSuplInfoL( TRequestStatus &aStatus, TSuplInfoRequestData& aSuplInfoRequestData, HBufC8* aPosPayload);

        //Cancels an outstanding request to GetSuplInfoL
        IMPORT_C void CancelGetSuplInfo();
        
        //Called to pass a positioning payload to SPPM
        IMPORT_C void SendPosPayloadL(const HBufC8& aPayload);    
       
        // Tells SPPM the positioning methods the SUPL FW wants to use
        IMPORT_C void SetPositioningMethod(const TLbsNetPosRequestMethod& aMethod);	
        
        // Tells SPPM the SLP address
        IMPORT_C void SetSlpAddress(const TDesC& aServerAddress);
        
    private:
    	TPckg<TSuplInfoRequestData>* iRequestInfoPkg;
    	TPckg<TPositionCourseInfo>* iPosPckg;
        };

#endif      // LBS_SUPLPROXY_POSCLIENT_H
