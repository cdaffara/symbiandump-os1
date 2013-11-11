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
//

#include "mobilitymessagesecom.h"
#include <comms-infras/es_mobility_apiext.h>

using namespace ESock;

Meta::SMetaDataECom* CMobilityMsgFactory::NewMessageL(TAny* aParams)
   {
   TInt32 type = reinterpret_cast<TInt32>(aParams);

   switch (type)
      {
   case EPreferredCarrierAvailable:
      return new (ELeave) CPreferredCarrierAvailable;
      
   case ENewCarrierActive:
      return new (ELeave) CNewCarrierActive;
      
   default:
      User::Leave(KErrNotFound);
      }

   return NULL;
   }

/*-------------------------------------------------------
	Server -> Client messages
  -------------------------------------------------------*/

START_ATTRIBUTE_TABLE( CPreferredCarrierAvailable, KMobilityMessagesImplementationUid, EPreferredCarrierAvailable )
   REGISTER_ATTRIBUTE( CPreferredCarrierAvailable, iOldAP, TMeta<TAccessPointInfo>)
   REGISTER_ATTRIBUTE( CPreferredCarrierAvailable, iNewAP, TMeta<TAccessPointInfo>)
   REGISTER_ATTRIBUTE( CPreferredCarrierAvailable, iIsUpgrade, TMeta<TBool>)
   REGISTER_ATTRIBUTE( CPreferredCarrierAvailable, iIsSeamless, TMeta<TBool>)
END_ATTRIBUTE_TABLE()

START_ATTRIBUTE_TABLE( CNewCarrierActive, KMobilityMessagesImplementationUid, ENewCarrierActive )
   REGISTER_ATTRIBUTE( CNewCarrierActive, iNewAP, TMeta<TAccessPointInfo>)
   REGISTER_ATTRIBUTE( CNewCarrierActive, iIsSeamless, TMeta<TBool>)
END_ATTRIBUTE_TABLE()

CPreferredCarrierAvailable::CPreferredCarrierAvailable()
   {
   }

void CPreferredCarrierAvailable::DispatchL(MMobilityProtocolResp& aProtocol)
	{
	aProtocol.PreferredCarrierAvailable(iOldAP, iNewAP, iIsUpgrade, iIsSeamless);
	}

CNewCarrierActive::CNewCarrierActive()
   {
   }

void CNewCarrierActive::DispatchL(MMobilityProtocolResp& aProtocol)
	{
	aProtocol.NewCarrierActive(iNewAP, iIsSeamless);
	}

