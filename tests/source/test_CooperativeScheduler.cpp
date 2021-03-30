/*
 * Copyright 2020 Electrooptical Innovations
 * */
#include "test_CooperativeScheduler.h"
#include <iostream>
#include <vector>

#if 0
int32_t TestFunc(void) { return 0; }

TEST(ValidateTaskLink) {
  TaskLink tl;
  TaskLink *ptl{nullptr};
  TaskScheduler taskManager;
  CHECK_EQUAL(taskManager.ValidateTaskLink(&tl), false);
  CHECK_EQUAL(taskManager.ValidateTaskLink(ptl), false);

  ptl = &tl;
  CHECK_EQUAL(taskManager.ValidateTaskLink(ptl), false);

  // Callback function being nullptr is allowed
  CooperativeTask vs{0, 10, 0, nullptr, false};
  tl.p_task = &vs;
  tl.link = nullptr;
  CHECK_EQUAL(taskManager.ValidateTaskLink(&tl), true);
  CHECK_EQUAL(taskManager.ValidateTaskLink(ptl), true);

  CooperativeTask ct{0, 10, 0, TestFunc, false};
  tl.p_task = &ct;

  CHECK_EQUAL(taskManager.ValidateTaskLink(&tl), true);
  CHECK_EQUAL(taskManager.ValidateTaskLink(ptl), true);
}

TEST(TicksRemainingNormal) {
  // Check Rollover
  constexpr uint32_t StartTime = -500;
  constexpr uint64_t EndTime =
      static_cast<uint64_t>(StartTime) + 1000; //(1UL<<32)+1000;
  CooperativeTask vs{0, 10, StartTime, nullptr, false};
  static_assert(StartTime < EndTime, "Mussed up");

  uint32_t cnt = 0;
  for (uint64_t i = StartTime; i < EndTime; i++) {
    uint32_t tick = static_cast<uint32_t>(i);
    uint32_t Remaining = vs.TicksRemaining(tick);

    cnt++;
    if (cnt > vs.GetInterval()) {
      CHECK_EQUAL(0, Remaining);
    } else {
      CHECK(Remaining > 0);
    }
    CHECK(Remaining <= vs.GetInterval());
  }
}
TEST(TicksRemainingRollover) {
  // Check Rollover
  const uint32_t Interval = 100;
  constexpr uint32_t StartTime = -Interval + 10;
  constexpr uint64_t EndTime =
      static_cast<uint64_t>(StartTime) + 1000; //(1UL<<32)+1000;
  CooperativeTask vs{0, Interval, StartTime, nullptr, false};
  static_assert(StartTime < EndTime, "Mussed up");

  uint32_t cnt = 0;
  for (uint64_t i = StartTime; i < EndTime; i++) {
    uint32_t tick = static_cast<uint32_t>(i);
    uint32_t Remaining = vs.TicksRemaining(tick);

    cnt++;
    if (cnt > vs.GetInterval()) {
      CHECK_EQUAL(0, Remaining);
    } else {
      CHECK(Remaining > 0);
    }
    CHECK(Remaining <= vs.GetInterval());
  }
}
TEST(TicksRemainingEdge0) {
  // Check Rollover
  const uint32_t Interval = 100;
  constexpr uint32_t StartTime = -(Interval);
  constexpr uint64_t EndTime =
      static_cast<uint64_t>(StartTime) + 1000; //(1UL<<32)+1000;
  CooperativeTask vs{0, Interval, StartTime, nullptr, false};
  static_assert(StartTime < EndTime, "Mussed up");

  uint32_t cnt = 0;
  for (uint64_t i = StartTime; i < EndTime; i++) {
    uint32_t tick = static_cast<uint32_t>(i);
    uint32_t Remaining = vs.TicksRemaining(tick);

    cnt++;
    if (cnt > vs.GetInterval()) {
      CHECK_EQUAL(0, Remaining);
    } else {
      CHECK(Remaining > 0);
    }
    CHECK(Remaining <= vs.GetInterval());
  }
}

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
  CHECK_EQUAL(0, taskManager.GetNumTasks());

  TaskLink tl1{nullptr, &vs};
  TaskLink tl2{nullptr, &vs};
  TaskLink tl3{nullptr, &vs};
  taskManager.AddTask(&tl1, StartTime);
  taskManager.AddTask(&tl2, StartTime);
  taskManager.AddTask(&tl3, StartTime);
  CHECK_EQUAL(3, taskManager.GetNumTasks());
}

TEST(GetNextPriority) {}
TEST(GetNextAvailable) {}
TEST(PopFromFree) {}

TEST(RemoveTask) {}
TEST(RunNextTask) {}
TEST(RestartOnFinish) {}
#endif
