// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// The frame rate delay used in CRateLimiterRenderStage
const TInt KFrameRateDelay = 500000; // 0.5 seconds

//No of test frames used in CWsRateLimitGraphicDrawer
const TInt KMaxFrames = 40;

// Approximate length of the animation
const TInt KAnimLength = 20000000; // 20 seconds

// 1 Second in microseconds
const TInt KOneSecondInMicroSecs = 1000000;

// Expected framerate without any additional rate limiting
const TReal KStdMinimumFrameRate = 4;

// Allowed error when testing framerate without any additinal rate limiting
const TReal KStdErrorThreshHold = 0.2;


#endif /*CONSTANTS_H_*/
