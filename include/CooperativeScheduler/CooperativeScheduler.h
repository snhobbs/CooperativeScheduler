/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 * */

#pragma once
#ifndef COOPERATIVESCHEDULER_COOPERATIVESCHEDULER_H_
#define COOPERATIVESCHEDULER_COOPERATIVESCHEDULER_H_

#include <assert.h>

#include <cstdint>
#include <limits>
/**
 * Simple reusable cooperative task scheduler. The Tick Count needs to be
 * incrimented by a periodic timer, i.e. systick. Tasks have an implied priority
 * level, the first finished task in the list is run if multiple are scheduled
 */
class CooperativeTask {
  const uint32_t interval_;
  uint32_t start_time_;
  int32_t (*fp_func_)(void) { nullptr }
  void (*const fp_callback_)(int32_t) { nullptr }

 public:
  void SetCallFunction(int32_t (*fp_func)(void)) { fp_func_ = fp_func; }
  uint32_t GetInterval(void) const { return interval_; }
  uint32_t GetStartTime(void) const { return start_time_; }
  void SetStartTime(const uint32_t tick) { start_time_ = tick; }
  uint32_t TicksRemaining(const uint32_t tick) const {
    /*
         diff = tick - st
         re = int - diff
         re = int - tick + st
         re = st + int - tick
     */

    //  const uint32_t mask = ~(1U<<31U);
    const uint32_t diff = tick - GetStartTime();
    const uint32_t interval = GetInterval();
    uint32_t remaining = 0;
    if (interval > diff) {
      remaining = interval - diff;
    }
    return remaining;
  }
  bool CheckFinished(const uint32_t tick) const {
    return TicksRemaining(tick) == 0;
  }
  int32_t Call(void) {
    if (fp_func_ != nullptr) {
      const int32_t resp = fp_func_();
      if (fp_callback_ != nullptr) {
        fp_callback_(resp);
      }
      return resp;
    }
    return 0;
  }

  explicit CooperativeTask(uint32_t interval, uint32_t start_time,
                           int32_t (*const func)(void),
                           void (*const callback)(int32_t) = nullptr)
      : interval_{interval},
        start_time_{start_time},
        fp_func_{func},
        fp_callback_{callback} {}

#if 0
    CooperativeTask(void):
        priority_{0}, interval_{0}, start_time_{0}, fp_func_{nullptr}, fp_callback_{nullptr}  {}
#endif
};

class PriorityCooperativeTask : public CooperativeTask {
  const uint32_t priority_;

 public:
  uint32_t GetPriority(void) const { return priority_; }
  PriorityCooperativeTask(uint32_t priority, uint32_t interval,
                          uint32_t start_time, int32_t (*const func)(void),
                          void (*const callback)(int32_t) = nullptr)
      : CooperativeTask{interval, start_time, func, callback},
        priority_{priority} {}
};

class StaticTaskScheduler {
 private:
  bool ready_flag_ = false;
  CooperativeTask* task_table_ = nullptr;
  std::size_t task_count_ = 0;

  bool CheckReady(void) const { return ready_flag_; }
  void SetReady(void) { ready_flag_ = true; }

 public:
  /*
      Finds the first task available and returns its index.
  */
  uint32_t GetNextAvailable(const uint32_t tick) {
    for (std::size_t i = 0; i < task_count_; i++) {
      CooperativeTask& task = task_table_[i];
      if (task.CheckFinished(tick)) {
        return i;
      }
    }
    return static_cast<uint32_t>(task_count_ - 1);  //  Runs last task
  }

  void SetTaskList(CooperativeTask* const task_table, const std::size_t tasks) {
    assert(task_table[tasks - 1].GetInterval() ==
           0);  //  housekeeping task needs to be last entry
    for (uint32_t i = 0; i < tasks - 1 /*Skip last*/; i++) {
      assert(task_table[i].GetInterval() > 0);
    }
    if (!CheckReady()) {
      task_table_ = task_table;
      task_count_ = tasks;

      if (task_table_ != nullptr && task_count_ > 0) {
        SetReady();
      }
    }
  }

  void RunNextTask(const uint32_t tick) {
    volatile uint32_t index = GetNextAvailable(tick);
    CooperativeTask& next_task_ = task_table_[index];
    //  int32_t ReturnValue = next_task_.Call();
    next_task_.Call();
    next_task_.SetStartTime(tick);  //  restart
  }

  StaticTaskScheduler(void) = default;

 private:
  StaticTaskScheduler(const StaticTaskScheduler&) = delete;
  void operator=(const StaticTaskScheduler&) = delete;
};

#endif  //  COOPERATIVESCHEDULER_COOPERATIVESCHEDULER_H_
