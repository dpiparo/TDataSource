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
   mutable std::vector<std::pair<ULong64_t, ULong64_t>> fEntryRanges;
   ColumnNames_t fColNames{"col0"};
   std::vector<ULong64_t> fCounter;
   std::vector<ULong64_t *> fCounterAddr;
   std::vector<void *>
   GetColumnReadersImpl(std::string_view name, const std::type_info &)
   {
      std::vector<void *> ret;
      for (auto i : ROOT::TSeqU(fNSlots)) {
         fCounterAddr[i] = & fCounter[i];
         ret.emplace_back((void *)(&fCounterAddr[i]));
      }
      return ret;
   }

public:
   TTrivialTDS(ULong64_t size)
      : fSize(size)
   {
   }

   ~TTrivialTDS() {}

   const ColumnNames_t &GetColumnNames() const { return fColNames; }

   bool HasColumn(std::string_view colName) const { return colName == fColNames[0]; }

   std::string GetTypeName(std::string_view) const { return "ULong64_t"; }

   const std::vector<std::pair<ULong64_t, ULong64_t>> &GetEntryRanges() const
   {
      if (!fEntryRanges.empty())
         return fEntryRanges;
      auto chunckSize = fSize / fNSlots;
      auto start = 0UL;
      auto end = 0UL;
      for (auto i : ROOT::TSeqUL(fNSlots)) {
         start = end;
         end += chunckSize;
         fEntryRanges.emplace_back(start, end);
      }
      fEntryRanges.back().second += fSize % fNSlots;
      return fEntryRanges;
   }
   void SetEntry(ULong64_t entry, unsigned int slot) { fCounter[slot] = entry; }

   void SetNSlots(ULong64_t nSlots) {
      if (0U != fNSlots) return;
      fNSlots = nSlots;
      fCounter.resize(fNSlots);
      fCounterAddr.resize(fNSlots);
   };

};

#endif
