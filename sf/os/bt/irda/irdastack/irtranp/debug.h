// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32svr.h>

// The bottom define enables/disables debbuging in the IrTranp Protocol
//#define _DEBUGTRANP_

#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef _DEBUGTRANP_ // define debug macros
	
	// These defines enable/disable various levels of debugging
	#define DEBUG_COMREADWRITE		// IrComm read write layer
	#define DEBUG_SCEP				// SCEP layer
	#define DEBUG_BFTP				// BFTP layer
	#define DEBUG_SESSION			// Session Layer

	#define DEBUG_OPEN() {RDebug::Open();} // Creates connection to Debug port	
#else
	#define DEBUG_OPEN()
#endif  //_DEBUGTRANP_

#ifdef DEBUG_SCEP // enables debuging in SCEP layer
	#define SCEPPRINT(a) {RDebug::Print(a);}
	#define SCEPPRINT_2(a,b) {RDebug::Print(a,b);}
	#define SCEPPRINT_3(a,b,c) {RDebug::Print(a,b,c);}
#else
	#define SCEPPRINT(a)
	#define SCEPPRINT_2(a,b)
	#define SCEPPRINT_3(a,b,c)
#endif //DEBUG_SCEP

#ifdef DEBUG_COMREADWRITE
	#define COMRWPRINT(a) {RDebug::Print(a);}
	#define COMRWPRINT_2(a,b) {RDebug::Print(a,b);}
	#define COMRWPRINT_3(a,b,c) {RDebug::Print(a,b,c);}
#else
	#define COMRWPRINT(a)
	#define COMRWPRINT_2(a,b)
	#define COMRWPRINT_3(a,b,c)
#endif //DEBUG_COMREADWRITE

#ifdef DEBUG_BFTP
	#define BFTPPRINT(a) {RDebug::Print(a);}
#else
	#define BFTPPRINT(a)
#endif //DEBUG_BFTP

#ifdef DEBUG_SESSION
	#define SESSIONPRINT(a) {RDebug::Print(a);}
	#define SESSIONPRINT_2(a,b) {RDebug::Print(a,b);}
#else
	#define SESSIONPRINT(a)
	#define SESSIONPRINT_2(a,b)
#endif //DEBUG_SESSION


#endif //_DEBUG_H_
