/*
 * Copyright 2020 ElectroOptical Innovations, LLC
 * */

#pragma once
#ifndef COOPERATIVESCHEDULER_COOPERATIVESCHEDULER_H_
#define COOPERATIVESCHEDULER_COOPERATIVESCHEDULER_H_

#include <cstdint>
#include <cassert>
#include <limits>
/**
 * Simple reusable cooperative task scheduler. The Tick Count needs to be incrimented by a periodic timer, i.e. systick.
 * Tasks have an implied priority level, the first finished task in the list is run if multiple are scheduled
 */
class CooperativeTask{
    const uint32_t interval_;
    uint32_t start_time_;
    int32_t(* fp_func_)(void) {nullptr};
    void(*const fp_callback_)(int32_t) {nullptr};

 public:
    void SetCallFunction(int32_t(* fp_func)(void)) {
        fp_func_ = fp_func;
    }
    uint32_t GetInterval(void) const {
        return interval_;
    }
    uint32_t GetStartTime(void) const {
        return start_time_;
    }
    void SetStartTime(const uint32_t tick) {
        start_time_ = tick;
    }
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

    explicit CooperativeTask(uint32_t interval, uint32_t start_time, int32_t(*const func)(void), void(*const callback)(int32_t) = nullptr):
        interval_{interval}, start_time_{start_time}, fp_func_{func}, fp_callback_{callback}  {}

#if 0
    CooperativeTask(void):
        priority_{0}, interval_{0}, start_time_{0}, fp_func_{nullptr}, fp_callback_{nullptr}  {}
#endif
};


class PriorityCooperativeTask : public CooperativeTask{
    const uint32_t priority_;
 public:
    uint32_t GetPriority(void) const {
        return priority_;
    }
    PriorityCooperativeTask(uint32_t priority, uint32_t interval, uint32_t start_time, int32_t(*const func)(void), void(*const callback)(int32_t) = nullptr):
        CooperativeTask{interval, start_time, func, callback}, priority_{priority}{}
};


class StaticTaskScheduler{
 private:
    bool ready_flag_ = false;
    CooperativeTask* task_table_ = nullptr;
    std::size_t task_count_ = 0;

    bool CheckReady(void) const {
        return ready_flag_;
    }
    void SetReady(void) {
        ready_flag_ = true;
    }

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
        assert(task_table[tasks - 1].GetInterval() == 0);  //  housekeeping task needs to be last entry
        for (uint32_t i = 0; i < tasks -1 /*Skip last*/; i++) {
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

#if 0
struct TaskLink;

struct TaskLink{
    TaskLink* link{nullptr};
    CooperativeTask* p_task{nullptr};
    int32_t ReturnValue{0};
};



/**
    Gets a pointer to first element in a linked list of tasks
    Tasks are linked in order of the next to complete.
    So house keeping tasks will appear at the beginning of the list but have the
    lowest priority.
*/

class TaskScheduler{
 private:
    TaskLink* HeadTask{nullptr};
    TaskLink* HeadFreeList{nullptr};
    TaskLink* HeadFinishList{nullptr};

    /*
        finds the highest priority task thats ready to run
        Lower value -> higher priority
    */
    uint32_t GetNextPriority(const uint32_t tick) const {
        TaskLink* link = HeadTask;
        uint32_t HighestReadyPriority = -1;
        while (link != nullptr) {
            if (link->p_task->CheckFinished(tick)) {
                HighestReadyPriority = link->p_task->GetPriority() < HighestReadyPriority ? link->p_task->GetPriority() : HighestReadyPriority;
                link = link->link;
            } else {
                //  We've hit the first not ready task, return the priority level
               break;
            }
        }
        return HighestReadyPriority;
    }


    /*
        Finds the first task of the highest prioity task available and returns a pointer to it
    */
    TaskLink* GetNextAvailable(const uint32_t tick) {
        const uint32_t RunPriority = GetNextPriority(tick);
        TaskLink* link = HeadTask;
        TaskLink* nextAvailable = nullptr;
        while (link != nullptr) {
            if (link->p_task->CheckFinished(tick)) {
                if (link->p_task->GetPriority() == RunPriority) {
                    nextAvailable = link;
                    break;
                } else {
                    link = link->link;
                }
            } else {
               break;
            }
        }
        return nextAvailable;
    }

    /*
        Find the link in the list, remove from linked list
    */
    void RemoveTask(TaskLink*& head, TaskLink const* finishedLink) {
        TaskLink* link = head;
        TaskLink* lastLink = nullptr;
        /*
            Cycle through active list until we find its position, remove itself and add to front of 
            free list
         */
        while (true) {
            if (link == finishedLink) {
                if (lastLink == nullptr) {
                    assert(finishedLink == HeadTask);  //  HeadTask case
                    if (head->link == nullptr) {
                        head = nullptr;
                    } else {
                        head = head->link;
                    }
                } else {
                    assert(link->link == finishedLink->link);
                    lastLink->link = finishedLink->link;
                }
                break;
            }
            lastLink = link;
            link = link->link;
        }
    }
    static bool ValidateTaskLink(TaskLink const* link) {
        bool valid = true;
        if (link == nullptr) {
            valid = false;
        } else if (link->p_task == nullptr) {
            valid = false;
        }
        return valid;
    }

    static uint32_t GetChainLen(TaskLink const* link) {
        uint32_t cnt = 0;
        while (link != nullptr) {
            link = link->link;
            cnt++;
        }
        return cnt;
    }
    static TaskLink* PopFromList(TaskLink*& link) {
        TaskLink* ptl = nullptr;
        if (link != nullptr) {
            ptl = link;
            if (link->link != nullptr) {
                link = link->link;
            } else {
                link = nullptr;
            }
        }
        return ptl;
    }
    void AddToTail(TaskLink*& head, TaskLink* new_link) {
        TaskLink* link = head;
        if (head != nullptr) {
            while (link->link != nullptr) {
                link = link->link;
            }
        }
        link->link = new_link;
    }
    void AddToHead(TaskLink*& head, TaskLink* new_link) {
        new_link->link = head;
        head = new_link;
    }

 public:
    int32_t AddToFreeList(TaskLink* link) {
        AddToHead(HeadFreeList, link);
#if 0
        link->link = HeadFreeList;
        HeadFreeList = link;
#endif
        return 0;
    }

    int32_t AddToFinished(TaskLink* link) {
        AddToHead(HeadFinishList, link);
#if 0
        link->link = HeadFreeList;
        HeadFreeList = link;
#endif
        return 0;
    }

    void RunNextTask(const uint32_t tick) {
        TaskLink* link = GetNextAvailable(tick);
        if (link != nullptr) {
            link->ReturnValue = link->p_task->Call();
            RemoveTask(HeadTask, link);  //  Remove finished tasks
            AddToFinished(link);
#if 0
            if (link->p_task->GetRestartOnFinish()) {
                TaskLink* ptl = PopFromFree();
                assert(ptl == link);
                ptl->p_task->SetStartTime(tick);
                AddTask(link, tick);
            }
#endif
        }
    }


    /*
        Add a task to the linked list, the list is sorted by time to completion
    */
    void AddTask(TaskLink* new_link, const uint32_t tick) {
        TaskLink* link = HeadTask;
        TaskLink* lastLink = nullptr;
        if (ValidateTaskLink(new_link)) {
            new_link->p_task->SetStartTime(tick);
            uint32_t new_task_time = new_link->p_task->TicksRemaining(tick);
            while (true) {
                if (link == nullptr) {
                    new_link->link = nullptr;  //  last item
                    if (lastLink == nullptr) {
                        //  Empty List
                        HeadTask = new_link;  //  first item
                    } else {
                        lastLink->link = new_link;
                    }
                    break;
                } else if (link->p_task->TicksRemaining(tick) >= new_task_time) {
                    if (lastLink == nullptr) {
                        //  becomes first entry in chain
                        HeadTask = new_link;  //  first item
                        new_link->link = link;
                        assert(new_link->link != nullptr);
                        //  link->link = link->link;
                    } else {
                        //  insert between lastlink and link
                        lastLink->link = new_link;
                        new_link->link = link;
                    }
                    break;
                } else {
                    lastLink = link;
                    link = link->link;
                }
            }
        }
    }
    uint32_t GetNumTasks(void) {
        return GetChainLen(HeadTask);
    }
    uint32_t GetNumFree(void) {
        return GetChainLen(HeadFreeList);
    }
    uint32_t GetNumFinished(void) {
        return GetChainLen(HeadFinishList);
    }
    TaskLink* PopFromFree(void) {
        return PopFromList(HeadFreeList);
    }
    TaskLink* PopFromFinished(void) {
        return PopFromList(HeadFinishList);
    }

 public:
    TaskScheduler(void) = default;

 private:
    TaskScheduler(TaskScheduler const&) = delete;
    void operator=(TaskScheduler const&) = delete;
};
#endif
#endif  //  COOPERATIVESCHEDULER_COOPERATIVESCHEDULER_H_
