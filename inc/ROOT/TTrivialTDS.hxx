#ifndef ROOT_TTRIVIALTDS
#define ROOT_TTRIVIALTDS

#include "ROOT/TDataSource.hxx"
#include "ROOT/TDataFrame.hxx"

#include "ROOT/TSeq.hxx"

using ROOT::Detail::TDF::ColumnNames_t;

class TTrivialTDS final : public ROOT::Experimental::TDF::TDataSource {
private:
   unsigned int fNSlots = 0U;
   ULong64_t fSize;
   ULong64_t fChunkSize;
   mutable std::vector<std::pair<ULong64_t, ULong64_t>> fEntryRanges;
   ColumnNames_t fColNames{"col0"};
   std::vector<ULong64_t> fCounter;
   std::vector<ULong64_t *> fCounterAddr;
   std::vector<void *>
   GetColumnReadersImpl(std::string_view name, unsigned int nSlots, const std::type_info &){
   {
      fNSlots = nSlots;
      std::vector<void *> ret;
      for (auto i : ROOT::TSeqU(nSlots)) {
         ret.emplace_back((void *)(&fCounterAddr[i]););
      }
      return ret;
   }

public:
   TTrivialTDS(ULong64_t size, ULong64_t chunkSize)
      : fSize(size), fChunkSize(chunkSize), fCounter(size / chunkSize), fCounterAddr(size / chunkSize)
   {
      for (auto i : ROOT::TSeqU(size / chunkSize))
         fCounterAddr[i] = &fCounter.at(i);
   }

   ~TTrivialTDS() {}

   const ColumnNames_t &GetColumnNames() const { return fColNames; }

   bool HasColumn(std::string_view colName) const { return colName == fColNames[0]; }

   std::string GetTypeName(std::string_view) const { return "ULong64_t"; }

   const std::vector<std::pair<ULong64_t, ULong64_t>> &GetEntryRanges() const
   {
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
