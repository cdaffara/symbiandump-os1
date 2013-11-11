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

/**
 @file
 @internalComponent
*/

#ifndef HCTL_BCSP_DEBUG_H
#define HCTL_BCSP_DEBUG_H

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCTL_BCSP);
#endif

#ifdef _DEBUG
#	define __DEBUG_FLOG_RAW_UART_ //enable UART Flogging
#	define __DEBUG_FLOG_STACK_TX_ //enable UART Flogging
#	define __DEBUG_RxDecodedFrame__ //Enable flogging for RxDecodedFrame
#	define __DEBUG_TxDatagramDecodedFrame__ //Enable flogging for RxDecodedFrame
#	define __DEBUG_PacketRouter__ //Enable Flogging for PacketRouter and Processing
#	define __DEBUG_FLOG_RX_ //Enable RX Flogging in CHCTLBcspReceiver::ProcessDataL()
#	define __DEBUG_SEQ_VERBOSE__ //Enable flogging for Sequencer
#	define __DEBUG_SEQ_Values__ //Enable flogging of Sequencer Ack Seq etc.
#else
#	undef __DEBUG_FLOG_RAW_UART_ //enable UART Flogging
#	undef __DEBUG_FLOG_STACK_TX_ //enable UART Flogging
#	undef __DEBUG_RxDecodedFrame__ //Enable flogging for RxDecodedFrame
#	undef __DEBUG_TxDatagramDecodedFrame__ //Enable flogging for RxDecodedFrame
#	undef __DEBUG_PacketRouter__ //Enable Flogging for PacketRouter and Processing
#	undef __DEBUG_FLOG_RX_ //Enable RX Flogging in CHCTLBcspReceiver::ProcessDataL()
#	undef __DEBUG_SEQ_VERBOSE__ //Enable flogging for Sequencer
#	undef __DEBUG_SEQ_Values__ //Enable flogging of Sequencer Ack Seq etc.
#endif

#endif //HCTL_BCSP_DEBUG_H
