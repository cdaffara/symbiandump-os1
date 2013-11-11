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

CPreferredCarrierAvailable* CPreferredCarrierAvailable::NewL()
   {
   STypeId typeId = STypeId::CreateSTypeId(KMobilityMessagesImplementationUid, EPreferredCarrierAvailable);
   return static_cast<CPreferredCarrierAvailable*>(CCommsApiExtRespMsg::NewL(typeId));
   }

TAccessPointInfo CPreferredCarrierAvailable::OldAPInfo() const
   {
   return iOldAP;
   }

void CPreferredCarrierAvailable::SetOldAPInfo(TAccessPointInfo aAPInfo)
   {
   iOldAP = aAPInfo;
   }

TAccessPointInfo CPreferredCarrierAvailable::NewAPInfo() const
   {
   return iNewAP;
   }

void CPreferredCarrierAvailable::SetNewAPInfo(TAccessPointInfo aAPInfo)
   {
   iNewAP = aAPInfo;
   }

TBool CPreferredCarrierAvailable::IsUpgrade() const
   {
   return iIsUpgrade;
   }

void CPreferredCarrierAvailable::SetIsUpgrade(TBool aIsUpgrade)
   {
   iIsUpgrade = aIsUpgrade;
   }
   
TBool CPreferredCarrierAvailable::IsSeamless() const
	{
	return iIsSeamless;
	}
	
void CPreferredCarrierAvailable::SetIsSeamless(TBool aIsSeamless)
	{
	iIsSeamless = aIsSeamless;
	}

CNewCarrierActive* CNewCarrierActive::NewL()
   {
   STypeId typeId = STypeId::CreateSTypeId(KMobilityMessagesImplementationUid, ENewCarrierActive);
   return static_cast<CNewCarrierActive*>(CCommsApiExtRespMsg::NewL(typeId));
   }

TAccessPointInfo CNewCarrierActive::NewAPInfo() const
   {
   return iNewAP;
   }

void CNewCarrierActive::SetNewAPInfo(TAccessPointInfo aAPInfo)
   {
   iNewAP = aAPInfo;
   }

TBool CNewCarrierActive::IsSeamless() const
	{
	return iIsSeamless;
	}
	
void CNewCarrierActive::SetIsSeamless(TBool aIsSeamless)
	{
	iIsSeamless = aIsSeamless;
	}   
