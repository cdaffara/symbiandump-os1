// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef MMFERRORS_H
#define MMFERRORS_H

#include <e32base.h>

/**
Base of the Multimedia Subsystem error code allocation.
*/
const TInt KErrMMBase = -12000;


// Error codes for the CVideoPlayerUtility API
/**
Not enough network bandwidth.
*/
const TInt KErrMMNotEnoughBandwidth = KErrMMBase;

/**
Error locating network socket services.
*/
const TInt KErrMMSocketServiceNotFound = KErrMMBase - 1;	

/**
An error occurred while reading data from the network.
*/
const TInt KErrMMNetworkRead = KErrMMBase - 2;

/**
An error occurred while writing data to the network.
*/
const TInt KErrMMNetworkWrite = KErrMMBase - 3;	

/**
Invalid socket error or An error occurred while creating a network socket.
*/
const TInt KErrMMServerSocket = KErrMMBase - 4;

/**
Streaming Server not supported.
*/
const TInt KErrMMServerNotSupported = KErrMMBase - 5;

/**
Server alert.
*/
const TInt KErrMMServerAlert = KErrMMBase - 6;

/**
Player cannot receive UDP data packets.
*/
const TInt KErrMMUDPReceive = KErrMMBase - 7;

/**
Network transport not recognized.
*/
const TInt KErrMMInvalidProtocol = KErrMMBase - 8;

/**
Invalid URL.
*/
const TInt KErrMMInvalidURL = KErrMMBase - 9;

/**
An error occured attempting to join or access multicast session.
*/
const TInt KErrMMMulticast = KErrMMBase - 10;

/**
Proxy status error, Proxy invalid response error or Invalid hostname for proxy.
*/
const TInt KErrMMProxyServer = KErrMMBase - 11;

/**
Client cannot support proxy server.
*/
const TInt KErrMMProxyServerNotSupported = KErrMMBase - 12;

/**
Unable to locate proxy server or Proxy connection could not be established.
*/
const TInt KErrMMProxyServerConnect = KErrMMBase - 13;

/**
Cannot open audio device, or lost control of audiodevice.
*/
const TInt KErrMMAudioDevice = KErrMMBase - 14;

/**
Problem blitting video to display.
*/
const TInt KErrMMVideoDevice = KErrMMBase - 15;

/**
Invalid decoder, decoder not properly initialized, or appropriate decoder could not be found.
*/
const TInt KErrMMDecoder = KErrMMBase - 16;

/**
Only able to play either the audio or the video portion of the media file. Could be due to a
codec that isn't supported, the video frame size being too large etc.
*/
const TInt KErrMMPartialPlayback = KErrMMBase - 17;

/**
Digital Rights have expired.
*/
const TInt KErrMMDRMNotAuthorized = KErrMMBase - 18;

//Error codes for Quality of Service

/**
Low Bandwidth.
*/
const TInt KErrMMQosLowBandwidth = KErrMMBase - 19;

/**
Traffic Class not supported.
*/
const TInt KErrMMQosUnsupportedTrafficClass = KErrMMBase - 20;

/**
Poor or unsufficient Traffic Class. 
*/
const TInt KErrMMQosPoorTrafficClass = KErrMMBase - 21;

/**
Parameters not supported.
*/
const TInt KErrMMQosUnsupportedParameters = KErrMMBase - 22;

/**
Poor or unsufficient Parameters.
*/
const TInt KErrMMQosPoorParameters = KErrMMBase - 23;

/**
Not supported.
*/
const TInt KErrMMQosNotSupported = KErrMMBase - 24;

#endif
