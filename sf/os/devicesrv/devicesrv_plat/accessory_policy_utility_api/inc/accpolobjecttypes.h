/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Consts and definitions for audio device topology.
*
*/

#ifndef ACCPOLOBJECTTYPES_H
#define ACCPOLOBJECTTYPES_H

/**
* Object types for audio stream format container.
*/
enum TStreamConObjectType
    {        
    EAccPolStreamObjectCon = 0,
    EAccPolAudioInputStreamObject,
    EAccPolAudioOutputStreamObject    
    };
    
/**
* Object types for audio topology container.
*/
enum TTopologyConObjectType
    {    
    EAccPolTopologyObjectCon = 0,    
    EAccPolAudioInputTerminalObject,
    EAccPolAudioOutputTerminalObject,
    EAccPolAudioMixerUnitObject,
    EAccPolAudioFeatureUnitObject,
    EAccPolAudioSelectorUnitObject,
    EAccPolAudioVolumeControlObject,
    EAccPolAudioMuteControlObject,
    EAccPolAudioInputControlObject
    };

/**
* Object types for HDMI container. 
*/
enum THdmiConObjectType
    {
    EAccPolHdmiObjectCon = 0,
    EAccPolHdmiSinkObject,
    EAccPolHdmiAudioFormatObject,
    EAccPolHdmiLatencyObject,
    EAccPolHdmiSpeakerAllocationObject,
    EAccPolHdmiVideoFormatObject
    };

#endif // ACCPOLOBJECTTYPES_H
