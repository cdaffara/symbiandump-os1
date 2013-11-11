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

#ifndef PCMOUTPUTPORT_H
#define PCMOUTPUTPORT_H

#include "pcmprocessingunit.h"
#include "omxoutputport.h"

class TPuConfig;
class CMMFBuffer;
class CPcmProcessingUnit;
class MMDFInputPort;


class CPcmOutputPort : public COmxOutputPort
	{
public:
	static CPcmOutputPort* NewL(TInt aIndex, COmxProcessingUnit* aParent, TPcmDataType aDataType);
	TInt MopConfigure(const TPuConfig&  aConfiguration);
 	void MopInitialize();

protected:
	void ConstructL(TInt aIndex, COmxProcessingUnit* aParent);
	CPcmOutputPort(TPcmDataType aDataType);

private:
	// PCM configuration parameters
	TInt 						iSampleRate;
	TInt						iChannels;
	TBool						iInterleaved;	
	TBool 						iStopped;	
	TPcmDataType 				iDataType;
	};


#endif // PCMOUTPUTPORT_H
