/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the Audio Visualization Observer class.
*
*/



#ifndef MVISUALIZATIONOBSERVER_H
#define MVISUALIZATIONOBSERVER_H

class CVisualization;
// CLASS DECLARATION


class TVisualizationData
    {
    public:

        /**
        * Constructor.
        */
        TVisualizationData() { iFrequencyData = NULL, iWaveFormData = NULL; }

		TUint64 iTimeStamp;
		const TArray<TUint32>* iFrequencyData;
		const TArray<TInt16>* iWaveFormData;

    };


/**
*  Interface class to be implemented by objects that are interested in receiving notifications
*  from Audio Visualization object.
*
*  @since 3.0
*/
class MVisualizationObserver
    {
    public:

        /**
        * Invoked when the state of the visualization object has changed.
        * @since 3.0
        * @param aVisualization The visualization object that changed
        * @param aData The visualization data.
        */
        virtual void DataChanged( const CVisualization* aVisualization, TVisualizationData aData ) = 0;

     	/**
        * Invoked when the visualization data rate changed.
        * @since 3.0
        * @param aVisualization The visualization object that changed
        * @param aData The visualization data.
        */
        virtual void DataRateChanged( const CVisualization* aVisualization ) = 0;

     	/**
        * Invoked when the visualization frequency band count changed.
        * @since 3.0
        * @param aVisualization The visualization object that changed
        * @param aData The visualization data.
        */
        virtual void FrequencyBandCountChanged( const CVisualization* aVisualization ) = 0;

     	/**
        * Invoked when the visualization is preempted.
        * @since 3.0
        */
        virtual void VisualizationPreempted( ) = 0;

     	/**
        * Invoked when the visualization is resumed.
        * @since 3.0
        */
        virtual void VisualizationResumed( ) = 0;

     	/**
        * Invoked when the visualization is started.
        * @since 3.0
        */
        virtual void VisualizationStarted( ) = 0;

     	/**
        * Invoked when the visualization is stopped.
        * @since 3.0
        */
        virtual void VisualizationStopped( ) = 0;

     	/**
        * Invoked when the visualization the waveform data length changed.
        * @since 3.0
        * @param aVisualization The visualization object that changed
        * @param aData The visualization data.
        */
        virtual void WaveformDataLengthChanged( const CVisualization* aVisualization ) = 0;

    };

#endif      // MVISUALIZATIONOBSERVER_H

// End of File
