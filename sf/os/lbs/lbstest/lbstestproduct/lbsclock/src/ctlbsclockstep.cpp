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
// @file ctlbsclockstep.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsclockstep.h"


const TInt KAssistanceDataProviderPluginUidValue = 0x10281D77;

#define REFPOS_LAT				2
#define REFPOS_LONG				3
#define REFPOS_ALT				10
#define REFPOS_HORZ_ACCURACY	100
#define REFPOS_VERT_ACCURACY	100


CT_LbsClockStep::~CT_LbsClockStep()
	{
	}


CT_LbsClockStep::CT_LbsClockStep(CT_LbsClockServer& aParent) : CTestStep(), iParent(aParent)
	{
	}

void CT_LbsClockStep::ConstructL()
	{
	}


TInt CT_LbsClockStep::OpenNetSim()
	{
	// Connect to net sim.
	TInt err = iNetSim.ConnectL(NULL);
	if (err)
		{
		return err;
		}


	// Set the reference position, which also be used for any required verification.
	iRefPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
	iRefPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
	iRefPos.SetCurrentTime();

	if (!iNetSim.SetReferenceLocation(iRefPos))
		{
		iNetSim.Close();

		return KErrGeneral;
		}	
		

	// Set plugin to use.
	TUid pluginUid = TUid::Uid(KAssistanceDataProviderPluginUidValue);
	if (!iNetSim.SetAssistanceDataProvider(pluginUid))
		{
		iNetSim.Close();

		return KErrGeneral;
		}
		
	return err;
	}


void CT_LbsClockStep::CloseNetSim()
	{
	iNetSim.Close();	
	}	

