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

auto LRUKReplacer::ShouldEvicted(const LRUKNode &node, frame_id_t candidate_frame_id) -> bool {
  size_t k_distance = node.GetKDistance(current_timestamp_);
  const auto &candidate_node = node_store_.at(candidate_frame_id);

  return k_distance > candidate_node.GetKDistance(current_timestamp_) ||
         (k_distance == candidate_node.GetKDistance(current_timestamp_) &&
          node.history_.front() < candidate_node.history_.front());
}

auto LRUKReplacer::Evict() -> std::optional<frame_id_t> {
  std::scoped_lock scoped(latch_);

  if (curr_size_ == 0) {
    // std::cout << "eviction failed: no evictable frames" << std::endl;
    return std::nullopt;
  }

  frame_id_t candidate_frame = INVALID_FRAME_ID;

  for (const auto &[frame_id, node] : node_store_) {
    if (!node.is_evictable_) {
      continue;
    }

    if (candidate_frame == INVALID_FRAME_ID || ShouldEvicted(node, candidate_frame)) {
      candidate_frame = frame_id;
    }
  }

  if (candidate_frame != INVALID_FRAME_ID) {
    node_store_.erase(candidate_frame);
    curr_size_--;
    // std::cout << "evicted frame: " << candidate_frame << std::endl;
    return candidate_frame;
  }
  // std::cout << "no suitable frame found for eviction" << std::endl;
  return std::nullopt;
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
  BUSTUB_ASSERT(static_cast<size_t>(frame_id) < replacer_size_, "Invalid frame_id");
  std::scoped_lock scoped(latch_);

  if (node_store_.count(frame_id) == 0) {
    node_store_[frame_id] = LRUKNode{frame_id, k_};
    // std::cout << "added new frame to node store: " << frame_id << std::endl;
  }

  auto &node = node_store_[frame_id];
  node.history_.push_back(current_timestamp_++);

  if (node.history_.size() > node.k_) {
    // std::cout << "trimmed history for frame: " << frame_id << std::endl;
    node.history_.pop_front();
  }
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  BUSTUB_ASSERT(static_cast<size_t>(frame_id) < replacer_size_, "Invalid frame_id");
  std::scoped_lock lock(latch_);

  if (auto &node = node_store_[frame_id]; node.is_evictable_ != set_evictable) {
    curr_size_ += set_evictable ? 1 : -1;
    node.is_evictable_ = set_evictable;
    // std::cout << "frame " << frame_id << (set_evictable ? " marked as evictable" : " marked as non-evictable") <<
    // std::endl;
  }
}

void LRUKReplacer::Remove(frame_id_t frame_id) {
  std::scoped_lock lock(latch_);

  BUSTUB_ASSERT(static_cast<size_t>(frame_id) < replacer_size_, "Invalid frame_id");

  auto it = node_store_.find(frame_id);
  if (it == node_store_.end()) {
    return;
  }

  if (!it->second.is_evictable_) {
    throw bustub::Exception("trying to remove non-evictable frame");
  }

  node_store_.erase(it);
  curr_size_--;
  // std::cout << "removed frame: " << frame_id << std::endl;
}

auto LRUKReplacer::Size() -> size_t { return curr_size_; }

}  // namespace bustub