
"""
/////////////////////////////////////////////////////////////////////////////
  Author      :     P.S. Mandrik, IHEP
  Date        :     28/01/21
  Last Update :     28/01/21
  Version     :     0.1.0
/////////////////////////////////////////////////////////////////////////////

  Python module latex_master.py for:
    - create latex table

/////////////////////////////////////////////////////////////////////////////
  Changelog : 

    28/01/21  - 0.1.0
                day of creation, version
"""

def make_table(columns, lines, content, style_dic = {}):
  N_col = len( columns )
  N_lin = len( lines )

  if len(content) != N_col or len(content[0]) != N_lin:
    print "latex_master.make_table(): incorrect table data size ",  N_col,"x",N_lin, "vs", len(content),"x",len(content[0])

  latex_table  = "\\begin{tabular}{ c |"
  for n in xrange(N_col):
    if n == N_col-1:
      latex_table += " c "
    else:
      latex_table += " c "
  latex_table += " }\n & "

  
  for n in xrange(N_col):
    val = columns[n]
    if n == N_col-1 : 
      latex_table += str(val) + " \\\\  \hline \n"
    else :
      latex_table += str(val) + " & "

  for y in xrange(N_lin):
    latex_table += lines[y] + " & "
    for x in xrange(N_col):
      if x == N_col - 1:
        latex_table += str(content[x][y]) + " \\\\ \n"
      else : 
        latex_table += str(content[x][y]) + " & "
  latex_table  += "\\end{tabular}\n"
  print latex_table

def examples():
  print "####################################### example 0 #######################################"
  xsecs = """testrun_NLO_massive_0/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb   3.3009334290242942E-002 +-   1.8924694288650985E-004
testrun_NLO_massive_0/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb   3.3009334290242942E-002 +-   1.8924694288650985E-004
testrun_NLO_massive_10/pwg-st3-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.57547192360630151      +-   2.3913735866754162E-003
testrun_NLO_massive_10/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.57547192360630151      +-   2.3913735866754162E-003
testrun_NLO_massive_10/run-2-1.log:  total (btilde+remnants) cross section in pb  0.57547192360630151      +-   2.3913735866754162E-003
testrun_NLO_massive_10/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.57547192360630151      +-   2.3913735866754162E-003
testrun_NLO_massive_11/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.17507589724325662      +-   4.2596177893789404E-004
testrun_NLO_massive_11/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.17507589724325662      +-   4.2596177893789404E-004
testrun_NLO_massive_12/pwg-st3-0001-stat.dat:  total (btilde+remnants) cross section in pb   3.6298306904324127      +-   7.0111877424018781E-003
testrun_NLO_massive_12/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb   3.6298306904324127      +-   7.0111877424018781E-003
testrun_NLO_massive_12/run-2-1.log:  total (btilde+remnants) cross section in pb   3.6298306904324127      +-   7.0111877424018781E-003
testrun_NLO_massive_12/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb   3.6298306904324127      +-   7.0111877424018781E-003
testrun_NLO_massive_2/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb   1.4814089099150654E-002 +-   8.1509391946483985E-005
testrun_NLO_massive_2/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb   1.4814089099150654E-002 +-   8.1509391946483985E-005
testrun_NLO_massive_3/pwg-st3-0001-stat.dat:  total (btilde+remnants) cross section in pb   1.0507146815103843      +-   2.6481640879519844E-003
testrun_NLO_massive_3/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb   1.0507146815103843      +-   2.6481640879519844E-003
testrun_NLO_massive_3/run-2-1.log:  total (btilde+remnants) cross section in pb   1.0507146815103843      +-   2.6481640879519844E-003
testrun_NLO_massive_3/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb   1.0507146815103843      +-   2.6481640879519844E-003
testrun_NLO_massive_5/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb   5.9501737628617657E-002 +-   2.2978572188124926E-004
testrun_NLO_massive_5/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb   5.9501737628617657E-002 +-   2.2978572188124926E-004
testrun_NLO_massive_6/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb   2.4602174150872846E-002 +-   1.1112841176613848E-004
testrun_NLO_massive_6/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb   2.4602174150872846E-002 +-   1.1112841176613848E-004
testrun_NLO_massive_7/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.16980078693709746      +-   4.4066305160845996E-004
testrun_NLO_massive_8/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.15744127246969000      +-   5.2787426610408387E-004
testrun_NLO_massive_8/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.15744127246969000      +-   5.2787426610408387E-004
testrun_NLO_massive_9/pwg-st2-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.14732334595456517      +-   5.0656168654259956E-004
testrun_NLO_massive_9/pwg-0001-stat.dat:  total (btilde+remnants) cross section in pb  0.14732334595456517      +-   5.0656168654259956E-004"""
  lines = ["SM"] + [str(i) for i in xrange(1, 13)]
  cols = ["Powheg LO", "MadGraph LO", "arxiv:1806.05162", "Powheg NLO", "arxiv:1806.05162"]
  data = [[0 for y in lines] for x in cols]
  make_table(cols, lines, data)
  


if __name__ == "__main__": examples()



