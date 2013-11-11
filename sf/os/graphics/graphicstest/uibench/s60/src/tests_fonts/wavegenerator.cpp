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


#include "wavegenerator.h"

#include <e32math.h>


/**
Generates a set of values that resembles a sine wave that has no negative 
values.
*/
void WaveGenerator::GenerateSineWave(TValueList& aResults, TInt aNumSamples,
        TReal aAmplitude, TReal aFrequency, TBool aPositiveValues)
    {
    TReal prefix = -1;
    TReal part = 1.0 / aNumSamples;
    TReal time = 0;
    TReal preCalculation = 3.14159265358979323846 * aFrequency;
    if (aPositiveValues)
        {
        prefix = 1;
        }
    TReal sinResult;
    for (TInt i = aNumSamples; i > 0; --i)
        {
        time += part;
        Math::Sin(sinResult, preCalculation * time);
        aResults.Append(prefix * Abs(aAmplitude * sinResult));
        }
    }
