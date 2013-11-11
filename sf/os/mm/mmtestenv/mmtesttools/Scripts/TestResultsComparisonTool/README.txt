This script is for the purpose of comparing test runs run on Mistral or a developer's machine against a Known Failure sheet and/or comparison Mistral build to identify any new regressions introduced in the current run.

For usage call ResultsComparison.pl -h

Whenever updating the script please run the regression suite prior to submission.  The steps are as follows:

1.  Install Apache Web Server and host a server on port 8080.
2.  Map the contents of the RegressionSuite folder to the root of the web server's source location.
3.  Copy RegressionTestKF.xml to the C:\ drive.
4.  Call RunTesting.bat
5.  Compare the output files to the contents of the ExpectedResultLogs folder and ensure they match.

N.B.  If adding new tests that affect the Known Failure sheet remember to regenerate the XML version from within Excel:
File --> Save As --> XML Spreadsheet 2003 filetype

