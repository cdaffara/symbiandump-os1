// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// const.h - icmp echo client constants
// This software has been implemented in the 6PACK
// project at the Mobile Networks Laboratory (MNW)
// http://www.research.nokia.com/projects/6pack/
//

#ifndef __CONST_H
#define __CONST_H

#define KHostNameLimit	80

#define ICMP_ECHO_HEADER_SIZE   8       //Size of a ICMP header echo request/reply
#define MAX_IP_PACKETLEN      65535		//Determined by the 16-bit field total length
#define MAX_IP_HDRLEN      60			//Determined by the 4-bit field header length
#define ICMP_HDRLEN      8				//Defined the HDR for a ECHO_REQUEST ICMP packet

// Maximum ICMP Packet Data size
#define MAX_ICMP_PACKETSIZE MAX_IP_PACKETLEN - MAX_IP_HDRLEN - ICMP_HDRLEN

#endif
