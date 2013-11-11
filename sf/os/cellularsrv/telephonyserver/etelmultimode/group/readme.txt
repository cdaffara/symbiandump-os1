Please note that the current test for this component is TS_MMCli. This is essentially a version of Tmmcli that is ported to use the ScheduleTest framework. Since the harness uses Scheduletest, the user should compile the latest version of ScheduleTest before compiling this test.

ScheduleTest can be found in networking/integrationtest and is built from the networking/integrationtest/group directory. TS_MMCli can be found  in telephony/etelmm/tetel/ts_mmcli. The test is complied from the telephony/etelmm/group directory. Please note that in order to run this test you also need to compile the dummy tsy.

For ease of merging test code defect fixes from Hurricane to 3GComms, the old test code is still present in the 3GComms_Hurricane branch but is not built.
