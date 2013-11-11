@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

testexecute.exe z:\ssmatest\ssmatest_ssm_platsec.script
testexecute.exe z:\ssmatest\ssmatest_ssm_paniccle.script
testexecute.exe z:\ssmatest\ssmatest_ssm_server.script
testexecute.exe z:\ssmatest\ssmatest_ssm_ssmcledeadlock.script
testexecute.exe z:\ssmatest\ssmatest_ssm_repeatedpublishstate.script
testexecute.exe z:\ssmatest\ssmatest_ssm_conditionalcommands.script
testexecute.exe z:\ssmatest\ssmatest_ssm_statefurthertransition.script
testexecute.exe z:\ssmatest\ssmatest_ssm_statepolicyresolver.script
testexecute.exe z:\ssmatest\ssmatest_ssm_statetransitionengine.script
testexecute.exe z:\ssmatest\ssmatest_ssm_swpcommonplugin.script
testexecute.exe z:\ssmatest\ssmatest_ssm_swpdependencies.script
testexecute.exe z:\ssmatest\ssmatest_ssm_swphandlecleret.script
testexecute.exe z:\ssmatest\ssmatest_ssm_swppolicymap.script
testexecute.exe z:\ssmatest\ssmatest_ssm_swppolicyresolver.script
testexecute.exe z:\ssmatest\ssmatest_ssm_swptransitionengine.script
testexecute.exe z:\ssmatest\ssmatest_ssm_swptransitionscheduler.script
testexecute.exe z:\ssmatest\ssmatest_ssm_swprequesthandler.script
testexecute.exe z:\ssmatest\ssmatest_ssm_validstatelist.script
testexecute.exe z:\ssmatest\ssmatest_ssm_validswplist.script
testexecute.exe z:\ssmatest\ssmatest_ssm_rswppolicysess.script
testexecute.exe z:\ssmatest\ssmatest_ssm_cmdexecution_oom.script

:: Ignore the following if using this script for emulator testing.
:: Logs are automatically copied on hardware to MMC
:: which is assumed to be drive E:
@echo off
if not exist c:\logs\testexecute\ goto :EOF
md e:\logs
md e:\logs\testexecute
copy c:\logs\testexecute\ssmatest_ssm*.htm e:\logs\testexecute\