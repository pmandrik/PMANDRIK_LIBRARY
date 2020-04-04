
from pmlib_decors import *

import os
import subprocess
import re

MAX_RETRIES = 3
SLEEP_TIME  = 30

CONDOR_TEMPLATE="""
executable            = %(EXE_NAME)s
arguments             = $(ClusterID) $(ProcId)
output                = %(JOB_NAME)s.$(ClusterId).$(ProcId).out
error                 = %(JOB_NAME)s.$(ClusterId).$(ProcId).err
log                   = %(JOB_NAME)s.$(ClusterId).log

requirements          = (OpSysAndVer =?= "CentOS7")

+JobFlavour           = \"%(JOB_FLAVOUR)s\"

queue
"""

### class to submite and control condor jobs 
class CondorCluster():
  def __init__(self, options = {}):
    try:
      import htcondor
      self.schedd = htcondor.Schedd()
      self.action = htcondor.JobAction
    except Exception, error:
      raise ClusterManagmentError, 'could not import htcondor python API: \n%s' % error

    ### http://pages.cs.wisc.edu/~adesmet/status.html
    self.status_map = {}
    self.status_map[0] =	"Unexpanded"
    self.status_map[1] =	"Idle"
    self.status_map[2] =	"Running"
    self.status_map[3] =	"Removed"
    self.status_map[4] =	"Completed"
    self.status_map[5] =	"Held"
    self.status_map[6] =	"Submission_err"

    self.max_jobs_per_request = 50
    self.max_retries = MAX_RETRIES
    self.sleep_time  = SLEEP_TIME

    self.job_ids = []
    self.verbose = True

  def create_condor_cfg(self, options={}, template=CONDOR_TEMPLATE):
    # https://twiki.cern.ch/twiki/bin/view/ABPComputing/LxbatchHTCondor
    req_options = ["EXE_NAME", "JOB_NAME", "JOB_FLAVOUR", "CONDOR_CFG_NAME"]
    for value in req_options:
      if value in options.keys(): continue
      print value, "is requred to be in provided options, abort job creation"
      if value == "JOB_FLAVOUR":
        print """possible values are:
espresso 	20min
microcentury 	1h 	
longlunch 	2h 
workday 	8h 
tomorrow 	1d 
testmatch 	3d
nextweek 	1w """
      
      return
    condor_tmp = template % options
    f = open(  options["CONDOR_CFG_NAME"], "w")
    f.write( condor_tmp )
    f.close()

  @multiple_try(MAX_RETRIES, SLEEP_TIME)
  def submit_job(self, job_file):
    f = open(job_file)
    job_text = f.read()

    a = subprocess.Popen(['condor_submit'], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
    output, _ = a.communicate( job_text )
    if self.verbose : print output
    #output = a.stdout.read()
    #Submitting job(s).
    #Logging submit event(s).
    #1 job(s) submitted to cluster 2253622.
    pat = re.compile("submitted to cluster (\d*)",re.MULTILINE)
    try:
      id = pat.search(output).groups()[0]
    except:
      raise ClusterManagmentError, 'fail to submit to the cluster: \n%s'  % output 

    self.job_ids += [ id ]
    return id

  def get_status(self, status):
    if not self.status_map.has_key( status ) : return status
    return self.status_map[ status ]

  def query(self, ids, requested_attributes=[], lim=-1):
    # https://research.cs.wisc.edu/htcondor/manual/v7.8/4_1HTCondor_s_ClassAd.html
    # Boolean stringListMember(String x, String list [ , String delimiter ])
    # Returns TRUE if item x is in the string list, as delimited by the optional delimiter string. Returns FALSE if item x is not in the string list. Comparison is done with strcmp(). The return value is ERROR, if any of the arguments are not strings. 
    # https://research.cs.wisc.edu/htcondor/manual/v8.6/6_7Python_Bindings.html
    q = self.schedd.query(constraint='stringListmember(string(ClusterId),"{0}")'.format(",".join(str(id) for id in ids)), attr_list=requested_attributes, limit=lim)
    return q

  @multiple_try(MAX_RETRIES, SLEEP_TIME)
  def check_job(self, id):
    q = self.query([str(id)], ["JobStatus", "HoldReason"], lim=1)
    try:
      status_raw = q[0]["JobStatus"]
    except Exception, error:
      raise ClusterManagmentError, 'could not retrieve job query:\n%s' % error

    status = self.status_map(status_raw)

    if status == "Held":
      hold_reason = q[0]["HoldReason"]
      print "CondorCluster.check_job(): ClusterId %s held with HoldReason: %s" % (str(id), hold_reason)

    return status

  # @multiple_try(MAX_RETRIES, SLEEP_TIME)
  def check_jobs(self, job_ids):
    status_counter = dict.fromkeys( self.status_map.values(), 0 )
    ongoing_jobs = []

    for i in range( 1 + (len(job_ids) - 1) / self.max_jobs_per_request ):
      start = i * self.max_jobs_per_request
      stop = min( (i+1) * self.max_jobs_per_request, len(job_ids))
      results = self.query(job_ids[start:stop], ["ClusterId", "JobStatus", "HoldReason"])

      for job_result in results:
        id, status = job_result["ClusterId"], self.get_status( job_result["JobStatus"] )

        ongoing_jobs.append( id )
        status_counter[status] += 1

        if status == "Held":
          hold_reason = q[0]["HoldReason"]
          print "CondorCluster.check_jobs(): ClusterId %s held with HoldReason: %s" % (str(id), hold_reason)

      idle     = status_counter["Idle"] + status_counter["Unexpanded"]
      running  = status_counter["Running"]
      fail     = status_counter["Held"]
      finished = status_counter["Completed"]

    ids = [ int(id) for id in job_ids ]
    ids = set(ids).difference(ongoing_jobs)
    finished += len( ids ) #TODO add status checking
      
    return idle, running, fail, finished

  def wait_jobs(self):
    ### Wait that all job are finished
    idle_prev, running_prev = 0, 0
    iters_since_last_update = 0
    time_to_sleep = self.sleep_time
    while True : 
      idle, running, fail, finished = self.check_jobs( self.job_ids )

      time_to_sleep = self.sleep_time
      if idle == idle_prev and running == running_prev:
        iters_since_last_update += 1
        time_to_sleep = min( self.sleep_time * iters_since_last_update, self.sleep_time * 10 )
      else : 
        iters_since_last_update = 0

      print "CondorCluster.wait_jobs(): %s idle, %s running, %s failed, %s finished jobs ... wait %s sec " % (idle, running, fail, finished, time_to_sleep)
      if idle + running == 0: break
      idle_prev, running_prev = idle, running
      try:
        time.sleep( time_to_sleep )
      except KeyboardInterrupt:
        pass

def pmlib_condor_tests():
  def create_test_exe( options ):
    text = """#!/bin/bash

echo "hello, it is test script for condor!"

sdir=%(SCRIPT_DIRECTORY)s
wdir=`pwd`

cp $sdir/test_exe_input.txt  $wdir/test_exe_input.txt
cat test_exe_input.txt > test_exe_output.txt
cp $wdir/test_exe_output.txt $sdir/test_exe_output.txt

ls -lathr

echo "done!"

exit 0
"""

    f = open( options["EXE_NAME"], "w" )
    f.write( text % options )
    f.close()

    f = open( "test_exe_input.txt", "w" )
    f.write( "kvarki kvarki kvarki" )
    f.close()

  cond = CondorCluster()
  options = { "EXE_NAME" : "test_exe.sh", "JOB_NAME" : "test_condor_job", "JOB_FLAVOUR" : "espresso", "CONDOR_CFG_NAME" : "test_condor_cfg.txt" }
  options[ "SCRIPT_DIRECTORY" ] = os.getcwd()
  create_test_exe( options )

  cond.create_condor_cfg( options )
  cond.submit_job( options["CONDOR_CFG_NAME"] )

  cond.wait_jobs()

if __name__ == "__main__": pmlib_condor_tests()









