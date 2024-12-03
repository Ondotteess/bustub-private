//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.h
//
// Identification: src/include/buffer/buffer_pool_manager.h
//
// Copyright (c) 2015-2024, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "buffer/lru_k_replacer.h"
#include "common/config.h"
#include "recovery/log_manager.h"
#include "storage/disk/disk_scheduler.h"
#include "storage/page/page.h"
#include "storage/page/page_guard.h"

namespace bustub {

class BufferPoolManager;
class ReadPageGuard;
class WritePageGuard;

/**
 * @brief A helper class for `BufferPoolManager` that manages a frame of memory and related metadata.
 *
 * This class represents headers for frames of memory that the `BufferPoolManager` stores pages of data into. Note that
 * the actual frames of memory are not stored directly inside a `FrameHeader`, rather the `FrameHeader`s store pointer
 * to the frames and are stored separately them.
 *
 * ---
 *
 * Something that may (or may not) be of interest to you is why the field `data_` is stored as a vector that is
 * allocated on the fly instead of as a direct pointer to some pre-allocated chunk of memory.
 *
 * In a traditional production buffer pool manager, all memory that the buffer pool is intended to manage is allocated
 * in one large contiguous array (think of a very large `malloc` call that allocates several gigabytes of memory up
 * front). This large contiguous block of memory is then divided into contiguous frames. In other words, frames are
 * defined by an offset from the base of the array in page-sized (4 KB) intervals.
 *
 * In BusTub, we instead allocate each frame on its own (via a `std::vector<char>`) in order to easily detect buffer
 * overflow with address sanitizer. Since C++ has no notion of memory safety, it would be very easy to cast a page's
 * data pointer into some large data type and start overwriting other pages of data if they were all contiguous.
 *
 * If you would like to attempt to use more efficient data structures for your buffer pool manager, you are free to do
 * so. However, you will likely benefit significantly from detecting buffer overflow in future projects (especially
 * project 2).
 */
class FrameHeader {
  friend class BufferPoolManager;
  friend class ReadPageGuard;
  friend class WritePageGuard;

 public:
  explicit FrameHeader(frame_id_t frame_id);
  auto GetPageId() const -> page_id_t { return page_id_; }
  void SetPageId(page_id_t page_id) { page_id_ = page_id; }
  void Reset();

 private:
  auto GetData() const -> const char *;
  auto GetDataMut() -> char *;
  page_id_t page_id_{INVALID_PAGE_ID};

  /** @brief The frame ID / index of the frame this header represents. */
  const frame_id_t frame_id_;

  /** @brief The readers / writer latch for this frame. */
  std::shared_mutex rwlatch_;

  /** @brief The number of pins on this frame keeping the page in memory. */
  std::atomic<size_t> pin_count_;

  /** @brief The dirty flag. */
  bool is_dirty_;
public:
  /**
   * @brief A pointer to the data of the page that this frame holds.
   *
   * If the frame does not hold any page data, the frame contains all null bytes.
   */
  std::vector<char> data_;
  /**
   * TODO(P1): You may add any fields or helper functions under here that you think are necessary.
   *
   * One potential optimization you could make is storing an optional page ID of the page that the `FrameHeader` is
   * currently storing. This might allow you to skip searching for the corresponding (page ID, frame ID) pair somewhere
   * else in the buffer pool manager...
   */
};

/**
 * @brief The declaration of the `BufferPoolManager` class.
 *
 * As stated in the writeup, the buffer pool is responsible for moving physical pages of data back and forth from
 * buffers in main memory to persistent storage. It also behaves as a cache, keeping frequently used pages in memory for
 * faster access, and evicting unused or cold pages back out to storage.
 *
 * Make sure you read the writeup in its entirety before attempting to implement the buffer pool manager. You also need
 * to have completed the implementation of both the `LRUKReplacer` and `DiskManager` classes.
 */
class BufferPoolManager {
 public:
  BufferPoolManager(size_t num_frames, DiskManager *disk_manager, size_t k_dist = LRUK_REPLACER_K,
                    LogManager *log_manager = nullptr);
  ~BufferPoolManager();

  auto Size() const -> size_t;
  page_id_t FindPageToDelete();auto NewPage() -> page_id_t;
  auto DeletePage(page_id_t page_id) -> bool;
  auto CheckedWritePage(page_id_t page_id, AccessType access_type = AccessType::Unknown)
      -> std::optional<WritePageGuard>;
  auto CheckedReadPage(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> std::optional<ReadPageGuard>;
  auto WritePage(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> WritePageGuard;
  auto ReadPage(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> ReadPageGuard;
  auto FlushPage(page_id_t page_id) -> bool;
  void FlushAllPages();
  auto GetPinCount(page_id_t page_id) -> std::optional<size_t>;


template <typename PageGuard>
auto ManagePage(page_id_t page_id, AccessType access_type, bool is_write)
    -> std::optional<PageGuard> {
  std::shared_ptr<FrameHeader> frame;
  frame_id_t frame_id;

  /**
   *    1) проверить во frames
   *        - если есть осталось только зарегестрировать доступ
   *
   *    2) во frames не нашлось
   *        - найти кого выселить
   *        - выгрузить его во внешнее хранилище
   *        - сбросить фрейм и обновить таблицу
   *        - загрузить фрейм и еще раз обновить таблицу
   *        - зарегестрировать доступ
   ***/


  {
    std::scoped_lock lock(*bpm_latch_);

    if (const auto it = page_table_.find(page_id);it != page_table_.end()) {
      frame_id = it->second;
      frame = frames_[frame_id];
    } else {
      const auto evctd_frame = replacer_->Evict();
      if (!evctd_frame.has_value()) {
        return std::nullopt;

      }

      frame_id = evctd_frame.value();
      frame = frames_[frame_id];
      if (frame->is_dirty_) {
        std::promise<bool> write_promise;
        const auto write_future = write_promise.get_future();

        disk_scheduler_->Schedule({true, frame->GetDataMut(), frame->GetPageId(), std::move(write_promise)});
        write_future.wait();
        frame->is_dirty_ = false;
      }



      page_table_.erase(std::find_if(page_table_.begin(), page_table_.end(),
                                     [&](const auto &pair) { return pair.second == frame_id; }));
      frame->Reset();

      std::promise<bool> read_promise;
      const auto read_future = read_promise.get_future();
      disk_scheduler_->Schedule({false, frame->GetDataMut(), page_id, std::move(read_promise)});
      read_future.wait();

      page_table_[page_id] = frame_id;
    }

    frame->pin_count_.fetch_add(1, std::memory_order_relaxed);
    replacer_->RecordAccess(frame_id);
    replacer_->SetEvictable(frame_id, false);
  }

  if (is_write) {
    std::unique_lock rwlatch_lock(frame->rwlatch_);
  } else {
    std::shared_lock rwlatch_lock(frame->rwlatch_);
  }


  // std::unique_lock rwlatch_lock(frame->rwlatch_);


  return PageGuard(page_id, frame, replacer_, bpm_latch_);
}

  /** @brief The frame headers of the frames that this buffer pool manages. */
  // frame и data пришлось временно унести в public чтобы отлаживать сиаутами
  std::vector<std::shared_ptr<FrameHeader>> frames_;

 private:
  /** @brief The number of frames in the buffer pool. */
  const size_t num_frames_;

  /** @brief The next page ID to be allocated.  */
  std::atomic<page_id_t> next_page_id_;

  /**
   * @brief The latch protecting the buffer pool's inner data structures.
   *
   * TODO(P1) We recommend replacing this comment with details about what this latch actually protects.
   */
  std::shared_ptr<std::mutex> bpm_latch_;


  /** @brief The page table that keeps track of the mapping between pages and buffer pool frames. */
  std::unordered_map<page_id_t, frame_id_t> page_table_;

  /** @brief A list of free frames that do not hold any page's data. */
  std::list<frame_id_t> free_frames_;

  /** @brief The replacer to find unpinned / candidate pages for eviction. */
  std::shared_ptr<LRUKReplacer> replacer_;

  /** @brief A pointer to the disk scheduler. */
  std::unique_ptr<DiskScheduler> disk_scheduler_;

  /**
   * @brief A pointer to the log manager.
   *
   * Note: Please ignore this for P1.
   */
  LogManager *log_manager_ __attribute__((__unused__));

  /**
   * TODO(P1): You may add additional private members and helper functions if you find them necessary.
   *
   * There will likely be a lot of code duplication between the different modes of accessing a page.
   *
   * We would recommend implementing a helper function that returns the ID of a frame that is free and has nothing
   * stored inside of it. Additionally, you may also want to implement a helper function that returns either a shared
   * pointer to a `FrameHeader` that already has a page's data stored inside of it, or an index to said `FrameHeader`.
   */
};
}  // namespace bustub
