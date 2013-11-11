// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(D_PARAMS_H)
#define D_PARAMS_H


#include <d32comm.h>

// For common dummy parameters between TE_C32 and ECOMMTST

#define DUMMYFLOWCONTROLSTATUS	EFlowControlOn

#define DUMMYSIGNALS		(KSignalRTS|KRTSChanged)

#define DUMMYSIGNALMASK		KSignalDTEOutputs



#define DUMMYCHANGED		(KRateChanged|KDataFormatChanged|KHandshakeChanged)
#define DUMMYCONFIGRATE 	EBps9600
#define DUMMYDATABITS	 	EData8
#define DUMMYSTOPBITS	 	EStop1
#define DUMMYPARITY	 		EParityNone
#define DUMMYHANDSHAKE	 	KConfigObeyCTS

#endif

