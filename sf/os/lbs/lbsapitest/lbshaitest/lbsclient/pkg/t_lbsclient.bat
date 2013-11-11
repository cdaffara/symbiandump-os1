@rem
@rem Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem

testexecute c:\lbs\lbsclient\lbs-lbsclient-api-stress-automated-illegaluse.script -tcx c:\lbs\lbsclient\t_lbsclient.tcs
rename c:\logs\testexecute\TestResults.htm c:\logs\testexecute\TestResults_lbs-lbsclient-api-stress-automated-illegaluse.htm

testexecute c:\lbs\lbsclient\lbs-lbsclient-api-validation-automated-gps.script -tcx c:\lbs\lbsclient\t_lbsclient.tcs
rename c:\logs\testexecute\TestResults.htm c:\logs\testexecute\TestResults_lbs-lbsclient-api-validation-automated-gps.htm

testexecute c:\lbs\lbsclient\lbs-lbsclient-api-validation-automated-gps-accuracy.script -tcx c:\lbs\lbsclient\t_lbsclient.tcs
rename c:\logs\testexecute\TestResults.htm c:\logs\testexecute\TestResults_lbs-lbsclient-api-validation-automated-gps-accuracy.htm

testexecute c:\lbs\lbsclient\lbs-lbsclient-api-validation-automated-agps.script -tcx c:\lbs\lbsclient\t_lbsclient.tcs
rename c:\logs\testexecute\TestResults.htm c:\logs\testexecute\TestResults_lbs-lbsclient-api-validation-automated-agps.htm

testexecute c:\lbs\lbsclient\lbs-lbsclient-api-validation-automated-agps-accuracy.script -tcx c:\lbs\lbsclient\t_lbsclient.tcs
rename c:\logs\testexecute\TestResults.htm c:\logs\testexecute\TestResults_lbs-lbsclient-api-validation-automated-agps-accuracy.htm
