/*
 * Copyright 2020 Electrooptical Innovations
 * */
#include "test_CooperativeScheduler.h"
#include <gtest/gtest.h>
#include <array>
#include <iostream>
#include <vector>

int32_t TestFunc(void) { return 0; }

int32_t HouseKeeping(void) {
  //std::cout << __FUNCTION__ << std::endl;
  return 0;
}
int32_t StartADCBurst(void) {
  //std::cout << __FUNCTION__ << std::endl;
  return 0;
}
int32_t SendDataFrame(void) {
  //std::cout << __FUNCTION__ << std::endl;
  return 0;
}
int32_t CheckForSerialCommand(void) {
  //std::cout << __FUNCTION__ << std::endl;
  return 0;
}
int32_t ReadPressureSensor(void) {
  //std::cout << __FUNCTION__ << std::endl;
  return 0;
}
int32_t ReadAmbientLightSensor(void) {
  //std::cout << __FUNCTION__ << std::endl;
  return 0;
}
int32_t CheckPowerSupplies(void) {
  //std::cout << __FUNCTION__ << std::endl;
  return 0;
}

const uint32_t ReadADCInterval = 200;
namespace SETTINGS {
const uint32_t SysTickFrequency = 1000;
}

TEST(CooperativeScheduler, StaticTable) {
  uint32_t SystemTime = 2;
  std::array<CooperativeTask, 7> Table{
      CooperativeTask{SETTINGS::SysTickFrequency / ReadADCInterval, SystemTime,
       &StartADCBurst},
      CooperativeTask{SETTINGS::SysTickFrequency / ReadADCInterval, SystemTime,
       &SendDataFrame},
      CooperativeTask{SETTINGS::SysTickFrequency / 25, SystemTime, &CheckForSerialCommand},
      CooperativeTask{SETTINGS::SysTickFrequency, SystemTime, &ReadPressureSensor},
      CooperativeTask{SETTINGS::SysTickFrequency, SystemTime, &ReadAmbientLightSensor},
      CooperativeTask{SETTINGS::SysTickFrequency, SystemTime, &CheckPowerSupplies},
      CooperativeTask{0, SystemTime, &HouseKeeping},
  };
  StaticTaskScheduler sch;
  sch.SetTaskList(Table.data(), Table.size());

  for (uint32_t i = 1; i < 2 * SETTINGS::SysTickFrequency; i++) {
    //std::cout << static_cast<int>(i) << "\t";
    sch.RunNextTask(i);
  }
}
