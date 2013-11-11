// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef VORBISDECODERINPUTPORT_H
#define VORBISDECODERINPUTPORT_H

#include <mdf/mdfinputport.h>
#include <omxinputport.h>

class MMdfOutputPort;
class MMdfOutputPortObserver;
class CVorbisDecoderProcessingUnit;
class CVorbisDecoderOutputPort;
class CMMFBuffer;

const TInt KVorbisDecoderInputPortIndex = 0;

class CVorbisDecoderInputPort : public COmxInputPort
	{
public:
	static CVorbisDecoderInputPort* NewL(TInt aIndex, CVorbisDecoderProcessingUnit* aComponent);
	TInt MipConfigure(const TPuConfig&  aConfiguration);
	virtual void MipInitialize();
private:
	CVorbisDecoderInputPort();
	};

#endif // VORBISDECODERINPUTPORT_H
