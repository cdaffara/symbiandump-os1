// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_NODEMESSAGES_PARAMETERS_H
#define SYMBIAN_NODEMESSAGES_PARAMETERS_H

#include <comms-infras/ss_nodemessages.h>

namespace ESock
{

//any new enum added here needs to go into one of the utracedecoder definition files
//NEVER INSERT ANYTHING IN HERE ONLY ADD THE PENALTY IS SUDDEN DEATH!
class TCFParameterProvider
	{
  private:
  	enum
 	{
	ECFApplyRequest					= 1,
	ECFApplyResponse				= 2,
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	ECFSetParamsRequest				= 3,
	ECFSetParamsResponse			= 4,
	ECFGetParamsRequest				= 5,
	ECFGetParamsResponse			= 6,
#else
	ECFParamsRequest				= 3,
	ECFParamsResponse				= 4,
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	};
protected:

public:
    enum { ERealmId = 0x10285F4B }; //UID allocated on 4/6/08 from KUidNodeMessageRealms 
	/*==========================================================================
    [Name]:      TParamsRequest
    [Semantics]: Sent to a MMComms' ServiceProvider to request new configuration
                 or to retrieve the current configuration parameters.
                 The ServiceProvider should validate the parameters. but mustn't
                 commit the change until a subsequent TApplyRequest.

    [Type]:      Request for TParamsResponse.
    [Fallible]:  Yes.

    [Structure]:
    param: iBundle - the requested bundle of subconnection parameters.
           If empty handle, this is a request to retrieve current parameters.
           If handle has a bundle, this is a request to change current parameters.
    ===========================================================================*/
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    typedef TMessageSigFamilyBundle<ECFSetParamsRequest, TCFParameterProvider::ERealmId> TSetParamsRequest;
	typedef TMessageSigFamilyBundle<ECFGetParamsRequest, TCFParameterProvider::ERealmId> TGetParamsRequest;
#else
    typedef TMessageSigFamilyBundle<ECFParamsRequest, TCFParameterProvider::ERealmId> TParamsRequest;
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

    /*==========================================================================
    [Name]:      TParamsResponse
    [Semantics]: Sent as a response to TParamsRequest.

    [Type]:      Response to TParamsRequest

    [Structure]:
    param: iBundle - the verified parameter bundle.
    ===========================================================================*/
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    typedef TMessageSigFamilyBundle<ECFSetParamsResponse, TCFParameterProvider::ERealmId> TSetParamsResponse;
	typedef TMessageSigFamilyBundle<ECFGetParamsResponse, TCFParameterProvider::ERealmId> TGetParamsResponse;
#else
    typedef TMessageSigFamilyBundle<ECFParamsResponse, TCFParameterProvider::ERealmId> TParamsResponse;
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    /*==========================================================================
    [Name]:      TApplyRequest
    [Semantics]: Request to apply the parameters previously sent with TParamsRequest.

    [Type]:      Request for TApplyResponse.
    [Fallible]:  Yes.

    [Structure]:
    ===========================================================================*/
  	typedef Messages::TMessageSigVoid<ECFApplyRequest, TCFParameterProvider::ERealmId> TApplyRequest;
  	typedef Messages::TMessageSigVoid<ECFApplyResponse, TCFParameterProvider::ERealmId> TApplyResponse;
	};

} //namespace esock

#endif //SYMBIAN_NODEMESSAGES_PARAMETERS_H

