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
// 

/**
 * This file contains macros to allow the LTSY skeleton to be used with
 * the Mock LTSY.
 */

#ifndef __LTSYMACROS_H_
#define __LTSYMACROS_H_

	#ifdef BUILD_AS_LTSY_STUB
#define MOCKLTSYREQ(A)//to remove
		#define MTEST_ISSUE_MOCKLTSYENGINE_REQ(A, B, C, ARGS...)  A = iMockLtsy->ExtFuncL(B,C,##ARGS)
		#define MTEST_DECLARE_MOCKLTSYENGINE CMockLtsyEngine* iMockLtsy;
		#define MTEST_FORWARD_DECLARE_MOCKLTSYENGINE class CMockLtsyEngine;
		#define MTEST_DELETE_MOCKLTSYENGINE delete iMockLtsy
		#define MTEST_CREATE_MOCKLTSYENGINE(A)  iMockLtsy = CMockLtsyEngine::NewL(A);
		#define MTEST_INCLUDE_MOCKLTSYENGINE_HEADER "cmockltsyengine.h"
		#define MTEST_SET_MOCKLTSYENGINE(A) A->iMockLtsy = iMockLtsy
		#define MTEST_QUERY_SUPPORT_MOCKLTSYENGINE(A,B) return iMockLtsy->IsInterfaceSupported(A, B)
		#define MTEST_QUERY_IND_SUPPORT_MOCKLTSYENGINE(A,B, C) return iMockLtsy->IsCallbackIndSupported(A, B, C)
		#define MTEST_DECLARE_EVENT_COUNTER TInt iCounter;
//all we are doing is is simulating "events from received from baseband occuring at interval of 5s.
		#define MTEST_WAIT_FOR_NEXT_EVENT	User::After(500000);iCounter++;	
				
		#define MTEST_LOG_RECEIVE_EVENT_NO LOG(_L8("LtsyReceiveThread simulated event = %d"), iCounter);
		#define MTEST_COUNTER_EXCEEDED_SHUTDOWN_RECEIVETHREAD   if(iCounter>3) err = KShutdownLtsyReceiveThread;

	#else
#define MOCKLTSYREQ(A) //to remove
		#define MTEST_ISSUE_MOCKLTSYENGINE_REQ(A, B, C, ARGS...)
		#define MTEST_DECLARE_MOCKLTSYENGINE 
		#define MTEST_FORWARD_DECLARE_MOCKLTSYENGINE
		#define MTEST_DELETE_MOCKLTSYENGINE
		#define MTEST_CREATE_MOCKLTSYENGINE(A)
		#define MTEST_INCLUDE_MOCKLTSYENGINE_HEADER <e32std.h> //does not reallymatter what header is put here
		#define MTEST_SET_MOCKLTSYENGINE(A)
		#define MTEST_QUERY_SUPPORT_MOCKLTSYENGINE(A,B)
		#define MTEST_QUERY_IND_SUPPORT_MOCKLTSYENGINE(A,B, C)
		#define MTEST_DECLARE_EVENT_COUNTER
		#define MTEST_WAIT_FOR_NEXT_EVENT
		#define MTEST_LOG_RECEIVE_EVENT_NO
		#define MTEST_COUNTER_EXCEEDED_SHUTDOWN_RECEIVETHREAD   
	#endif

#endif // __LTSYMACROS_H_
