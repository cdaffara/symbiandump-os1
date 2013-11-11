// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the Spud tests related to R5 Qos parameters
// 
//

/**
 @file
 @internalComponent
*/

#ifndef TE_SPUD_NETWORK_SIDE_R5STEPS_H
#define TE_SPUD_NETWORK_SIDE_R5STEPS_H

#include "te_spudNetworkSideSteps.h"
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#include <networking/umtsextn.h>
#include <networking/imsextn.h>
#endif

using namespace te_spudNetworkSide;
	
	class CSpudQosR5TestBase : public CSpudNetSideTestBase
		{
		
	protected:
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
		void SetIMSParameters(TImsParameter& aParameters);
		void SetQoSR5Parameters(TUmtsR5QoSParameters& aParameters);
		void ChangeQoSR5Parameters(TUmtsR5QoSParameters& aParameters);
		virtual void ChangeQoSL();
#endif
		
		virtual void InitiateSecondaryStartL();
		};
		
	/** Define a basic declaration for testClass as a subclass of CSpudNetSideTestBase 
	and define a literal from testClass */
	#define SYMBIAN_SPUD_TEST_CLASSR5(testClass) SYMBIAN_TEST_CLASS(testClass) : public CSpudQosR5TestBase \
				{ \
			public: \
				inline testClass() {SetTestStepName(K##testClass);}; \
				virtual TVerdict RunTestStepL(); \
				}; 
				
			
	//R5 Qos parameters testing
	SYMBIAN_SPUD_TEST_CLASSR5(CSpudQosR5Secondary)
#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	SYMBIAN_SPUD_TEST_CLASSR5(CSpudSecondaryQosR5Event)
	SYMBIAN_SPUD_TEST_CLASSR5(CSpudSecondaryChangeR5QoS)
#endif
	SYMBIAN_SPUD_TEST_CLASSR5(CSpudDeletePrimaryR5Qos)	
	
#endif 
