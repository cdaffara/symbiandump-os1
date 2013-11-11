call ResultsComparison.pl -m Mistral -s http://localhost:8080/new/new_summary.html -r http://localhost:8080/ref/ref_summary.html -k c:\RegressionTestKF.xml -c TB92 -p Winscw -v > normal_log.txt

call ResultsComparison.pl -m Mistral -s http://localhost:8080/new/new_summary.html -r http://localhost:8080/ref/ref_summary.html -k c:\RegressionTestKF.xml -c TB92 -p Winscw -i -v > normal_ignorenotinref_log.txt

call ResultsComparison.pl -m Mistral -s http://localhost:8080/incorrect_url.html -r http://localhost:8080/ref/ref_summary.html -k c:\RegressionTestKF.xml -c TB92 -p Winscw -v > nosummary_log.txt

call ResultsComparison.pl -m Mistral -s http://localhost:8080/empty_summary/empty_summary.html -r http://localhost:8080/ref/ref_summary.html -k c:\RegressionTestKF.xml -c TB92 -p Winscw -v > emptysummary_log.txt

call ResultsComparison.pl -m Mistral -s http://localhost:8080/new/new_summary.html -r http://localhost:8080/ref/ref_summary.html -v > nokfsheet_log.txt

call ResultsComparison.pl -m Mistral -s http://localhost:8080/new/new_summary.html -r http://localhost:8080/ref/ref_summary.html -k c:\KFSheetWrongFormat.xls -c TB92 -p Winscw -v > wrongformatkfsheet_log.txt

call ResultsComparison.pl -m Mistral -s http://localhost:8080/new/new_summary.html -r http://localhost:8080/ref/ref_summary.html -k c:\missing_kf_sheet.xml -c TB92 -p Winscw -v > missingkfsheet_log.txt

call ResultsComparison.pl -m Mistral -s http://localhost:8080/new/new_summary.html -k c:\RegressionTestKF.xml -c TB92 -p Winscw -v > noreferencebuild_log.txt

call ResultsComparison.pl -m Local -s ./logs/ -r http://localhost:8080/ref/ref_summary.html -k c:\RegressionTestKF.xml -c TB92 -p Winscw -v > local_normal_log.txt