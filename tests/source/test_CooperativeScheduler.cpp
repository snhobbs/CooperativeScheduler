/*
 * Copyright 2020 Electrooptical Innovations
 * */
#include "test_CooperativeScheduler.h"
#include <iostream>
#include <gtest/gtest.h>
#include <vector>

TEST(CooperativeTask, GetSetStartTime) {
  const size_t start = 100;
  CooperativeTask task{1, start, nullptr};
  ASSERT_EQ(start, task.GetStartTime());

  const size_t start_new = 1000;
  task.SetStartTime(start_new);
  ASSERT_EQ(start_new, task.GetStartTime());
}

TEST(CooperativeTask, GetInterval) {
  const size_t interval = 999;
  CooperativeTask task{interval, 0, nullptr};
  ASSERT_EQ(task.GetInterval(), interval);
}

TEST(CooperativeTask, CheckFinishedEqual) {
  const size_t interval = 1;
  const size_t start = 1;
  CooperativeTask task{interval, start, nullptr};
  ASSERT_TRUE(task.CheckFinished(start + interval));
}

TEST(CooperativeTask, CheckFinishedOneOver) {
  const size_t interval = 1;
  const size_t start = 1;
  CooperativeTask task{interval, start, nullptr};
  ASSERT_TRUE(task.CheckFinished(start + interval + 1));
}

TEST(CooperativeTask, CheckFinishedOneUnder) {
  const size_t interval = 1;
  const size_t start = 100;
  CooperativeTask task{interval, start, nullptr};
  ASSERT_FALSE(task.CheckFinished(start-1 + interval));
}

TEST(CooperativeTask, CheckFinishedManyOver) {
  const size_t interval = 1;
  const size_t start = 100;
  CooperativeTask task{interval, start, nullptr};
  ASSERT_TRUE(task.CheckFinished(start + interval + 1000));
}

TEST(CooperativeTask, CallNullptrNoCrash) {
  const size_t interval = 1;
  const size_t start = 100;
  CooperativeTask task{interval, start, nullptr};
  task.Call();
}

TEST(CooperativeTask, TicksRemainingNormal) {
  // Check Rollover
  const uint32_t interval = 10;
  constexpr uint32_t StartTime = -500;
  constexpr uint64_t EndTime =
      static_cast<uint64_t>(StartTime) + 1000; //(1UL<<32)+1000;
  CooperativeTask vs{interval, StartTime, nullptr};
  static_assert(StartTime < EndTime, "Mussed up");

  uint32_t cnt = 0;
  for (uint64_t i = StartTime; i < EndTime; i++) {
    uint32_t tick = static_cast<uint32_t>(i);
    uint32_t Remaining = vs.TicksRemaining(tick);

    cnt++;
    if (cnt > vs.GetInterval()) {
      ASSERT_EQ(0, Remaining);
    } else {
      ASSERT_TRUE(Remaining > 0);
    }
    ASSERT_TRUE(Remaining <= vs.GetInterval());
  }
}
TEST(CooperativeTask, TicksRemainingRollover) {
  // Check Rollover
  const uint32_t Interval = 100;
  constexpr uint32_t StartTime = -Interval + 10;
  constexpr uint64_t EndTime =
      static_cast<uint64_t>(StartTime) + 1000; //(1UL<<32)+1000;
  CooperativeTask vs{Interval, StartTime, nullptr};
  static_assert(StartTime < EndTime, "Mussed up");

  uint32_t cnt = 0;
  for (uint64_t i = StartTime; i < EndTime; i++) {
    uint32_t tick = static_cast<uint32_t>(i);
    uint32_t Remaining = vs.TicksRemaining(tick);

    cnt++;
    if (cnt > vs.GetInterval()) {
      ASSERT_EQ(0, Remaining);
    } else {
      ASSERT_TRUE(Remaining > 0);
    }
    ASSERT_TRUE(Remaining <= vs.GetInterval());
  }
}
TEST(CooperativeTask, TicksRemainingEdge0) {
  // Check Rollover
  const uint32_t Interval = 100;
  constexpr uint32_t StartTime = -(Interval);
  constexpr uint64_t EndTime =
      static_cast<uint64_t>(StartTime) + 1000; //(1UL<<32)+1000;
  CooperativeTask vs{Interval, StartTime, nullptr};
  static_assert(StartTime < EndTime, "Mussed up");

  uint32_t cnt = 0;
  for (uint64_t i = StartTime; i < EndTime; i++) {
    uint32_t tick = static_cast<uint32_t>(i);
    uint32_t Remaining = vs.TicksRemaining(tick);

    cnt++;
    if (cnt > vs.GetInterval()) {
      ASSERT_EQ(0, Remaining);
    } else {
      ASSERT_TRUE(Remaining > 0);
    }
    ASSERT_TRUE(Remaining <= vs.GetInterval());
  }
}

#if 0
int32_t TestFunc(void) { return 0; }

TEST(ValidateTaskLink) {
  TaskLink tl;
  TaskLink *ptl{nullptr};
  TaskScheduler taskManager;
  ASSERT_EQ(taskManager.ValidateTaskLink(&tl), false);
  ASSERT_EQ(taskManager.ValidateTaskLink(ptl), false);

  ptl = &tl;
  ASSERT_EQ(taskManager.ValidateTaskLink(ptl), false);

  // Callback function being nullptr is allowed
  CooperativeTask vs{0, 10, 0, nullptr};
  tl.p_task = &vs;
  tl.link = nullptr;
  ASSERT_EQ(taskManager.ValidateTaskLink(&tl), true);
  ASSERT_EQ(taskManager.ValidateTaskLink(ptl), true);

  CooperativeTask ct{0, 10, 0, TestFunc};
  tl.p_task = &ct;

  ASSERT_EQ(taskManager.ValidateTaskLink(&tl), true);
  ASSERT_EQ(taskManager.ValidateTaskLink(ptl), true);
}
#endif

#if 0
TEST(AddTask) {
  const uint32_t Interval = 100;
  constexpr uint32_t StartTime = -(Interval);
  CooperativeTask vs{0, Interval, StartTime, nullptr, false};

  TaskScheduler taskManager;

  // invalid so wont add
  TaskLink tl0{nullptr, nullptr};
  taskManager.AddTask(&tl0, StartTime);
  taskManager.AddTask(&tl0, StartTime);
  taskManager.AddTask(&tl0, StartTime);
  taskManager.AddTask(&tl0, StartTime);
  ASSERT_EQ(0, taskManager.GetNumTasks());

  TaskLink tl1{nullptr, &vs};
  TaskLink tl2{nullptr, &vs};
  TaskLink tl3{nullptr, &vs};
  taskManager.AddTask(&tl1, StartTime);
  taskManager.AddTask(&tl2, StartTime);
  taskManager.AddTask(&tl3, StartTime);
  ASSERT_EQ(3, taskManager.GetNumTasks());
}

TEST(GetNextPriority) {}
TEST(GetNextAvailable) {}
TEST(PopFromFree) {}

TEST(RemoveTask) {}
TEST(RunNextTask) {}
TEST(RestartOnFinish) {}
#endif
