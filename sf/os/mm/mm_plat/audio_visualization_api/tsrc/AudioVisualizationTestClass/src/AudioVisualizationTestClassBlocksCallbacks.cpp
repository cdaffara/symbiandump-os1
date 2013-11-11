/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  AudioVisualization test module
*
*/



// INCLUDE FILES
#include "AudioVisualizationTestClass.h"
#include "debug.h"


void CAudioVisualizationTestClass::DataChanged( const CVisualization* aVisualization, TVisualizationData aData )
	{
    iLog->Log(_L("DataChanged, return code =%x!!!"),aVisualization);
    iLog->Log(_L("DataChanged, return code =%x!!!"),aData);
	}
void CAudioVisualizationTestClass::DataRateChanged( const CVisualization* aVisualization )
	{
    iLog->Log(_L("DataRateChanged, return code =%x!!!"),aVisualization);
	}
void CAudioVisualizationTestClass::FrequencyBandCountChanged( const CVisualization* aVisualization )
	{
    iLog->Log(_L("FrequencyBandCountChanged, return code =%x!!!"),aVisualization);
	}
void CAudioVisualizationTestClass::VisualizationPreempted( )
	{

	}
void CAudioVisualizationTestClass::VisualizationResumed( )
	{

	}
void CAudioVisualizationTestClass::VisualizationStarted( )
	{

	}
void CAudioVisualizationTestClass::VisualizationStopped( )
	{

	}
void CAudioVisualizationTestClass::WaveformDataLengthChanged( const CVisualization* aVisualization )
	{
    iLog->Log(_L("WaveformDataLengthChanged, return code =%x!!!"),aVisualization);
	}



/************************************************************************************************************/


// End of File
