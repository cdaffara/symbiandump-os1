// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SS_NODEINTERFACES_H
#define SS_NODEINTERFACES_H

#include <elements/nm_interfaces.h>

namespace ESock
{

class TCFClientType : public Messages::TClientType
	{
public:
    enum TType
        {
        //-=============================================
        //
        // 1. Types 32bits (a client can only have one type)
        //
        // TClientType = BIT0..BIT7
        //-=============================================

        EData =              0x00000100,	// data client (node to left, 0..*)
        ECtrl =              0x00000200,	// control client (node above, 0..*)
        EServProvider =      0x00000400,	// service provider (node below, 0..*)
        ECtrlProvider =      0x00000800,	// control provider (node to right, 0..1)
        EAux =               0x00001000, // auxiliary client. The framework will consider the presence of
                                // these when determining the holder's state: auxialiary clients
                                // render the holder not idle. It is ok for auxiliary clients
                                // to be TNodeId::NullCommsId().
        EFactoryContainer =  0x00002000,    // a factory container as a client of central object broker
		};

	enum TFlags
		{
        //-=============================================
        //
        // 2. Flags 32bits (a client can have many flags)
        // (two sub-sections)
        //
        // TClientType = BIT0..BIT7
        //-=============================================

        //-------------------
        // 2.1. Type extensions (subtypes)
        //-------------------
		/**
		   @publishedPartner
		   @released since 9.5
		*/
        EMonitor                 = 0x80000000,
        EDefault                 = 0x40000000,//for default sub-connection
		EAttach                  = 0x20000000,// used by NetUPS to check if a JoinRequest comes from an RConnection::Attach

		/**
		   This flag is internal and not to be used by partner.
		   Indicates that a client is a configuration access point
		   i.e. it should be started after other access points on the layer
		   and should be stopped before other access points on the layer
		   
		   @internalComponent
		*/
		EConfigAccessPoint       = 0x10000000,

        //-------------------
        // 2.2. State Information
        //-------------------
		/**
		   @publishedPartner
		   @released since 9.5
		*/
        EJoining                 = 0x00000100,//The client has been prepared to be added and awaiting TApply to be formally joined.
        EActivating              = 0x00000200,//Given away at least one handle to this client and awaiting response(s).
		EActive                  = 0x00000400,//Given away handle to this client and got a response, i.e.: assuming somebody bound to the client until
		                                      //the client reports idle. (cleared by TDataClientIdle).
		EStarting                = 0x00000800,//
		EStarted                 = 0x00001000,//The client has sent TStarted .
		EStopping                = 0x00002000,//Stop Data Cients (cleared by TStopped)
		/**
		   Internal, not to be used by partners
		   
		   @internalComponent
		*/
		EAvailabilityProvider    = 0x00004000,
        };

public:
	TCFClientType()
	:	Messages::TClientType(EUnknown)
		{
		}

	TCFClientType(TUint32 aClientType, TUint32 aClientFlags = 0)
	:	Messages::TClientType(aClientType, aClientFlags)
		{
		}

	TUint IsMonitor() const
	    {
	    return iClientFlags & EMonitor;
	    }
	};


//
//RNodeInterface
class RCFNodePriorityInterface : public Messages::RNodeInterface
	{
public:
	RCFNodePriorityInterface(TUint aPriority)
	:	iPriority(aPriority)
		{
		}

	void SetPriority(TUint aPriority)
		{
		iPriority = aPriority;
		}

	TUint Priority() const
		{
		return iPriority;
		}
protected:
	TUint iPriority;
	};

} //namespace ESock

#endif
// SS_NODEINTERFACES_H


