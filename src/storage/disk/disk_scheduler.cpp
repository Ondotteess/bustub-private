//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_scheduler.cpp
//
// Identification: src/storage/disk/disk_scheduler.cpp
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/disk/disk_scheduler.h"
#include "common/exception.h"
#include "storage/disk/disk_manager.h"

namespace bustub {

DiskScheduler::DiskScheduler(DiskManager *disk_manager) : disk_manager_(disk_manager) {
  background_thread_.emplace([&] { StartWorkerThread(); });
}

DiskScheduler::~DiskScheduler() {
  request_queue_.Put(std::nullopt);
  if (background_thread_.has_value()) {
    background_thread_->join();
  }
}

void DiskScheduler::Schedule(DiskRequest r) {
  request_queue_.Put(std::move(r));
}


void DiskScheduler::StartWorkerThread() {
  while (true) {
    auto request_ = request_queue_.Get();
    if (!request_.has_value()) {
      break;
    }

    auto &[is_write_, data_, page_id_, callback_] = request_.value();
    try {
      if (is_write_) {
        disk_manager_->WritePage(page_id_, data_);
      } else {
        disk_manager_->ReadPage(page_id_, data_);
      }
      callback_.set_value(true);
    } catch (...) {
      callback_.set_value(false);
    }
  }
}

}  // namespace bustub
