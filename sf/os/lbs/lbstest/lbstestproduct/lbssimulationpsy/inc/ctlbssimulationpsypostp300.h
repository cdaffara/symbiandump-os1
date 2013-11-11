/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef CTLBSSIMULATIONPSYPOSTP300_H_
#define CTLBSSIMULATIONPSYPOSTP300_H_

//  INCLUDES
#include "ctlbsportedstepbase.h"

/*
 *  Test procedure 300
 */
class CT_LbsSimulationPsyPosTp300 : public CT_LbsPortedStepBase
	{
public:
	// Constructors and destructor

	/*
	 * Constructor.
	 */
	CT_LbsSimulationPsyPosTp300(CT_LbsServer& aParent, TInt aIndex);

	/*
	 * Destructor.
	 */
	~CT_LbsSimulationPsyPosTp300();

	// Functions from base classes

	/*
	 * From CT_LbsPortedStepBase
	 */
	void StartL();

protected:
	/*
	 * Cleans up after a test.
	 * Always called, even if the test leaves.
	 * By default does nothing.
	 */
	 virtual void CloseTest();

private:
	/*
	 * Gets the name of an individual test index
	 * @param aIndex Indicates which test index name that is asked for.
	 * @param aName The name of the test index.
	 */
	TPtrC GetIndexName(TInt aIndex) const;
	/*
	 * Contains calls to tests itself. 
	 * @param aIndex test number
	 */
	void RunTestsL(TInt aIndex);
	/*
	 * Set Position Update Time Out for iPositioner. 
	 * @param aInterval update timeout
	 */
	void SetUpdateTimeOutL(const TInt aInterval);
	/*
	 * Set New Source file for Positioner server. 
	 * @param aName file to open name.
	 */
	void OpenNewSourceFileL(const TDesC& aName);
	/*
	 * Allows Positioner to accept partial update. 
	 * @param ETrue for partial updates allowed.
	 */
	void SetPartialUpdateL(TBool aPartial);
	/*
	 * The method checks whether the position is a partial fix.
	 * @param aPosInfo position information.
	 * @return ETrue for partial position info. Otherwise EFalse.
	 */
	TBool IsPositionInfoPartial(const TPositionInfo& aPosInfo);
	/*
	 * The method checks whether the position is a partial fix.
	 * @param aFileName simulation movement sps file or specified NMEA file.
	 * @param aCancelLastRequest if ETrue, cancel last request
	 */
	void TestMultiplyClientAccessL(const TDesC& aFileName, const TBool aCancelLastRequest = EFalse);
	void TestMultiplyClientDataL();
	void TestSpsPartialUpdateOptionL();
	void TestLatitudeWrapAroundL();
	TBool TestLatitudeWrapAround_HelperL(const TPositionInfo& aPosInfo, const TInt aCaseSelector, const TBool aLastIteration);
	void TestInvalidSpsFileL();
	void CheckBadInputFileL(const TDesC& aFrom);
	void TestNmeaNoFixL();
private:	
	TInt iIndex;
	};

#endif /* CTLBSSIMULATIONPSYPOSTP300_H_ */
