// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TRACEEVENTFACTORY_H
#define TRACEEVENTFACTORY_H

enum TCustomTraceEvents
	{
	ECustomMMF1 = 192,
	ECustomMMF2 = 193,
	
	ENodeMessages = 194,
	EMeshMachine = 195,
    ESimpleLogging = 196,
	};

class CTraceEventBase;
class CUTraceFrame;
class MEventOutputHandler;
class CMultiPartFrameCollection;

class CTraceEventFactory
    {
    public:
        static CTraceEventBase* CreateTraceEvent(const CUTraceFrame& aFrame, MEventOutputHandler& aOutputEventHandler);
        static CTraceEventBase* CreateTraceEvent(const CMultiPartFrameCollection& aFrameCollection, MEventOutputHandler& aOutputEventHandler);
    };

#endif
// TRACEEVENTFACTORY_H

