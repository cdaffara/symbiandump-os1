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

/**
 @file
 @internalComponent 
*/

#ifndef __TETELCS_H
#define __TETELCS_H


enum
	{
	ETestEtelNarrowSetAndGet=1000000,  // offset from core ETel IPC
	ETestEtelUnicodeSetAndNarrowGet,
	ETestEtelUnicodeSetAndGet,
	ETestEtelNarrowSetAndUnicodeGet,
	ETestEtelDoubleUnicodeGet,
	ETestCustomIPC1=1000055,
	ETestCustomIPC2=1000056,
	ETestCustomIPC3=1000099,
	ETestCustomIPC4=1000100,
	ETestCustomIPC5=1000499,
	ETestCustomIPC6=1000500,
	ETestCustomIPC7=1000599,
	ETestCustomIPC8=1001000,
	ETestCustomIPC9=999999
	};

#endif
