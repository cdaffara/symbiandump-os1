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
// testvideoplayer_secdisp.h
// Header file: Video Player Tests.
// 
//

#ifndef TESTVIDEOPLAYER_SECDISP_H
#define TESTVIDEOPLAYER_SECDISP_H

#include "tsi_mvs_agents_stepbase.h"

/**
 * Set the screen id and Play the video file.
 * Set the invalid screen id and Play the video file.
 *
 * RTestMVSVideoAgentSetScrIdAndPlay
 * MM-MVS-VIDEOAGENT-I-4001, MM-MVS-VIDEOAGENT-I-5001
 *
 */ 

class RTestMVSVideoAgentSetScrIdAndPlay : public RTestMVSVideoAgentSetScrIdAndPlayBase
    {
public:
    static RTestMVSVideoAgentSetScrIdAndPlay* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError,TInt aScreenNumber);
   	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	
	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode);

private:
    RTestMVSVideoAgentSetScrIdAndPlay(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError,TInt aScreenNumber);

protected:
    };
  
 /**
 * Set the screen id during play and Play the video file.
 * Set the invalid screen id and Play the video file.
 *
 * RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile
 * MM-MVS-VIDEOAGENT-I-4002, MM-MVS-VIDEOAGENT-I-5001
 *
 */
class RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile : public RTestMVSVideoAgentSetScrIdAndPlayBase
    {
public:
    static RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile* NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError,TInt aScreenNumber);
   	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	
	//State Machine Functions
	void FsmL(TMvsTestAgentEvents aEventCode);

private:
    RTestMVSVideoAgentSetScrIdDuringPlayAndPlayVideoFile(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, TInt aExpectedError,TInt aScreenNumber);

protected:
	TInt 								iCountNumber;
    };    
         

#endif //TESTVIDEOPLAYER_SECDISP_H
