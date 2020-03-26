
import time
import traceback

# Decorator for re-running a crashing function automatically.
def multiple_try(tryes=5, sleep_time=20):
  def multiple_try_(f):
    def multiple_try__(*args, **opt):
      for i in range(tryes):
        try:
          return f(*args, **opt)
        except KeyboardInterrupt:
          raise
        except Exception, error:
          print 'pmlib.multiple_try() : fail to do %s function with %s args (%s try of %s) ... ' % (str(f), ', '.join([str(a) for a in args]), i+1, nb_try))
          print 'with error: %s' % str(error)
          if i == tryes-1 : print 'traceback :', traceback.format_exc()
          else : time.sleep(sleep * (i+1))
      raise error.__class__, 'pmlib.multiple_try() : fail %i times \n %s ' % (i+1, error)
    return multiple_try__
  return multiple_try_
