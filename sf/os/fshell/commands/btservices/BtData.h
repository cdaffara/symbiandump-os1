// BtData.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef BTDATA_H_
#define BTDATA_H_

struct TUUidPair
	{
	TUint32 uuid;
	TPtrC txt;
	};

const TUUidPair UuidAsArray[] = 
	{
			{0x0001,_L("SDP") },
			{0x0003,_L("RFCOMM") },
			{0x0005,_L("TCS-BIN") },
			{0x0100,_L("L2CAP") },
			{0x0009,_L("IP") },
			{0x0002,_L("UDP") },
			{0x0004,_L("TCP") },
			{0x0006,_L("TCS-AT") },
			{0x0008,_L("OBEX") },
			{0x000A,_L("FTP") },
			{0x000C,_L("HTTP") },
			{0x000E,_L("WSP") },
			{0x1000,_L("ServiceDiscoveryServerServiceClassID") },
			{0x1001,_L("BrowseGroupDescriptorServiceClassID") },
			{0x1002,_L("PublicBrowseGroup") },
			{0x1101,_L("SerialPort") },
			{0x1102,_L("LANAccessUsingPPP") },
			{0x1103,_L("DialupNetworking") },
			{0x1104,_L("IrMCSync") },
			{0x1105,_L("OBEXObjectPush") },
			{0x1106,_L("OBEXFileTransfer") },
			{0x1107,_L("IrMCSyncCommand")},
			{0x1108,_L("Headset")},
			{0x1109,_L("CordlessTelephony")},
			{0x1110,_L("Intercom")},
			{0x1111,_L("Fax")},
			{0x1112,_L("HeadsetAudioGateway")},
			{0x1113,_L("WAP")},
			{0x1114,_L("WAP_CLIENT")},
			{0x1200,_L("PnPInformation")},
			{0x1201,_L("GenericNetworking")},
			{0x1202,_L("GenericFileTransfer")},
			{0x1203,_L("GenericAudio")},
			{0x1204,_L("GenericTelephony")}				
	};

#endif
