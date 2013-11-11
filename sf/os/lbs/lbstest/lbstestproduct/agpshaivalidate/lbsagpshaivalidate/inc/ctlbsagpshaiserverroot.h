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
// @file ctlbsagpshaiserverroot.h
// This contains the header file for LBS AGPS HAI Validation Test Server
// 
//

#ifndef __CTLBSAGPSHAISERVERROOT_H__
#define __CTLBSAGPSHAISERVERROOT_H__

// System includes
#include <e32def.h>
#include <e32cmn.h>

#include <test/testexecuteserverbase.h>

// User includes

// LBS includes
#include <lbscommon.h>

// Implements the LBS Client Test server
class CT_LbsAGPSHAIServerRoot : public CTestServer
	{
public :
	CT_LbsAGPSHAIServerRoot();
	virtual ~CT_LbsAGPSHAIServerRoot();
protected:
	void ConstructL();
public:
	/**
	iScheduler is a pointer to the active scheduler for use in the tests
	@see CActiveScheduler
	*/ 
	CActiveScheduler* iScheduler;
	
	/**
	iModuleInfoQuality is the quality reported by the HAI module via it's module info (.ini) file
	*/
	TPositionQuality iModuleInfoQuality;	
	
private:
	// abstract
	virtual const TDesC& ServerName() = 0;
	};

#endif //__CTLBSAGPSHAISERVERROOT_H__
