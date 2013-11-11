/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


#if (!defined __T_BT_UTIL_H__)
#define __T_BT_UTIL_H__

//	EPOC includes
#include <test/datawrapper.h>
#include <btsdp.h>
#include <es_sock.h>
#include <bt_sock.h>
#include <d32comm.h>
#include <bttypes.h>
#include <bluetoothav.h>
#include <gavdp.h>
#include "DataWrapperBase.h"

class CT_BTUtil
	{
public:
	static void					GetChannelPriority(CDataWrapper& aDataWrapper, TL2CapConfig::TChannelPriority& aType, const TDesC& aSection, const TDesC& aField);
	static void 				GetChannel(CDataWrapper& aDataWrapper, TL2CapConfig::TChannelReliability& aType, const TDesC& aSection, const TDesC& aField);
	static void					GetBitValue(CDataWrapper& aDataWrapper, TBps& aType, const TDesC& aSection, const TDesC& aField);
	static void					GetDataBits(CDataWrapper& aDataWrapper, TDataBits& aType, const TDesC& aSection, const TDesC& aField);
	static void					GetParity(CDataWrapper& aDataWrapper, TParity& aType, const TDesC& aSection, const TDesC& aField);
	static void					GetStopBits(CDataWrapper& aDataWrapper, TStopBits& aType, const TDesC& aSection, const TDesC& aField);
	static void					GetTRPNFlowCtrlMask(CDataWrapper& aDataWrapper, TRPNFlowCtrlMask& aType, const TDesC& aSection, const TDesC& aField);
	static TBool				GetPacketTypeIntValue(TPtrC packetTypeName, TInt& aValue);
	static TBool				ReadTBTSyncPackets(CDataWrapper& aDataWrapper, TBTSyncPackets::TSyncPackets& aSyncPacket, const TDesC& aSection, const TDesC& aField);
	static TBool				ReadTBTeSCORetransmissionTypes(CDataWrapper& aDataWrapper, TBTeSCORetransmissionTypes& aRetransmissionType, const TDesC& aSection, const TDesC& aField);
	static TBool				GetLowPowerModeIntValue(TPtrC lowPowerModeName, TInt& aValue);
	static TBool				GetPhysicalLinkStateIntValue(TPtrC physicalLinkStateName, TInt& aValue);
	static void					GetTShutdown(CDataWrapper& aDataWrapper, RSocket::TShutdown& aShutdown, const TDesC& aSection, const TDesC& aField);

	static TBool				ReadSdpElementType(CDataWrapper& aDataWrapper, TSdpElementType& aType, const TDesC& aSection, const TDesC& aField);
	static MSdpElementBuilder*	ElementBuilderCastL(CDataWrapper& aDataWrapper, const TDesC& aObjectName, const TDesC& aTypeName);
	static CSdpAttrValueList*	AttrValueListCastL(CDataWrapper& aDataWrapper, const TDesC& aObjectName, const TDesC& aTypeName);
	static TUint				ConvertUUID32toInt(const TUUID& aUUID);

	// Uses a default value when not matched
	static void					GetLowPowerMode(CDataWrapper& aDataWrapper, const TDesC& aSection, TUint32& aModes);
	static TBool				GetShutdown(CDataWrapper& aDataWrapper, const TDesC& aSection, RSocket::TShutdown& aShutdown);
	static void					GetTBTPacketType(CDataWrapper& aDataWrapper, const TDesC& aSection, TInt& aType);
	static void					GetLinkStateNotifierBits(CDataWrapper& aDataWrapper, const TDesC& aSection, TUint32& aModes);

	// Informs when match not found
	static TBool				GetIntValue(TPtrC aConstantName, TInt& aValue);
	
	static TBool				ReadAvdtpServiceCategory(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpServiceCategory& aCategory);
	static TBool				ReadAvdtpRecoveryType(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TAvdtpRecoveryType& aRecoveryType);
	static TBool				ReadAvdtpSignallingErrorCode(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TAvdtpSignallingErrorCode& aErrorCode);
	static TBool				ReadGavdpSignallingErrorCode(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TGavdpSignallingErrorCode& aErrorCode);
	static TBool				ReadA2dpSignallingErrorCode(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TA2dpSignallingErrorCode& aErrorCode);
	static TBool				ReadVdpSignallingErrorCode(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TVdpSignallingErrorCode& aErrorCode);
	static TBool				ReadBluetoothMediaType(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, SymbianBluetoothAV::TBluetoothMediaType& aMediaType);
	static TBool				ReadAvdtpTransportSessionType(CDataWrapper& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpTransportSessionType& aSessionType);
	
	static TBool 				GetGavdpUserL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, MGavdpUser*& aData);
	static TBool 				GetSocketServL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, RSocketServ*& aData);
	static TBool 				GetSEIDL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TSEID*& aData);
	static TBool 				GetAvdtpServiceCategoriesL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpServiceCategories*& aData);
	static TBool 				GetAvdtpSEPInfoL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpSEPInfo*& aData);
	static TBool 				GetAvdtpServiceCapabilityL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TAvdtpServiceCapability*& aData);
	static TBool 				GetBTDevAddrL(CDataWrapperBase& aDataWrapper, const TDesC& aSectName, const TDesC& aKeyName, TBTDevAddr*& aData);

	};

#endif /* __T_BT_UTIL_H__ */
