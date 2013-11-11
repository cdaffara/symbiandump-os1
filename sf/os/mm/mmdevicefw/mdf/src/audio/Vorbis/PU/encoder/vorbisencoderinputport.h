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

#ifndef VORBISENCODERINPUTPORT_H
#define VORBISENCODERINPUTPORT_H

#include <mdf/mdfinputport.h>
#include <omxinputport.h>

class MMdfOutputPort;
class MMdfOutputPortObserver;
class CVorbisEncoderProcessingUnit;
class CVorbisEncoderOutputPort;
class CMMFBuffer;

class CVorbisEncoderInputPort : public COmxInputPort
	{
public:
	static CVorbisEncoderInputPort* NewL(TInt aIndex, CVorbisEncoderProcessingUnit* aParent);

	TInt MipConfigure(const TPuConfig&  aConfiguration);
	virtual void MipInitialize();

private:
	CVorbisEncoderInputPort();
private:
	
	// PCM settings
	TInt 					iSampleRate;
	TInt 					iChannels;
	TBool					iInterleaved;
	};

#endif // VORBISENCODERINPUTPORT_H
