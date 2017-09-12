#ifndef ROOT_TTRIVIALTDS
#define ROOT_TTRIVIALTDS

#include "ROOT/TDataSource.hxx"

#include "ROOT/TSeq.hxx"

class TTrivialTDS : public TDataSource {
private:
  ULong64_t fSize;
  ULong64_t fChunkSize;
  mutable std::vector<std::pair<ULong64_t, ULong64_t>> fEntryRanges;
  ColumnNames_t fColNames{"col0"};
  std::vector<ULong64_t> fCounter;
  std::vector<ULong64_t *> fCounterAddr;
  void **GetColumnReaderImpl(const std::type_info &, unsigned int slot) {
    return (void **)(&fCounterAddr[slot]);
  }

public:
  TTrivialTDS(ULong64_t size, ULong64_t chunkSize)
      : fSize(size), fChunkSize(chunkSize), fCounter(size), fCounterAddr(size) {
        for (auto i : ROOT::TSeqU(size)) fCounterAddr[i] = &fCounter[i];
  }

  ~TTrivialTDS() {}

  const ColumnNames_t &GetColumnNames() const { return fColNames; }

  bool HasColumn(std::string_view colName) const {
    return colName == fColNames[0];
  }

  std::string GetTypeName(std::string_view) const { return "ULong64_t"; }

  const std::vector<std::pair<ULong64_t, ULong64_t>> &GetEntryRanges() const {
    if (!fEntryRanges.empty())
      return fEntryRanges;
    auto nChunks = fSize / fChunkSize;
    auto start = 0UL;
    auto end = 0UL;
    for (auto i : ROOT::TSeqUL(nChunks)) {
      start = end;
      end += fChunkSize;
      fEntryRanges.emplace_back(start, end);
    }
    return fEntryRanges;
  }
  void SetEntry(ULong64_t entry, unsigned int slot) { fCounter[slot] = entry; }
};

#endif