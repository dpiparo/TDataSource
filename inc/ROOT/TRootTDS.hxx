#ifndef ROOT_TROOTTDS
#define ROOT_TROOTTDS

#include "ROOT/TDataSource.hxx"

#include "TChain.h"
#include "ROOT/TSeq.hxx"

#include <algorithm>
#include <vector>

class TRootTDS : public TDataSource {
private:
   mutable TChain fModelChain;
   std::string fTreeName;
   std::string fFileNameGlob;
   std::vector<std::string> fRequiredColumns;
   mutable std::vector<std::string> fListOfBranches;
   mutable std::vector<std::pair<ULong64_t, ULong64_t>> fEntryRanges;
   std::vector<std::vector<void *>> fBranchAddresses {}; // first container-> slot, second -> column;

   std::vector<std::unique_ptr<TChain>> fChains;

   void InitAddresses() {
      auto nColumns = GetColumnNames().size();
      // Initialise the entire set of addresses
      std::cout << fNSlots << " this is the number..\n";
      fBranchAddresses.resize(fNSlots, std::vector<void *>(nColumns));
   }

   const void *GetColumnReaderImpl(std::string_view name, unsigned int slot,
                                   const std::type_info & /*TODO do something about type checking*/)
   {
      std::cout << "Branch addresses size = " << fBranchAddresses.size() << std::endl;
      if (fBranchAddresses.empty()) InitAddresses();
      // A cache for the name-index pairs?
      const auto &colNames = GetColumnNames();
      const auto index = std::distance(colNames.begin(), std::find(colNames.begin(), colNames.end(), name));
      return fBranchAddresses[slot][index];
   }

public:
   TRootTDS(std::string_view treeName, std::string_view fileNameGlob)
      : fTreeName(treeName), fFileNameGlob(fileNameGlob), fModelChain(std::string(treeName).c_str())
   {
      fModelChain.Add(fFileNameGlob.c_str());
   }

   ~TRootTDS(){};

   std::string GetTypeName(std::string_view colName) const
   {
      if (!HasColumn(colName)) {
         std::string e = "The dataset does not have column ";
         e += colName;
         throw std::runtime_error(e);
      }
      // TODO: we need to factor out the routine for the branch alone...
      // Maybe a cache for the names?
      return ROOT::Internal::TDF::ColumnName2ColumnTypeName(std::string(colName).c_str(), &fModelChain,
                                                            nullptr /*TCustomColumnBase here*/);
   }

   const ColumnNames_t &GetColumnNames() const
   {
      if (!fListOfBranches.empty())
         return fListOfBranches;
      auto &lob = *fModelChain.GetListOfBranches();
      fListOfBranches.resize(lob.GetEntries());
      std::transform(lob.begin(), lob.end(), fListOfBranches.begin(), [](TObject *o) { return o->GetName(); });
      return fListOfBranches;
   }

   bool HasColumn(std::string_view colName) const
   {
      if (!fListOfBranches.empty())
         GetColumnNames();
      return fListOfBranches.end() != std::find(fListOfBranches.begin(), fListOfBranches.end(), colName);
   }

   void InitSlot(unsigned int slot)
   {
      fChains[slot].reset(new TChain(fTreeName.c_str()));
      fChains[slot]->Add(fFileNameGlob.c_str());
      const auto nRequiredColumns = fRequiredColumns.size();
      auto &theseBranchAddresses = fBranchAddresses[slot];
      for (auto i : ROOT::TSeqU(theseBranchAddresses.size())) {
         fChains[slot]->SetBranchAddress(fRequiredColumns[i].c_str(), theseBranchAddresses[i]);
      }
   }

   const std::vector<std::pair<ULong64_t, ULong64_t>> &GetEntryRanges() const
   {
      // Quick implementation: divide in equal parts.
      // We need to respect the clusters to be fair.
      if (!fEntryRanges.empty())
         return fEntryRanges;
      auto nentries = fModelChain.GetEntriesFast();
      std::cout << fNSlots << std::endl;
      auto chunkSize = nentries / fNSlots;
      auto reminder = nentries % fNSlots;
      auto start = 0UL;
      auto end = 0UL;
      for (auto i : ROOT::TSeqU(fNSlots)) {
         start = end;
         end += chunkSize;
         fEntryRanges.emplace_back(start, end);
      }
      fEntryRanges.back().second += reminder;
      return fEntryRanges;
   }

   void SetEntry(ULong64_t entry, unsigned slot) { fChains[slot]->GetEntry(entry); }
};

#endif
