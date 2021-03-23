// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "spdy/core/hpack/hpack_static_table.h"

#include "absl/strings/string_view.h"
#include "spdy/core/hpack/hpack_constants.h"
#include "spdy/core/hpack/hpack_entry.h"
#include "spdy/platform/api/spdy_estimate_memory_usage.h"
#include "spdy/platform/api/spdy_logging.h"

namespace spdy {

HpackStaticTable::HpackStaticTable() = default;

HpackStaticTable::~HpackStaticTable() = default;

void HpackStaticTable::Initialize(const HpackStaticEntry* static_entry_table,
                                  size_t static_entry_count) {
  QUICHE_CHECK(!IsInitialized());

  int insertion_count = 0;
  for (const HpackStaticEntry* it = static_entry_table;
       it != static_entry_table + static_entry_count; ++it) {
    absl::string_view name(it->name, it->name_len);
    absl::string_view value(it->value, it->value_len);
    static_entries_.emplace_back(name, value);
    HpackEntry* entry = &static_entries_.back();
    auto result = static_index_.insert(std::make_pair(
        HpackLookupEntry{entry->name(), entry->value()}, insertion_count));
    QUICHE_CHECK(result.second);
    // Multiple static entries may have the same name, so inserts may fail.
    static_name_index_.insert(std::make_pair(entry->name(), insertion_count));

    ++insertion_count;
  }
}

bool HpackStaticTable::IsInitialized() const {
  return !static_entries_.empty();
}

size_t HpackStaticTable::EstimateMemoryUsage() const {
  return SpdyEstimateMemoryUsage(static_entries_) +
         SpdyEstimateMemoryUsage(static_index_) +
         SpdyEstimateMemoryUsage(static_name_index_);
}

}  // namespace spdy
