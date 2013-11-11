// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file atcommandtype.h
// 

#ifndef ATCOMMANDTYPE_H_
#define ATCOMMANDTYPE_H_

//Identifer at command class

const TInt KLtsyGeneralAtCommandTypeUnitId = 0;
const TInt KLtsyCallControlAtCommandTypeUnitId = 200;
const TInt KLtsyPhoneAtCommandTypeUnitId = 400;
const TInt KLtsySecurityAtCommandTypeUnitId = 600;
const TInt KLtsyCellBroadcastAtCommandTypeUnitId = 800;
const TInt KLtsyPhonebookOnTypeUnitId = 1000;
const TInt KLtsyPhonebookTypeUnitId = 1200;
const TInt KLtsySimTypeUnitId = 1400;
const TInt KLtsySmsTypeUnitId = 1600;
const TInt KLtsyCallControlMultipartyTypeUnitId = 1800;
const TInt KLtsySupplementaryServicesTypeUnitId = 2000;
const TInt KLtsyPacketServicesTypeUnitId = 2200;
const TInt KLtsySatTypeUnitId = 2400;

enum TLtsyATCommandType
	{
	/*Not Use*/
	ELtsyAt_User_NotUse = -1,
	/*General*/
	ELtsyAT_General_AT = KLtsyGeneralAtCommandTypeUnitId + 1,	//AT
	ELtsyAT_General_WIND = KLtsyGeneralAtCommandTypeUnitId + 2,	//+WIND (Unsolicited command)
	ELtsyAT_General_CLCC = KLtsyGeneralAtCommandTypeUnitId + 3, //AT+CLCC (list current call information)
	
	/*Call Control*/
	ELtsyAT_Call_ATD = KLtsyCallControlAtCommandTypeUnitId + 1,		//ATD
	ELtsyAT_Call_ATH = KLtsyCallControlAtCommandTypeUnitId + 2,     //ATH
	ELtsyAT_Call_RING = KLtsyCallControlAtCommandTypeUnitId + 3,    //RING or +CRING : *
	ELtsyAT_Call_CHLD = KLtsyCallControlAtCommandTypeUnitId + 4,    //CHLD(for use release/hold/resume/swap a call)
	
	/*Phone*/
	ELtsyAT_Phone_ModemStatus      = KLtsyPhoneAtCommandTypeUnitId + 1,
	ELtsyAT_Phone_NwkRegStatus     = KLtsyPhoneAtCommandTypeUnitId + 2,
	ELtsyAT_Phone_SimCardStatus    = KLtsyPhoneAtCommandTypeUnitId + 3,
	ELtsyAT_Phone_NwkInfo          = KLtsyPhoneAtCommandTypeUnitId + 4,
	ELtsyAT_Phone_GetSignal        = KLtsyPhoneAtCommandTypeUnitId + 5,
	ELtsyAT_Phone_GetBattery       = KLtsyPhoneAtCommandTypeUnitId + 6,
	ELtsyAT_Phone_SMSSetting1      = KLtsyPhoneAtCommandTypeUnitId + 7,
	ELtsyAT_Phone_SMSSetting2      = KLtsyPhoneAtCommandTypeUnitId + 8,
	ELtsyAT_Phone_RegistNwk        = KLtsyPhoneAtCommandTypeUnitId + 9,
	ELtsyAT_Phone_GPRSRegNwk       = KLtsyPhoneAtCommandTypeUnitId + 10,
	ELtsyAT_Phone_SetWind          = KLtsyPhoneAtCommandTypeUnitId + 11,
	ELtsyAT_Phone_GetImei          = KLtsyPhoneAtCommandTypeUnitId + 12,
	ELtsyAT_Phone_GetManufacturer  = KLtsyPhoneAtCommandTypeUnitId + 13,
	ELtsyAT_Phone_GetModel         = KLtsyPhoneAtCommandTypeUnitId + 14,
	ELtsyAT_Phone_GetRevision      = KLtsyPhoneAtCommandTypeUnitId + 15,
	ELtsyAT_Phone_SetOperFormat    = KLtsyPhoneAtCommandTypeUnitId + 16,
	ELtsyAT_Phone_CheckFdnSupport  = KLtsyPhoneAtCommandTypeUnitId + 17,
	ELtsyAT_Phone_GetFdnStatus     = KLtsyPhoneAtCommandTypeUnitId + 18,
	ELtsyAT_Phone_SetTDRSingal     = KLtsyPhoneAtCommandTypeUnitId + 19,
	/*Security*/
	
	/*Cell Broad Cast*/
	
	/*Phone book On*/
	
	/*Phone book*/
	
	ELtsyAT_Phonebook_SmsStoreGetInfo=KLtsyPhonebookTypeUnitId  + 1,
	/*Sim*/
	
	/*Sms*/
	ELtsyAT_Sms_GetMessageStoreInfo = KLtsySmsTypeUnitId + 1,
	
	ELtsyAT_Sms_ReceiveMessage = KLtsySmsTypeUnitId+2,
	/*Call Control Multiparty*/
	
	/*Supplementary Services*/
	
	/*PacketServices*/
	ELtsyAT_Packet_ActivateContext          = KLtsyPacketServicesTypeUnitId + 1,
	ELtsyAT_Packet_GetContextAddr           = KLtsyPacketServicesTypeUnitId + 2,
	ELtsyAT_Packet_SwitchOnlineMode         = KLtsyPacketServicesTypeUnitId + 3,
	ELtsyAT_Packet_HangUp                   = KLtsyPacketServicesTypeUnitId + 4,
	ELtsyAT_Packet_DeactivateContext        = KLtsyPacketServicesTypeUnitId + 5,
	ELtsyAT_Packet_ConnectContext           = KLtsyPacketServicesTypeUnitId + 6,
	ELtsyAT_Packet_DeleteContext1           = KLtsyPacketServicesTypeUnitId + 7,
	ELtsyAT_Packet_DeleteContext2           = KLtsyPacketServicesTypeUnitId + 8,
	ELtsyAT_Packet_DeleteContext3           = KLtsyPacketServicesTypeUnitId + 9,
	ELtsyAT_Packet_DeleteContext4           = KLtsyPacketServicesTypeUnitId + 10
	/*Sat*/
	};

#endif /*ATCOMMANDTYPE_H_*/
