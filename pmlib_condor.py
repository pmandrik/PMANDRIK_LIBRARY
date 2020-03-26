
from pmlib_decors import *

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
    self.max_retries = 3
    self.sleep_time  = 30

  def create_default_job(self):
    pass # TODO

  @multiple_try(self.max_retries, self.sleep_time)
  def submit_job(self, job_file):
    f = open(job_file)
    job_text = f.read()

    a = subprocess.Popen(['condor_submit'], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
    output, _ = a.communicate( job_text )
    #output = a.stdout.read()
    #Submitting job(s).
    #Logging submit event(s).
    #1 job(s) submitted to cluster 2253622.
    pat = re.compile("submitted to cluster (\d*)",re.MULTILINE)
    try:
      id = pat.search(output).groups()[0]
    except:
    raise ClusterManagmentError, 'fail to submit to the cluster: \n%s' \
                                                                        % output 
    return id

  def get_status(self, status):
    if not self.status_map.has_key[ status ] : return status
    return self.status_map[ status ]

  def query(self, ids, requested_attributes=[], lim=-1):
    # https://research.cs.wisc.edu/htcondor/manual/v7.8/4_1HTCondor_s_ClassAd.html
    # Boolean stringListMember(String x, String list [ , String delimiter ])
    # Returns TRUE if item x is in the string list, as delimited by the optional delimiter string. Returns FALSE if item x is not in the string list. Comparison is done with strcmp(). The return value is ERROR, if any of the arguments are not strings. 
    # https://research.cs.wisc.edu/htcondor/manual/v8.6/6_7Python_Bindings.html
    q = self.schedd.query(constraint='stringListmember(string(ClusterId),"{0}")'.format(",".join(str(id) for id in ids)), attr_list=requested_attributes, limit=lim)
    return q

  @multiple_try(self.max_retries, self.sleep_time)
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

  @multiple_try(self.max_retries, self.sleep_time)
  def check_jobs(self, job_ids):
    status_counter = fromkeys( self.status_map.values(), 100 )
    ongoing_jobs = []

    for i in range( 1 + (len(job_ids) - 1) / self.max_jobs_per_request ):
      start = i * self.max_jobs_per_request
      stop = min( (i+1) * packet, len(job_ids))
      results = self.query(job_ids[start:stop], ["ClusterId", "JobStatus", "HoldReason"])

      for job_result in results:
        id, status = job["ClusterId"], self.get_status( job["JobStatus"] )

        ongoing_jobs.append( id )
        status_counter[status] += 1

        if status == "Held":
          hold_reason = q[0]["HoldReason"]
          print "CondorCluster.check_jobs(): ClusterId %s held with HoldReason: %s" % (str(id), hold_reason)

        for id in list(self.submitted_ids):
            if int(id) not in ongoing:
                status = self.check_termination(id)
                if status == 'wait':
                    run += 1
                elif status == 'resubmit':
                    idle += 1

      idle     = status_counter["Idle"] + status_counter["Unexpanded"]
      running  = status_counter["Running"]
      fail     = status_counter["Held"]
      finished = status_counter["Completed"]

      print "CondorCluster.check_jobs(): %s idle, %s running, %s failed, %s finished jobs ..." % (idle, running, fail, finished)
      return idle, running, fail, finished

  def wait_jobs(self):
    ### Wait that all job are finish.
    idle_prev, running_prev = 0, 0
    iters_since_last_update = 0
    while True : 
      idle, running, fail, finished = check_jobs( job_ids )
      if idle + run == 0:
        break

      time_to_sleep = self.sleep_time
      if idle == idle_prev and running == running_prev:
        iters_since_last_update += 1
        time_to_sleep = min( self.sleep_time * iters_since_last_update, self.sleep_time * 10 )
      else : 
        iters_since_last_update = 0
        cummulative_time_to_check += time_to_sleep

      num_of_updates += 1
      try:
        time.sleep( time_to_sleep )
      except KeyboardInterrupt:
        pass
      

def pmlib_condor_tests():
  pass

if __name__ == "__main__": pmlib_condor_tests()









