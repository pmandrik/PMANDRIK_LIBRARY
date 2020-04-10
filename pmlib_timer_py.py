#############################################################################
##  Author      :     P.S. Mandrik, IHEP
##  Date        :     01/04/20
##  Last Update :     01/04/20
##  Version     :     1.0
#############################################################################

import random

class Timer():
  def __init__(self, max_time, n_beats_max=0):
    self.imax = max_time
    self.itime = 0
    self.ftime = 0.0
    self.beat = False
    self.done = False

    if n_beats_max:
      self.Tick = self.TickNTimes
      self.n_beats_max = n_beats_max
      self.n_beats     = 0

  def Rand(self):
    self.itime = random.randrange(0, self.imax)
    self.ftime = float(self.itime)/float(self.imax)

  def Reset(self):
    self.itime = 0
    self.ftime = 0.0
    self.done = False
    self.n_beats = 0

  def TickNTimes(self, increment=1):
    if self.n_beats >= self.n_beats_max :
      self.beat = False
      self.done = True
      return self.beat, self.done

    self.itime += int(increment)
    self.beat = (self.itime >= self.imax or self.itime < 0)

    while self.itime >= self.imax:
      self.itime -= self.imax
      self.n_beats += 1
    while self.itime < 0:
      self.itime += self.imax
      self.n_beats += 1

    self.ftime = float(self.itime)/float(self.imax)
    self.done = (self.n_beats >= self.n_beats_max)
    return self.beat, self.done

  def Tick(self, increment=1):
    self.itime += int(increment)
    self.beat = (self.itime >= self.imax or self.itime < 0)

    while self.itime >= self.imax:
      self.itime -= self.imax
    while self.itime < 0:
      self.itime += self.imax

    self.ftime = float(self.itime)/float(self.imax)
    return self.beat


