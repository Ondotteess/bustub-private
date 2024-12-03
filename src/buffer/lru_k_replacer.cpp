//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include "common/exception.h"

namespace bustub {
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

auto LRUKReplacer::Evict() -> std::optional<frame_id_t> {
  std::lock_guard guard(latch_);

  frame_id_t frame = INVALID_FRAME_ID;
  size_t max_dst = 0;
  bool flag = false;

  for (auto &[frame_id, node] : node_store_) {
    if (!node.is_evictable_) {
      continue;
    }

    size_t k_dst = node.GetKDistance(current_timestamp_);
    if (k_dst > max_dst ||
      (k_dst == max_dst && node.history_.front() < node_store_[frame]
                                                                      .history_
                                                                      .front())) {
      max_dst = k_dst;
      frame = frame_id;
      flag = true;
    }
  }

  if (flag) {
    node_store_.erase(frame);
    curr_size_--;
    return frame;
  }

  return std::nullopt;
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id, AccessType access_type) {
  std::lock_guard guard(latch_);

  if (frame_id < 0 || static_cast<size_t>(frame_id) >= replacer_size_) {
    throw Exception("Invalid frame_id");
  }

  current_timestamp_++;

  if (node_store_.find(frame_id) == node_store_.end()) {
    //std::cout << "Node store size before emplace: " << node_store_.size() << std::endl;
    node_store_.emplace(frame_id, LRUKNode(frame_id, k_));
    //std::cout << "Node store size after emplace: " << node_store_.size() << std::endl;
    //std::cout << node_store_.at(frame_id).fid_ << std::endl;
    // отладчик clion пишет что node_store_ всегда пустой
  }

  node_store_[frame_id].RecordAccess(current_timestamp_);
}

auto LRUKReplacer::SetEvictable(const frame_id_t frame_id, const bool set_evictable) -> void {
  std::lock_guard guard(latch_);

  // если фрейм не нашли просто выходим
  // а если индекс не корректный бросаем исключение

  if (frame_id < 0 || static_cast<size_t>(frame_id) >= replacer_size_) {
    throw Exception("invalid frame_id");
  }

  const auto it = node_store_.find(frame_id);
  if (it == node_store_.end()) {
    return;
  }

  if (auto &node = it->second ;node.is_evictable_ != set_evictable) {
    curr_size_ += set_evictable ? 1 : -1;
    node.is_evictable_ = set_evictable;
  }
}

void LRUKReplacer::Remove(const frame_id_t frame_id) {
  std::lock_guard guard(latch_);

  // std::cout  << "size: " << node_store_.size() << std::endl;
  for (auto & it : node_store_) {
    std::cout << it.second.fid_ << " " << it.second.is_evictable_<< std::endl;
  }
  //if (node_store_.find(frame_id) == node_store_.end() || !node_store_[frame_id].is_evictable_) {
  //  throw bustub::Exception("Cannot remove non-evictable frame");
  //}

  node_store_.erase(frame_id);
  curr_size_--;
}

auto LRUKReplacer::Size() -> size_t {
  std::lock_guard guard(latch_);
  return curr_size_;
}

}  // namespace bustub
