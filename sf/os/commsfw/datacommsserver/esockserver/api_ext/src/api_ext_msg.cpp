// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "api_ext_msg.h"
#include <es_panic.h>
#include <elements/responsemsg.h>

/** 
	Creates a new api extension message using ECOM.

	@param aTypeId Id of the class (Composed of Uid of the implementation and an integer sub-type).
	@return a generic pointer to a specific message if successful, otherwise leaves with system error code.
*/
EXPORT_C CCommsApiExtRespMsg* CCommsApiExtRespMsg::NewL(const Meta::STypeId& aTypeId)
	{
	return static_cast<CCommsApiExtRespMsg*>(Meta::SMetaDataECom::NewInstanceL(aTypeId));
	}

/** 
	Creates a new api extension message using ECOM.

	@param aMsgBuffer a buffer containing the serialised form of the message object.
	@return a generic pointer to a specific event if successful, otherwise leaves with system error code.
*/
EXPORT_C CCommsApiExtRespMsg* CCommsApiExtRespMsg::NewL(const TDesC8& aMsgBuffer)
	{
	TPtrC8 buf(aMsgBuffer.Ptr(), aMsgBuffer.Length());
	return static_cast<CCommsApiExtRespMsg*>(Meta::SMetaDataECom::LoadL(buf));
	}

EXPORT_C void TCommsApiExtReqMsg::DispatchL(NetInterfaces::TInterfaceControl& /* aIntfCtl */,  const TSubSessionUniqueId& /* aSubSessionUniqueId */, Elements::RResponseMsg& aResponseMsg)
	{
	aResponseMsg.Panic(KESockClientPanic, EBadRequest);
	}


   	

