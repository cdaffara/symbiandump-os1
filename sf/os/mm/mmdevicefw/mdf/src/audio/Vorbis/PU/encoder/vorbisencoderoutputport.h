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

#ifndef VORBISENCODEROUTPUTPORT_H
#define VORBISENCODEROUTPUTPORT_H

#include <mdf/mdfoutputport.h>
#include <mdf/mdfinputport.h>
#include <omxoutputport.h>

class TPuConfig;
class CMMFBuffer;
class CVorbisEncoderProcessingUnit;
class MMDFInputPort;

class CVorbisEncoderOutputPort : public COmxOutputPort
	{
	friend class CVorbisEncoderProcessingUnit;
public:
	static CVorbisEncoderOutputPort* NewL(TInt aIndex, CVorbisEncoderProcessingUnit* aParent);

	TInt MopConfigure(const TPuConfig&  aConfiguration);
 	void MopInitialize();
		
	TInt BitRate();
	void SetBitRate(TInt aBitRate);
	
private:
	CVorbisEncoderOutputPort();
private:
	TBool 						iStopped;	
	// Vorbis settings	
	TInt 						iBitRate;
	};

#endif // VORBISENCODEROUTPUTPORT_H
