/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This header provides the common Name/Value defintions of Name/Value 
*                   pairs.
*
*/



#ifndef ACCPOLCOMMONNAMEVALUEPAIRS_H
#define ACCPOLCOMMONNAMEVALUEPAIRS_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

// Maximum length for name definitions - don't exceed this or a panic will occur
const TInt KSBMaxNameLength ( 45 );

// Maximum length for value definitions - don't exceed this or a panic will occur
const TInt KSBMaxValueLength ( 10 );

// Accessory capabilities can be described as name/value pairs. To use accessorys
// capabilities from generic identification, the user needs constants. These 
// constants are referenced with name/value pairs.
// Name constants are defined to a certain numerical space. 
// In this numerical space each of these constats is presented with a term called
// "name const literal". E.g. For name KAccAcousticEchoControl name const literal is
// "1#1#1".
// Accessory capabilities can be described the following values: name, value type,
// dynamic type, I/O type, description and bitmask.
// Name:           Acoustic Echo Control
// --------------> Name of the capability 
// Value Type:     TBool
// --------------> Type of the value
// Dynamic Type:   Dynamic/Static
// --------------> Indicates if value can change while accessory is connected.
//                 If value is dynamic type the client should registrate for 
//                 listening indications for value changes.
// I/O Type:       From accessory to terminal, input.
// --------------> I/O type indicates the direction how the state update is done. 
//                 E.g.output indicated that user can use setValueL method. If
//                 accessory updates the value I/O type is input and if it done
//                 by the terminal I/O type is output.
// Description:    Accessory supports acoustic echo control. If value is true terminal
//                 must disable its own echo control. To indicate if accessory has 
//                 internal acoustic echo control.
// --------------> Description of the capability.
// Bitmask:        00000000000000000000000000000001
// --------------> The bitmask for the capability.
//
// Format of capability:                                                 
// =====================                                                 
//                                                                       
// 0b 000001 00000 00010                                                 
//    ------ ----- -----                                                 
//    |      |     |                                                     
//    |      |     |_5 bits for capability. Tells which bit is one (*).  
//    |      |_5 bits for namespace,                                     
//    |_6 bits for capability group. Tells which bit is one (*).         
//                                                                       
// (*):                                                                  
// 0b00 -> bit number 0 is one -> value is 0b0001=1                      
// 0b01 -> bit number 1 is one -> value is 0b0010=2                      
// 0b10 -> bit number 2 is one -> value is 0b0100=4                      
// 0b11 -> bit number 3 is one -> value is 0b1000=8                      
//                                                                       


// COMMON AUDIO SB NAME-VALUE PAIRS:

// Name:          Acoustic Echo Control
// Value Type:    TBool
// Dynamic Type:  Dynamic
// I/O Type:      From accessory to terminal, input.
// Description:   Accessory supports acoustic echo control. If value is true
//                terminal must disable its own echo control. To indicate if
//                accessory has internal acoustic echo control.
// Bitmask:       000000 00000 00000
const TUint32 KAccAcousticEchoControl = 0x0000;


// Name:          Noise reduction.
// Value:         TBool
// Dynamic Type:  Dynamic
// I/O:           From accessory to terminal, input.
// Description:   To indicate if accessory has internal noise reduction. 
//                If value is true terminal must disable its own noise reduction.
// Bitmask:       000000 00000 00001
const TUint32 KAccNoiseReduction = 0x0001;

// Name:          Stereo audio output
// Value:         N/A
// Dynamic Type:  Static
// I/O:           N/A
// Description:   To indicate if accessory has stereo audio output.
// Bitmask:       000000 00000 00010 
const TUint32 KAccStereoAudio = 0x0002;

// Name:          Mono audio output
// Value:         N/A
// Dynamic Type:  Static
// I/O:           N/A
// Description:   To indicate if accessory has mono audio output.
// Bitmask:       000000 00000 00011
const TUint32 KAccMonoAudio = 0x0003;

// Name:          Audio in Connector 
// Value:         TBool.
// Dynamic Type:  Dynamic
// I/O:           From accessory to terminal, input.
// Description:   If capability exists it indicates that accessory has connector
//                enabling audio input from external audio device. If the value
//                is ETrue it indicates that external audio device is connected.
// Bitmask:       000000 00000 00100
const TUint32 KAccAudioInConnector = 0x0004;

// Name:          External Audio in Activation
// Value:         TBool
// Dynamic Type:  Dynamic
// I/O:           From terminal to accessory, output.
// Description:   If capability exists it indicates that accessory supports
//                activating(ETrue)/deactivating(EFalse) the external audio input.
//                External audio in activation capability requiers that accessory
//                has audio in connector capability.
// Bitmask:       000000 00000 00101
const TUint32 KAccExternalAudioInActivation = 0x0005;

// Name:          External Audio out Connector
// Value:         TBool
// I/O:           From accessory to terminal, input
// Dynamic Type:  Dynamic
// Description:   Audio out connector capability indicates that accessory has a
//                connector enabling audio output to external audio device. Value
//                indicates if the external audio device is removed(EFalse) or
//                connected(ETrue).
// Bitmask:       000000 00000 00110
const TUint32 KAccAudioOutConnector = 0x0006;

// Name:          Integrated Audio Output
// Value:         N/A
// I/O:           N/A
// Dynamic Type:  N/A
// Description:   Indicates that accessory has integrated audio output. 
//                E.g. integrated headset.
// Bitmask:       000000 00000 00111
const TUint32 KAccIntegratedAudioOutput = 0x0007;

// Name:          Integrated Audio Input
// Value:         N/A
// I/O:           N/A
// Dynamic Type:  N/A
// Description:   Indicates that accessory has integrated audio input. 
//                E.g. To check that accessory has integrated microphone.
// Bitmask:       000000 00000 01000
const TUint32 KAccIntegratedAudioInput = 0x0008;

// 0x0009 reserved for future use

// Name:          No DRM output
// Value:         N/A
// Dynamic Type:  Static
// I/O:           From accessory to terminal, input.
// Description:   indicates that accessory is NOT allowed to handle DRM protected audio output.
// Bitmask:       000000 00000 01010
const TUint32 KAccNoDRMOutput = 0x000A;

// Name:          Audio Output Type
// Value:         TAccAudioOutPutType
// Dynamic Type:  Static
// I/O:           N/A
// Description:   Audio Output Type definition corresponds to audio output type
//                definition in MMF. It is used to decide how audio is routed.
// Bitmask:       000000 00000 01011
const TUint32 KAccAudioOutputType = 0x000B;

enum TAccAudioOutPutType
    {
    EAccAudioOutPutTypePrivate = 0,
    EAccAudioOutPutTypePublic
    };

// Name:          Set Volume Level
// Value:         KAccSetVolumeLevel bitmask constants are declared in AccPolCommonNameValuePairs.h.
// Dynamic Type:  Static
// I/O:           From accessory to terminal, input.
// Description:   Indicates that accessory supports an interface for setting
//                the volume level from terminal to accessory.
//                If a bit is on then volume can be adjusted from the terminal.
//                If a bit is off then volume can not be adjusted from the terminal.
//
//                This capability can also be used to enable/disable proprietary specific
//                volume control. In such case this capability can only be used to transfer
//                proprietary information.
//
//                Following example shows how to set AVRCP remote volume control feature bit on.
//                Existing values must be read first to be able to set only AVRCP specific value.
//
//                @code
//                RAccessoryControl accessoryControl;
//                accessoryControl.CreateSubSession( iAccessoryServer );
//                                
//                CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();	
//    	          CleanupStack::PushL( nameArray );	  
//                accessoryControl.GetSubblockNameArrayL( iGenericID, *nameArray );
//                
//                TAccPolNameRecord nameRecord;
//    		      nameRecord.SetNameL( KAccSetVolumeLevel );		       		   
//    			  TAccValueTypeTInt value;
//    		   	            
//                if( nameArray->HasName( KAccSetVolumeLevel ) )
//                    {
//                    accessoryControl.GetValueL( iGenericID, nameRecord, value );                                               
//                    value.iValue = value.iValue | KAccSetVolumeLevelAVRCP;                
//                    accessoryControl.AccessoryValueChangedNotifyL( iGenericID, nameRecord, value );
//                    }
//                                    
//                CleanupStack::PopAndDestroy( nameArray );
//                accessoryControl.CloseSubSession();    
//                @endcode
// Bitmask: 000000 00000 01100
const TUint32 KAccSetVolumeLevel = 0x000C;
const TInt    KAccSetVolumeLevelHSP   = 0x1; //HSP remote volume control feature is supported.
const TInt    KAccSetVolumeLevelHFP   = 0x2; //HFP remote volume control feature is supported.
const TInt    KAccSetVolumeLevelAVRCP = 0x4; //AVRCP remote volume control feature is supported.

// Name:          Mute All Audio
// Value:         TBool
// Dynamic Type:  Dynamic
// I/O:           From accessory to terminal, input.
// Description:   Mute capability indicates that accessory supports muting all audio
//                outputs (e.g. any speakers). The value indicates that any
//                accessory audio outputs need to be muted (ETrue)/unmuted (EFalse).
// Bitmask:       000000 00000 01101
const TUint32 KAccMuteAllAudio = 0x000D;

// Name:          Audio Out
// Value:         TAccAudioOut
// Dynamic Type:  Static
// I/O:           N/A
// Description:   The audio out capability indicates that the accessory supports 
//                audio out signal.
// Bitmask:       000000 00000 01110
const TUint32 KAccAudioOut = 0x000E;

enum TAccAudioOut
    {
    TAccAudioOutLineout = 0 //Line Out
    };

// Name:          Audio Latency
// Value:         TInt ( value in milliseconds )
// Dynamic Type:  Dynamic
// I/O:           From audio source to terminal, input.
// Description:   Latency capability indicates that the audio source 
//                provides latency information.
// Bitmask:       000000 00000 01111
const TUint32 KAccAudioLatency = 0x000F;

// Name:          Audio Streams Format
// Value Type:    Object, inherited CAccPolObjectBase.
// Dynamic Type:  Dynamic
// I/O Type:      From accessory to terminal / From terminal to accessory. 
// Description:   Accessory supports to Get/Set audio stream(s) format related parameters
//                for input and output streams ( e.g. mic and speaker ).
// Bitmask:       000000 00000 10000
const TUint32 KAccAudioStreamsFormat = 0x0010;

// Name:          Audio Units
// Value Type:    Object, inherited CAccPolObjectBase.
// Dynamic Type:  static
// I/O Type:      From accessory to terminal. 
// Description:   Indicates an audio units which are supported by the accessory. 
// Bitmask:       000000 00000 10001
const TUint32 KAccAudioUnits = 0x0011;

// Name:          Get/Set Audio Control
// Value Type:    Object, inherited CAccPolObjectBase.
//                
// Dynamic Type:  static
// I/O Type:      From terminal to accessory.
// Description:   Gets value(s) of specific Audio Control object(s).
//                Sets value(s) for specific Audio Control object(s).
// Bitmask:       000000 00000 10010
const TUint32 KAccAudioControl = 0x0012;

// REQ-417-64804: In-Car optimized Device
// Name:          RTP Streaming
// Value:         N/A
// Dynamic Type:  Static
// I/O:           From accessory to terminal, input.
// Description:   indicates that accessory supports RPT streaming channel for audio data.
// Bitmask:       000000 00000 10011
const TUint32 KAccRTPStreaming = 0x0013;
// REQ-417-64804: In-Car optimized Device

// Name:          FM Antenna Support
// Value:         N/A
// Dynamic Type:  Static
// I/O:           From accessory to terminal, input.
// Description:   indicates that accessory has FM antenna support for audio data.
// Bitmask:       000000 00000 10100
const TUint32 KAccFMAntennaSupport = 0x0014;


// CAR KIT SB NAME-VALUE PAIRS:


// Name:          Audio To Privacy Handset
// Value:         TBool
// Dynamic Type:  Dynamic
// I/O:           From terminal to accessory, output.
// Description:   Audio To Privacy Handset capability indicates that car kit supports
//                an interface for routing the audio from default audio output to
//                privacy handset. The name is also used to route audio back to the
//                default audio output. Control handset audio in Car Handfree
//                peripherals. Audio is routed to handset when call is active and 
//                handset is removed from it's holder. audio needs to be 
//                activated (ETrue) / deactivated (EFalse).
// Bitmask:       000001 00000 00000 
const TUint32 KAccAudioToHandset = 0x0400;

// Name:          Privacy Handset
// Value:         TBool
// Dynamic Type:  Dynamic
// I/O:           From accessory to terminal, input.
// Description:   Indicate if handset is in the holder(ETrue) or not(EFalse).
// Bitmask:       000001 00000 00001
const TUint32 KAccPrivacyHandset = 0x0401;

// Name:          Car Radio Mute
// Value:         TBool
// I/O:           From terminal to accessory, output.
// Description:   Car Radio Mute, indicates that accessory supports an interface for
//                muting the peripheral car radio. The value (dynamic) indicates that
//                Car Radio needs to be muted(ETrue)/ unmuted(EFalse).
// Bitmask:       1 00000 00010
const TUint32 KAccCarRadioMute = 0x0402;


// CHARGING SB NAME-VALUE PAIRS:


// Name:          External Power Supply
// Value:         TBool
// I/O:           From accessory to terminal, input.
// Description:   When connected, the power supply of the enhancement is active and 
//                enhancement can work properly. When disconnected, the enhancement 
//                doesn’t have power supply and the enhancement is detected, 
//                but not indicated to the user and not activated, 
//                e.g. required audios are not routed to enhancement
//                The value (dynamic) indicates that external power supply is
//                connected (ETrue) / disconnected (EFalse).
// Bitmask:       000010 00000 00000
const TUint32 KAccExtPowerSupply = 0x0800;


// BLUETOOTH SB NAME-VALUE PAIRS:


// Name:          Headset Profile (HS), Bluetooth device supports HFP.
// Value:         N/A
// Bitmask:       000011 00000 00000
const TUint32 KAccBTHSP = 0x0C00;

// Name:          Object Push Profile (OPP), the device supports Object Push Profile.
// Value:         N/A
// Bitmask:       000011 00000 00001
const TUint32 KAccBTOPP = 0x0C01;

// Name:          Generic Object Exchange Profile (GOEP), The device supports Generic Object
//                Exchange Profile.
// Value:         N/A
// Bitmask:       000011 00000 00010
const TUint32 KAccBTGOEP = 0x0C02;

// Name:          File Transfer Profile (FTP), the name indicates that device supports File
//                Transfer Profile.
// Value:         N/A
// Bitmask:       000011 00000 00011
const TUint32 KAccBTFTP = 0x0C03;

// Name:          Generic Access Profile (GAP), the name indicates that device supports 
//                Generic Access Profile.
// Value:         N/A 
// Bitmask:       000011 00000 00100
const TUint32 KAccBTGAP = 0x0C04;

// Name:          Serial Port Profile (SPP), the name indicates that device supports Serial
//                Port Profile.
// Value:         N/A 
// Bitmask:       000011 00000 00101
const TUint32 KAccBTSPP = 0x0C05;

// Name:          Handsfree Profile (HFP), Bluetooth device supports HFP.
// Value:         N/A
// Bitmask:       00011 00000 00110
const TUint32 KAccBTHFP = 0x0C06;

// Name:          Audio Video Remote Control Profile (AVRCP), the device supports AVRCP.
// Value:         N/A
// Bitmask:       000011 00000 00111
const TUint32 KAccBTAVRCP = 0x0C07;

// Name:          Generic Audio Video Distribution Profile (GAVDP), the device supports AVRCP.
// Value:         N/A 
// Bitmask:       000011 00000 01000
const TUint32 KAccBTGAVDP = 0x0C08;

// Name:          Advanced Audio Distribution Profile (A2DP), the name indicates that device 
//                supports Advanced Audio Distribution Profile.
// Value:         N/A
// Bitmask:       000011 00000 01001
const TUint32 KAccBTA2DP = 0x0C09;

// Name:          Human Interface Device Profile (HID), the device supports Human Interface 
//                Device Profile.
// Value:         N/A
// Bitmask:       000011 00000 01010
const TUint32 KAccHID = 0x0C0A;

// Name:          Personal Area Networking Profile (PAN), the device supports Personal Area
//                Networking Profile..
// Value:         N/A
// Bitmask:       000011 00000 01011
const TUint32 KAccBTPAN = 0x0C0B;

// Name:          SIM Access Profile (SAP), the device supports SIM Access Profile.
// Value:         N/A
// Bitmask:       000011 00000 01100
const TUint32 KAccBTSAP = 0x0C0C;

// Name:          Basic Imaging Profile (BIP), the device supports this Profile.
// Value:         N/A
// Bitmask:       000011 00000 01101
const TUint32 KAccBTBIP = 0x0C0D;

// Name:          Basic Printing Profile (BPP), the device supports this Profile.
// Value:         N/A
// Bitmask:       000011 00000 01110
const TUint32 KAccBTBPP = 0x0C0E;

// Name:          Voice Recognition
// Value:         N/A
// Dynamic Type:  Static
// I/O:           N/A
// Description:   Indicates that the bluetooth HFP profile supports the voice recognition.
// Bitmask:       000011 00000 01111
const TUint32 KAccBTVoiceRecognition = 0x0C0F;

// VIDEO OUT SB NAME-VALUE PAIRS:

// Name:          Video out
// Value:         TAccVideoOut
// Dynamic Type:  Static
// I/O:           N/A.
// Description:   The Video out capability indicates that the accessory supports video 
//                out signal
// Bitmask:       000100 00000 00000
const TUint32 KAccVideoOut = 0x1000;

enum TAccVideoOut
    {
    EAccVideoOutCVideo = 0, //C-Video
    EAccVideoOutEHDMI       //HDMI
    };

// HDMI SB NAME-VALUE PAIRS:

// Name:          Get HDMI attributes.
// Value Type:    Object, inherited CAccPolObjectBase.
// Dynamic Type:  dynamic
// I/O Type:      From accessory to terminal.
// Description:   Get audio and video attributes of the currently connected
//                HDMI device.
// Bitmask:       000100 00000 00001
const TUint32 KAccVideoHdmiAttributes = 0x1001;

// Name:          Set HDCP support.
// Value Type:    TBool
// Dynamic Type:  dynamic
// I/O Type:      N/A
// Description:   Set HDCP support
// Bitmask:       000100 00000 00010
const TUint32 KAccVideoHdcpSupport = 0x1002;

// Name:          Get HDCP support status.
// Value Type:    TBool
// Dynamic Type:  dynamic
// I/O Type:      N/A
// Description:   Get HDCP support status. When the status changes
//                a notification is received.
// Bitmask:       000100 00000 00011
const TUint32 KAccVideoHdcpSupportStatus = 0x1003;

// Name:          Get active HDMI video format.
// Value Type:    Object, inherited CAccPolObjectBase.
// Dynamic Type:  static
// I/O Type:      N/A
// Description:   Get active HDMI video format information.
// Bitmask:       000100 00000 00100
const TUint32 KAccVideoFormat = 0x1004;

#endif      // ACCPOLCOMMONNAMEVALUEPAIRS_H

// End of File
