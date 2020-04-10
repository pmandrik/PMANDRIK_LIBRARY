#############################################################################
##  Author      :     P.S. Mandrik, IHEP
##  Date        :     07/04/20
##  Last Update :     07/04/20
##  Version     :     1.0
#############################################################################

from pmlib_timer_py import *
from copy import deepcopy

class State():
  def __init__(self, name):
    self.name = name
    self.tick       = None
    self.tick_end   = None
    self.tick_begin = None
    self.options    = None

    self.done   = False
    self.remove_on_time = True;
    self.timer = None

class FSM():
  def __init__(self):
    self.active_states = []
    self.states = {}
    self.states_to_delete = []
    self.states_to_add    = []

  def AddStateDefinition(self, name, state):
    self.states[name] = state

  def AddState(self, state_name, tick_time = 0, remove_on_time = False, call_begin = True):
    if not self.states.has_key( state_name ) :
      raise "errro" # FIXME

    state = deepcopy( self.states[ state_name ] )
    state.done   = False
    state.remove_on_time = remove_on_time
    if tick_time : state.timer = Timer(tick_time, 1)
    if not call_begin : state.tick_begin = None
    self.states_to_add += [ state ]

  def RemoveState(self, state_name, call_end = True):
    state = None
    for state_candidate in self.active_states:
      if state_candidate.name != state_name: continue
      state = state_candidate
      break
    if not state    : 
      raise "error" #FIXME
    if not call_end : state.tick_end = None
    self.states_to_delete += [ state ]

  def Tick(self):
    for state in self.states_to_delete:
      if state.tick_end : state.tick_end( self, state )
      self.active_states.remove( state )
    self.states_to_delete = []

    for state in self.active_states:
      state.tick( self, state )
      if state.timer :
        state.timer.Tick()
        if state.remove_on_time and state.timer.done : state.done = True
      if state.done : 
        self.states_to_delete += [ state ]

    for state in self.states_to_add:
      if state.tick_begin : state.tick_begin( self, state )
    self.active_states += self.states_to_add
    self.states_to_add = []

  def Print(self):
    print "Active states:"
    for state in self.active_states:
      print state.name



class FSM_test_class():
  def __init__(self):
    self.name = "qwertyqwertyqwertyqwertyqwertyqwertyqwertyqwerty"

    self.fsm = FSM()

    state_work = State("work")
    state_work.tick     = lambda fsm, state: self.tick_work(fsm, state)
    state_work.tick_end = lambda fsm, state: fsm.AddState( "relax" )

    state_relax = State("relax")
    state_relax.tick = lambda fsm, state: self.tick_relax(fsm, state)
    state_relax.tick_end = lambda fsm, state: fsm.AddState( "work" )

    self.fsm.AddStateDefinition("work", state_work)
    self.fsm.AddStateDefinition("relax", state_relax)

    self.fsm.AddState("work")

  def tick_work(self, fsm, state):
    for i, letter in enumerate(self.name):
      if letter.isupper() : continue
      self.name = self.name[:i] + letter.upper() + self.name[i+1:]
      print self.name
      return
    state.done = True

  def tick_relax(self, fsm, state):
    for i, letter in enumerate(self.name):
      if letter.islower() : continue
      self.name = self.name[:i] + letter.lower() + self.name[i+1:]
      print self.name
      return
    state.done = True

if __name__ == "__main__" :
  item = FSM_test_class()
  for i in xrange(100):
    item.fsm.Tick()
    # item.fsm.Print()







