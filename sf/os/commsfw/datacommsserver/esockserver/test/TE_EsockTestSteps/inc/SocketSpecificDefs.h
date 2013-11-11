/**
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
* This file define all the socket specific constants
* 
*
*/



/**
 @file SocketSpecificDefs.h
*/
#if (!defined SOCKETSPECIFICDEFS_H)
#define SOCKETSPECIFICDEFS_H

_LIT(KTe_SocketSectionName,"Socket");
_LIT(KTe_SocketName,"SocketName");
_LIT(KTe_ProtocolName,"Protocol");
_LIT(KTe_SourceAddressName,"SrcAddr");
_LIT(KTe_SourcePortName,"SrcPort");
_LIT(KTe_DestinationAddressName,"DstAddr");
_LIT(KTe_DestinationPortName,"DstPort");
_LIT(KTe_PacketSizeName,"PacketSize");
_LIT(KTe_NrOfPacketsName,"NrOfPackets");
_LIT(KTe_ShutdownTypeName,"ShutdownType");
_LIT(KTe_BearerName,"Bearer");
_LIT(KTe_IAPIndexName,"IAP");
_LIT(KTe_NetworkIndexName,"NET");
_LIT(KTe_SNAPIndexName,"SNAP");

_LIT(KTe_ShutdownNormal,"ENormal");
_LIT(KTe_ShutdownStopInput,"EStopInput");
_LIT(KTe_ShutdownStopOutput,"EStopOutput");
_LIT(KTe_ShutdownImmediate,"EImmediate");
_LIT(KTe_TCPName,"TCP");
_LIT(KTe_UDPName,"UDP");

_LIT(KTe_AddSocketSectionName,"AddSocket");
_LIT(KTe_RemoveSocketSectionName,"RemoveSocket");

#endif //SOCKETSPECIFICDEFS_H

