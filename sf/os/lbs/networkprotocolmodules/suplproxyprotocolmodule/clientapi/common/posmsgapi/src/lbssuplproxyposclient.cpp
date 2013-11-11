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
// Class implementation of RSuplProxyPosSession + RSuplProxyPosSubSession
// 
//

/**
 @file
 @internalTechnology
 @released
*/

// INCLUDE FILES
#include <e32std.h>
#include <lbssatellite.h>
#include "lbssuplproxyposclient.h" 
#include "lbssuplproxyservermsgenums.h"
#include "lbssystemcontroller.h"
#include "lbsdevloggermacros.h"

// The name of the server
// NB the name matches that of the LBS Privacy Server to allow common API class
_LIT(KSuplProxyServerName,"LBSPrivacyServer");

/**
 Constructor
 
 @param aAreaInfoClassType The type of class that is instantiated
 */
EXPORT_C TSuplInfoRequestDataBase::TSuplInfoRequestDataBase(TUint32 aSuplInfoRequestDataClassType)
	: iSuplInfoRequestDataClassType(aSuplInfoRequestDataClassType)
	{}

/**
 Specifies which type of class the object is an instantiation of.
 
 @return The class type
 */
EXPORT_C TUint32 TSuplInfoRequestDataBase::SuplInfoRequestDataClassType() const
	{
	return iSuplInfoRequestDataClassType;
	}

/**
 Constructor
 */
EXPORT_C TSuplInfoRequestData::TSuplInfoRequestData()
	: TSuplInfoRequestDataBase(ESuplInfoRequestDataDataClass), iRequestItemMask(0), iDataMask(0), iLastPosMsg(EFalse), 
		iAssistanceMaskError(KErrNone), iCapabilitiesError(KErrNone), 
		iPosPayloadError(KErrNone), iPositionError(KErrNone)
	{}


EXPORT_C void TSuplInfoRequestData::InitializeRequestMask()
	{
	iRequestItemMask = 0;
	}

EXPORT_C void TSuplInfoRequestData::AddRequestItemToMask(const TLbsRequestItem aRequestItem)
	{
	iRequestItemMask |= aRequestItem;
	}

EXPORT_C void TSuplInfoRequestData::RemoveRequestItemFromMask(const TLbsRequestItem aRequestItem)
	{
	iRequestItemMask &= ~aRequestItem;
	}

EXPORT_C void TSuplInfoRequestData::SetAssistanceDataMask(TLbsAssistanceDataGroup aDataMask)
	{
	iDataMask = aDataMask;
	}

EXPORT_C TLbsAssistanceDataGroup TSuplInfoRequestData::AssistanceDataMask() const
    {
    return iDataMask;
    }

EXPORT_C TInt TSuplInfoRequestData::RequestMask() const
	{
	return iRequestItemMask;
	}

EXPORT_C void TSuplInfoRequestData::SetCapabilities(const TLbsNetPosCapabilities& aCaps)
	{
	iPosCaps = aCaps;
	}

EXPORT_C TLbsNetPosCapabilities TSuplInfoRequestData::Capabilities() const
	{
	return iPosCaps;
	}

EXPORT_C TInt TSuplInfoRequestData::AssistanceMaskError() const
	{
	return iAssistanceMaskError;
	}

EXPORT_C void TSuplInfoRequestData::SetAssistanceMaskError(TInt aError)
	{
	iAssistanceMaskError = aError;
	}

EXPORT_C TInt TSuplInfoRequestData::CapabilitiesError() const
	{
	return iCapabilitiesError;
	}

EXPORT_C void TSuplInfoRequestData::SetCapabilitiesError(TInt aError)
	{
	iCapabilitiesError = aError;
	}

EXPORT_C TInt TSuplInfoRequestData::PosPayloadError() const
	{
	return iPosPayloadError;
	}

EXPORT_C void TSuplInfoRequestData::SetPosPayloadError(TInt aError)
	{
	iPosPayloadError = aError;
	}

EXPORT_C TInt TSuplInfoRequestData::PositionError() const
	{
	return iPositionError;
	}

EXPORT_C void TSuplInfoRequestData::SetPositionError(TInt aError)
	{
	iPositionError = aError;
	}

EXPORT_C const TPositionInfoBase& TSuplInfoRequestData::Position() const
    {
    return iPosition;
    }

EXPORT_C void TSuplInfoRequestData::SetPosition(const TPositionInfoBase& aPosition)
    {
    TUint8* baseToAddr = reinterpret_cast<TUint8*>(&iPosition)+sizeof(TPositionClassTypeBase);            
    const TUint8* const baseFromAddr = reinterpret_cast<const TUint8*>(&aPosition)+sizeof(TPositionClassTypeBase);
    TUint32 dataLength = aPosition.PositionClassSize()-sizeof(TPositionClassTypeBase); // we copy only this many bytes form the source
    Mem::Copy(baseToAddr, baseFromAddr, dataLength);
    }

EXPORT_C void TSuplInfoRequestData::SetIsLastPosMsg(TBool aStatus)
    {
    iLastPosMsg = aStatus;
    }

EXPORT_C TBool TSuplInfoRequestData::IsLastPosMsg() const
    {
    return iLastPosMsg;
    }

/*
 * C++ default constructor 
*/
EXPORT_C RSuplProxyPosSession::RSuplProxyPosSession()
    : RSessionBase()
    {
  	LBSLOG(ELogP1, "RSuplProxyPosSession::RSuplProxyPosSession() Begin and End\n");
    }

/** 
 * Opens a connection with the server. This method must be called
 * before any other operation.
 * @return KErrNone if the operation was successful, otherwise
 * one of the system-wide error codes.
 */

EXPORT_C TInt RSuplProxyPosSession::Connect()
    {
	LBSLOG(ELogP1, "RSuplProxyPosSession::Connect() Begin\n");

    TInt err = KErrNone;
        
	TVersion version = TVersion(KLbsSuplProxyPosAPIVersionMajor,
								KLbsSuplProxyPosAPIVersionMinor,
								KLbsSuplProxyPosAPIVersionBuild);

	// calls CSuplProxyServer::NewSessionL():
	err = CreateSession(KSuplProxyServerName, version);


	LBSLOG(ELogP1, "RSuplProxyPosSession::Connect() End\n");
	return err;
	}

/**
*Close the connection to the server
*/
EXPORT_C void RSuplProxyPosSession::Close()
    {
	LBSLOG(ELogP1, "RSuplProxyPosSession::Close() Begin\n");

    RSessionBase::Close();

	LBSLOG(ELogP1, "RSuplProxyPosSession::Close() End\n");
    }

/*
 * C++ default constructor 
*/
EXPORT_C RSuplProxyPosSubSession::RSuplProxyPosSubSession()
    : RSubSessionBase(),iRequestInfoPkg(NULL), iPosPckg(NULL)
    {
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::RSuplProxyPosSubSession() Begin and End\n");
    }

/*
 * Open
*/
EXPORT_C TInt RSuplProxyPosSubSession::Open(RSuplProxyPosSession& aSession, TInt aRequestId)
    {
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::Open() Begin\n");
    __ASSERT_ALWAYS(aSession.Handle() != 0, User::Panic(KSuplProxyPosPanicCategory, KErrBadHandle));

    // calls CSuplProxySession::ServiceL(ELbsSuplProxyServerPosOpen):
    TInt ret = CreateSubSession(aSession, ELbsSuplProxyServerPosOpen, TIpcArgs(aRequestId)); 

	LBSLOG(ELogP1, "RSuplProxyPosSubSession::Open() End\n");	
    return ret;
    }

/*
 * Close
*/
EXPORT_C void RSuplProxyPosSubSession::Close()
    {
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::Close() Begin\n");

    // calls CSuplProxySession::ServiceL(ELbsSuplProxyServerPosClose):
    CloseSubSession(ELbsSuplProxyServerPosClose);
    
    delete iRequestInfoPkg;	// will exist if GetSuplInfo was called
    iRequestInfoPkg = NULL; 
    
    delete iPosPckg;	// will exist if GetPosition was called
    iPosPckg = NULL;

	LBSLOG(ELogP1, "RSuplProxyPosSubSession::Close() End\n");
    }

// Provides position estimates to the SUPL FW
EXPORT_C void RSuplProxyPosSubSession::GetPositionL(TRequestStatus &aStatus, 
                        TPositionInfoBase& aPosition )
	{
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::GetPositionL() Begin\n");

    __ASSERT_ALWAYS(SubSessionHandle() != 0, User::Leave(KErrBadHandle));
    
    delete iPosPckg;	// in case we have one from a previous GetPositionL call, which must have completed now since client only allowed one.
    iPosPckg = NULL;
    
    // calls CSuplProxySession::ServiceL():
    iPosPckg = new (ELeave)TPckg<TPositionCourseInfo>(static_cast <TPositionCourseInfo&>(aPosition));   	
    
	SendReceive(ELbsSuplProxyServerGetPosition, TIpcArgs(iPosPckg), aStatus);	

	LBSLOG(ELogP1, "RSuplProxyPosSubSession::GetPositionL() End\n");
	}

//Cancels an outstanding request to GetPositionL
EXPORT_C void RSuplProxyPosSubSession::CancelGetPosition()
	{	
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::CancelGetPosition() Begin\n");

    __ASSERT_ALWAYS(SubSessionHandle() != 0,
        User::Panic(KSuplProxyPosPanicCategory, ESuplProxyPosBadHandle));
    
    // calls CSuplProxySession::ServiceL():
	SendReceive(ELbsSuplProxyServerCancelGetPosition);	
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::CancelGetPosition() End\n");
	}


//Used when an exisiting POS subsession handle is to be reused for
//a new SUPL session by reinitalizing it with the new request id from
//the SUPL FW
EXPORT_C TInt RSuplProxyPosSubSession::ResetSubsessionRequestId(const TInt aRequestId )
    {
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::ResetSubsessionRequestId() Begin\n");
	
    __ASSERT_ALWAYS(SubSessionHandle() != 0,
        User::Panic(KSuplProxyPosPanicCategory, ESuplProxyPosBadHandle));
    
    // calls CSuplProxySession::ServiceL() passing in the new request Id 
    TIpcArgs args;
    args.Set(0,aRequestId);
    
    TInt err = SendReceive(ELbsSuplProxyServerPosResetRequestId, args);
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::ResetSubsessionRequestId() End\n");
    return err;
    }

EXPORT_C void RSuplProxyPosSubSession::SessionComplete()
    {
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::SessionComplete() Begin\n");
    __ASSERT_ALWAYS(SubSessionHandle() != 0,
        User::Panic(KSuplProxyPosPanicCategory, ESuplProxyPosBadHandle));

    SendReceive(ELbsSuplProxyServerPosNotifySessionComplete);
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::SessionComplete() End\n");
    }

// Provides position estimates to the SUPL FW
EXPORT_C void RSuplProxyPosSubSession::SendPosPayloadL(const HBufC8& aPayload)
	{
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::SendPosPayloadL() Begin\n");
    __ASSERT_ALWAYS(SubSessionHandle() != 0, User::Leave(KErrBadHandle));

    // calls CSuplProxySession::ServiceL():    
	SendReceive(ELbsSuplProxyServerPosPayload, TIpcArgs(&aPayload));	
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::SendPosPayloadL() End\n");
	}

// Sets the positioning method requested by SUPL FW 
EXPORT_C void RSuplProxyPosSubSession::SetPositioningMethod(const TLbsNetPosRequestMethod& aMethod)
	{
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::SetPositioningMethod() Begin\n");
    __ASSERT_ALWAYS(SubSessionHandle() != 0, User::Panic(KSuplProxyPosPanicCategory, KErrBadHandle));

    TIpcArgs args;
    TPckg<TLbsNetPosRequestMethod> posMethodPckg(aMethod);
    args.Set(0, &posMethodPckg);
    
	SendReceive(ELbsSuplProxyServerSetPosMethod, args);	
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::SetPositioningMethod() End\n");
	}

// Sets the positioning method requested by SUPL FW 
EXPORT_C void RSuplProxyPosSubSession::SetSlpAddress(const TDesC& aServerAddress)
	{
    __ASSERT_ALWAYS(SubSessionHandle() != 0, User::Panic(KSuplProxyPosPanicCategory, KErrBadHandle));

    TIpcArgs args;
    args.Set(0, &aServerAddress);
	SendReceive(ELbsSuplProxyServerSetSlpAddress, args);	
	}

// Provides encoded rrlp info to the SUPL FW
EXPORT_C void RSuplProxyPosSubSession::GetSuplInfoL(TRequestStatus &aStatus, TSuplInfoRequestData& aSuplInfoRequestData, HBufC8* aPosPayload)
	{
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::GetSuplInfoL() Begin\n");
    __ASSERT_ALWAYS(SubSessionHandle() != 0, User::Leave(KErrBadHandle));
    
    delete iRequestInfoPkg;	// in case we have one from a previous GetSuplInfoL call, which must have completed now since client only allowed one.
    iRequestInfoPkg = NULL;
    
    // ipc args: 0 = TSuplInfoRequestData, 1 = pospayload where appropriate
    TIpcArgs args;
    
    // create a package that points to the supl info structure to be filled in
    iRequestInfoPkg = new (ELeave) TPckg<TSuplInfoRequestData>(aSuplInfoRequestData);	

    args.Set(0, iRequestInfoPkg);
    args.Set(1, TIpcArgs::ENothing);
    
    if(aPosPayload)
    	{
    	TPtr8 buffer = aPosPayload->Des();	
    	args.Set(1, &buffer); 
    	}

    // calls CSuplProxySession::ServiceL():   
    SendReceive(ELbsSuplProxyServerGetSuplInfo, args, aStatus);
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::GetSuplInfoL() End\n");
	}

					  
// Cancels an outstanding request to GetSuplInfoL
EXPORT_C void RSuplProxyPosSubSession::CancelGetSuplInfo()
	{
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::CancelGetSuplInfo() Begin\n");
    __ASSERT_ALWAYS(SubSessionHandle() != 0,
        User::Panic(KSuplProxyPosPanicCategory, ESuplProxyPosBadHandle));	
    
    // calls CSuplProxySession::ServiceL():
	SendReceive(ELbsSuplProxyServerCancelGetSuplInfo);	
	LBSLOG(ELogP1, "RSuplProxyPosSubSession::CancelGetSuplInfo() End\n");
	}
