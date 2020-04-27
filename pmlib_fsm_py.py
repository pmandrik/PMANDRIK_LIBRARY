#############################################################################
##  Author      :     P.S. Mandrik, IHEP
##  Date        :     07/04/20
##  Last Update :     07/04/20
##  Version     :     1.0
#############################################################################

from pmlib_timer_py import *
from copy import deepcopy

class State():
  def __init__(self, name, tick_f, begin_f = None, end_f = None):
    self.name = name
    self.tick  = tick_f
    self.end   = end_f
    self.begin = begin_f
    self.options = None

    self.done   = False
    self.remove_on_time = True;
    self.timer = None
    self.remove_mark = False

class FSM():
  def __init__(self) : 
    self.active_states = []
    self.active_states_names = {}
    self.states = {}
    self.states_to_delete = []
    self.states_to_add    = []
    self.owner = None
    
  def GetNActiveStates(self, name) : 
    if name in self.active_states_names:
      return self.active_states_names[ name ]
    return -1

  def AddStateDefinition(self, state) : 
    self.states[ state.name ] = state
    self.active_states_names[ state.name ] = 0
    
  def NewState(self, name, tick_f, begin_f = None, end_f = None) : 
    state = State( name, tick_f, begin_f, end_f )
    self.AddStateDefinition( state )
    return state

  def AddState(self, state_name, tick_time = 0, remove_on_time = False, call_begin = True) : 
    if not state_name in self.states :
      raise "errro" # FIXME

    state = deepcopy( self.states[ state_name ] )
    state.done   = False
    state.remove_on_time = remove_on_time
    if tick_time : state.timer = Timer(tick_time, 1)
    if not call_begin : state.begin = None
    self.states_to_add += [ state ]

  def RemoveState(self, state_name, call_end = True) : 
    state = None
    for state_candidate in self.active_states:
      if state_candidate.remove_mark : continue
      if state_candidate.name != state_name: continue
      state = state_candidate
      break
    if not state    : return
  
    print "GOINT TO REMOVE STATE", state.name
    print self.active_states_names
    self.active_states_names[ state.name ] -= 1
    if self.active_states_names[ state.name ] < 0:
      self.active_states_names[ state.name ] = 0
      print "FSM.RemoveState(): going to remove", state_name, "multiple times, ignore" 
      return
    
    print self.active_states_names
    if not call_end : state.end = None
    state.remove_mark = True
    self.states_to_delete += [ state ]
    
  def SwitchState(self, state_name, new_state_name, call_end = True, tick_time = 0, remove_on_time = False, call_begin = True) : 
    self.RemoveState( state_name, call_end )
    self.AddState( new_state_name, tick_time, remove_on_time, call_begin )

  def Tick(self) : 
    for state in self.states_to_delete:
      if state.end : state.end( self.owner, self, state )
      #print self.active_states_names
      #print "remove state", state.name, self.owner.id
      # self.Print()
      self.active_states.remove( state )
    self.states_to_delete = []

    for state in self.active_states:
      ### check if state in turned off during this loop by another state
      if state.remove_mark :
        if state.end : state.end( self.owner, self, state )
        state.end = None
        continue # no more Tick() for this state
      ### Tick
      if state.tick : state.tick( self.owner, self, state )
      if state.timer :
        state.timer.Tick()
        if state.remove_on_time and state.timer.done : state.done = True
      if state.done : 
        self.active_states_names[ state.name ] -= 1
        state.remove_mark = True
        self.states_to_delete += [ state ]

    for state in self.states_to_add:
      if state.begin : state.begin( self.owner, self, state )
      self.active_states_names[ state.name ] += 1
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
    state_work.end = lambda fsm, state: fsm.AddState( "relax" )

    state_relax = State("relax")
    state_relax.tick = lambda fsm, state: self.tick_relax(fsm, state)
    state_relax.end = lambda fsm, state: fsm.AddState( "work" )

    self.fsm.AddStateDefinition(state_work)
    self.fsm.AddStateDefinition(state_relax)

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







