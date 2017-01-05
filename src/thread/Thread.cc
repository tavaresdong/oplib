#include <thread/Thread.h>
#include <thread/Common.h>

#include <sys/prctl.h>
#include <stdio.h>
#include <cassert>

namespace oplib
{

namespace CurrentThread
{
  __thread int tl_tid;
  __thread const char* tl_tstatus = "UNKNOWN";
}

  void atfork()
  {
    // clear the cachedTid, and call tid() again
    CurrentThread::tl_tid = 0;
    CurrentThread::tl_tstatus = "main";
    CurrentThread::tid();

  }

  // Use a static variable's constructor to call pthread_atfork
  class ThreadInitializer
  {
  public:
    ThreadInitializer()
    {
      CurrentThread::tl_tstatus = "MAIN";
      pthread_atfork(nullptr, nullptr, &atfork);
    }
  };

  ThreadInitializer init;

  std::atomic_long Thread::_numCreated { 0 };

  struct ThreadInfo
  {
    std::string _name;
    Thread::ThreadFunc  _func;
    std::weak_ptr<pid_t> _weaktid;

    ThreadInfo(const std::string& name_,
               const Thread::ThreadFunc& func_,
               std::shared_ptr<pid_t> tid_)
    : _name(name_),
      _func(func_),
      _weaktid(tid_)
    {}

    void run ()
    {
      std::shared_ptr<pid_t> sptid = _weaktid.lock();
      if (sptid)
      {
        *sptid = CurrentThread::tid();
        sptid.reset();
      }

      CurrentThread::tl_tstatus = _name.empty() ? "OPTHREAD" : _name.c_str();

      // Set the thread name
      ::prctl(PR_SET_NAME, CurrentThread::tl_tstatus);

      try {
        _func();
        CurrentThread::tl_tstatus = "FINISHED";
      }
      catch (const std::exception& ex_)
      {
        CurrentThread::tl_tstatus = "CRASHED";
        fprintf(stderr, "exception caught in Thread %s\n", _name.c_str());
        fprintf(stderr, "reason: %s\n", ex_.what());
        abort();
      }
      catch (...)
      {
        CurrentThread::tl_tstatus = "CRASHED";
        fprintf(stderr, "unknown exception caught in Thread %s\n", _name.c_str());
        throw; // rethrow
      }
    }
  };

  void* threadLauncher(void *arg_)
  {
    ThreadInfo* info = static_cast<ThreadInfo*>(arg_);
    info->run();
    delete info;
    return nullptr;
  }


  void Thread::setDefaultName()
  {
    int num = ++_numCreated;
    if (_name.empty())
    {
      char buffer[sizeof(long)];
      snprintf(buffer, sizeof(buffer), "Thread%d", num);
      _name = buffer;
    }
  }

  Thread::Thread(const ThreadFunc& func_, const std::string& name_)
  : _pthreadId(0),
    _tid(new pid_t(0)),
    _func(func_),
    _name(name_)
  {
    setDefaultName();
  }

  Thread::Thread(ThreadFunc&& func_, const std::string& name_)
  :_pthreadId(0),
   _tid(new pid_t(0)),
   _func(std::move(func_)),
   _name(name_)
  {
    setDefaultName();
  }

  Thread::~Thread()
  {
    if (_started && !_joined)
    {
      //TODO make a log of: fprintf(stdout, "Detached");
      pthread_detach(_pthreadId);
    }
  }

  int Thread::join()
  {
    assert(_started && !_joined);
    _joined = true;
    return pthread_join(_pthreadId, nullptr);
  }

  void Thread::start()
  {
    assert(!_started);
    _started = true;
    ThreadInfo* info = new ThreadInfo(_name, _func, _tid);
    if (pthread_create(&_pthreadId, nullptr, threadLauncher, info) != 0)
    {
      _started = false;
      delete info;
      // TODO logging
    }
    // info will be cleaned up by threadLauncher
  }
}
