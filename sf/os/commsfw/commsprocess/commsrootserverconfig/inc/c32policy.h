// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Root Server Security Policy Definitions
//

/**
@file
@internalComponent
*/
#include <rsshared.h>
//#include <c32Exe.h>
using namespace RootServer;

/** The Symbian Vendor ID. */
const TUint KSymbianVID = 0x70000001;

const TUint PolicyRangeCount = 17 ;

// IPC ranges

const TInt PolicyRanges[PolicyRangeCount] = 
	{
	0,							// Must start at zero to prevent CPolicyServer from panicing
	RSIsCallerConfigurator,		// NetworkControl				: range is 1 - 2
	RSUnloadModule,				// NetworkControl + PowerMgmt	: range is 3 only
	RSBind,						// NetworkControl				: range is 4 only
	RSUnbind,					// NetworkControl + PowerMgmt	: range is 5 only
	RSGetModuleInfo,			// NetworkControl				: range is 6 - 10
	RSCancelUnloadModule,		// NetworkControl + PowerMgmt	: range is 11 only
	RSCancelBind,				// NetworkControl				: range is 12 only
	RSCancelUnbind,				// NetworkControl + PowerMgmt	: range is 13 only
	RSCloseSession,				// NetworkControl				: range is 14 only
	RSShutdown,					// NetworkControl + PowerMgmt	: range is 15 only
	RSSendMessage,				// NetworkControl				: range is 16 only
	RSDbgMarkHeap,				// VID+NetworkControl 			: range is 17 - 20
	RSDbgFailNext +1,			// fail 						: range is 21 - 100
	RSSetMBufPoolSize,			// NetworkControl 				: range is 101 - 102
	RSDbgFailNextMbuf, 			// VID+NetworkControl 			: range is 103 - 107
	RSDbgMbufTotalSpace + 1		// fail (to KMaxTInt)*/			: range is 108 -  KMaxTInt
	};

// Index numbers into PolicyElements[]

const TInt policyNetworkControl		  = 0;
const TInt policyVIDandNetworkControl = 1;
const TInt policyPass				  = 2;
const TInt policyPowerNetworkControl  = 3; 

// Mapping of IPCs to policy elements
//
// PowerMgmt:  for IPCs that can cause termination of CPMs.
// SymbianVID: for debug IPCs - restricted to Symbian test code (in fact, they should be DEBUG
// 			   only, but adding the policy for them here is fail-safe).

const TUint8 PolicyElementsIndex[PolicyRangeCount] = 
	{
	CPolicyServer::ENotSupported,	// For fake IPC 0 - RS IPC calls start at RSSetMBufPoolSize
	policyNetworkControl,		// RSIsCallerConfigurator (1)
								// RSLoadModule (2)
	policyPowerNetworkControl,	// RSUnloadModule (3)
	policyNetworkControl,		// RSBind (4)
	policyPowerNetworkControl,	// RSUnbind (5)
	policyNetworkControl,		// RSGetModuleInfo (6)
								// RSEnumerateModules (7)
								// RSEnumerateSubModules (8)
								// RSEnumerateBindings (9)
								// RSCancelLoadModule (10)
	policyPowerNetworkControl,	// RSCancelUnloadModule (11)
	policyNetworkControl,		// RSCancelBind (12)
	policyPowerNetworkControl,	// RSCancelUnbind (13)
	policyNetworkControl,		// RSCloseSession (14)
	policyPowerNetworkControl,	// RSShutdown (15)
	policyNetworkControl,       // RSSendMessage (16)
	policyVIDandNetworkControl,	// RSDbgMarkHeap (17)
								// RSDbgCheckHeap (18)
								// RSDbgMarkEnd (19)
								// RSDbgFailNext (20)
	CPolicyServer::ENotSupported,  // RSDbgFailNext + 1 to RSSetMBufPoolSize -1 						
	policyNetworkControl, 		// RSSetMBufPoolSize (101)
								// RSAddMBufAllocInfo (102)
	policyVIDandNetworkControl, // RSDbgFailNextMbuf (103)
								// RSDbgSetMbufPoolLimit (104)
								// RSDbgCheckMbuf (105)
								// RSDbgMbufFreeSpace (106)
								// RSDbgMbufTotalSpace (107)
	
	CPolicyServer::ENotSupported	// RSDbgMbufTotalSpace + 1 to KMaxTInt
	};

// Individual policy elements
	
const CPolicyServer::TPolicyElement PolicyElements[] = 
	{
	{ _INIT_SECURITY_POLICY_C1(ECapabilityNetworkControl), CPolicyServer::EFailClient },
	{ _INIT_SECURITY_POLICY_V1(KSymbianVID,ECapabilityNetworkControl), CPolicyServer::EFailClient },
	{ _INIT_SECURITY_POLICY_PASS },
	{ _INIT_SECURITY_POLICY_C2(ECapabilityNetworkControl, ECapabilityPowerMgmt), CPolicyServer::EFailClient },
	};

// Main policy
	
const CPolicyServer::TPolicy CC32RootServerPolicy =
	{
	policyNetworkControl,				//  all connect attempts require NetworkControl (all IPCs require it)
	PolicyRangeCount,					
	PolicyRanges,
	PolicyElementsIndex,
	PolicyElements,
	};

