#############################################################################
##  Author      :     P.S. Mandrik
##  Date        :     31/05/20
##  Last Update :     01/06/20
##  Version     :     1.0
#############################################################################
##  Watchdog to follow and log all events in directory 
## 

import sys
sys.path.append("/home/pmandrik/tmp/inotify-0.2.10")

import inotify
import inotify.adapters
import logging
import logging.handlers

class Watchdog:
  def __init__(self, folder, log_path, reqursive = False, mask = None, events_n_actions = {}, logger = None ):
    self.log_path = log_path
    self.fodler = folder
    self.events_n_actions = events_n_actions

    self.mask = mask if mask else inotify.constants.IN_ALL_EVENTS
    if reqursive:
      self.watcher = inotify.adapters.InotifyTree(self.fodler, mask)
    else : 
      self.watcher = inotify.adapters.Inotify()
      self.watcher.add_watch(self.fodler, mask)

    if logger : 
      self.logger = logger
    else :
      self.logs_interval = 24; # h between new log creation
      self.nlogs         = 10; # max number of logs files
      self.logger = logging.getLogger(__name__)
      self.logger.setLevel( logging.DEBUG )
      handler = logging.handlers.TimedRotatingFileHandler(log_path, when='h', interval=int(self.logs_interval), backupCount=int(self.nlogs))
      formatter = logging.Formatter(fmt='%(asctime)s %(levelname)-8s %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
      handler.setFormatter(formatter)
      self.logger.addHandler(handler)
      self.logger.info("create %s log file" % log_path)

  def Run(self):
    for event in self.watcher.event_gen(yield_nones=False):
      (_, type_names, path, filename) = event
      out = "PATH=[{}] FILENAME=[{}] EVENT_TYPES={}".format(path, filename, type_names)
      self.logger.info( out )
      print( out )
      
      type_names_ = " ".join( type_names )
      if type_names_ in self.events_n_actions:
        func = self.events_n_actions[ type_names_ ]
        func( self, event )

if __name__ == "__main__": 
  folder = "/home/pmandrik/tmp/wdog"
  log_path = "/home/pmandrik/tmp/log.txt"

  def folder_dog( wdog_father, event ):
    print("gav")

  from inotify.constants import *
  wdog = Watchdog( folder, log_path, True, IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO, { 'IN_CREATE IN_ISDIR': folder_dog }, None )
  while True:
    try:
      wdog.Run()
    except Exception as error_log:
      print( error_log )




