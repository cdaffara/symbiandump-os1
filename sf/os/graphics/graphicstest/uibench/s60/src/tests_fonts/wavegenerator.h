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

#ifndef WAVEGENERATOR_H
#define WAVEGENERATOR_H


#include <e32cmn.h> 


class WaveGenerator
    {
public:    
    typedef RArray<TReal> TValueList;

public:
    static void GenerateSineWave(TValueList& aResults, TInt aNumSamples, TReal aAmplitude, TReal aFrequency, TBool aPositiveValues);
    };


#endif /* WAVEGENERATOR_H */
