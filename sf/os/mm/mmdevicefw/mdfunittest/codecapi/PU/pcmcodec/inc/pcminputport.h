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

#ifndef PCMINPUTPORT_H
#define PCMINPUTPORT_H


#include "pcmprocessingunit.h"
#include "omxinputport.h"

class MMdfOutputPort;
class MMdfOutputPortObserver;
class CPcmOutputPort;
class CMMFBuffer;


class CPcmInputPort : public COmxInputPort
	{
public:
	static CPcmInputPort* NewL(TInt aIndex, COmxProcessingUnit* aParent, TPcmDataType aDataType);

	TInt MipConfigure(const TPuConfig&  aConfiguration);
	virtual void MipInitialize();
	
private:
	void ConstructL(TInt aIndex, COmxProcessingUnit* aParent);
	CPcmInputPort(TPcmDataType aDataType);
private:

	// PCM configuration parameters
	TInt 					iSampleRate;
	TInt 					iChannels;
	TBool					iInterleaved;
	TPcmDataType			iDataType;
	};

#endif // PCMINPUTPORT_H
