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


#ifndef PULOADER_H
#define PULOADER_H

#include <mdf/mdfpuloader.h>

class TFourCC;

/*
ECom plugin for the utility class used by the HwDevice to load Processing Units.
*/
class CPuLoader : public CMdfPuLoader
	{
public:
	static CPuLoader* NewL();
	
	CMdfProcessingUnit* LoadProcessingUnitL(const MMdfProcessingUnitObserver& aProcessingUnitObserver, 
		TUid aImplementationUid);   
		
	CMdfProcessingUnit* LoadProcessingUnitL( const MMdfProcessingUnitObserver& aProcessingUnitObserver, 
		TFourCC aSrcDataType, TFourCC aDestDataType);
	
	CMdfProcessingUnit* LoadProcessingUnitL( const MMdfProcessingUnitObserver& aProcessingUnitObserver, 
		const TDesC8& aSrcDataType, const TDesC8& aDestDataType, const TUid& aImplementationType);
		
	TInt TunnelSetup(MMdfOutputPort& aOutputPort, MMdfInputPort& aInputPort);
	
	void UnloadProcessingUnit(CMdfProcessingUnit*& aPu);
	virtual ~CPuLoader();

protected:

	CPuLoader();

	/**
	Standard ConstructL() method.
	*/
	void ConstructL();
	};

#endif // PULOADER_H
