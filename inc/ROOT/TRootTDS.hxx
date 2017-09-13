#ifndef ROOT_TROOTTDS
#define ROOT_TROOTTDS

#include "ROOT/TDataSource.hxx"
#include "ROOT/TDataFrame.hxx"

#include "TChain.h"
#include "TClass.h"
#include "ROOT/TSeq.hxx"

#include <algorithm>
#include <vector>

using ROOT::Detail::TDF::ColumnNames_t;

class TRootTDS final : public ROOT::Experimental::TDF::TDataSource {
private:
   unsigned int fNSlots = 1U;
   mutable TChain fModelChain;
   std::string fTreeName;
   std::string fFileNameGlob;
   mutable std::vector<std::string> fListOfBranches;
   mutable std::vector<std::pair<ULong64_t, ULong64_t>> fEntryRanges;
   std::vector<std::vector<void *>> fBranchAddresses; // first container-> slot, second -> column;

   std::vector<std::unique_ptr<TChain>> fChains;

   void InitAddresses() {
      auto nColumns = GetColumnNames().size();
      // Initialise the entire set of addresses
      fBranchAddresses.resize(nColumns, std::vector<void *>(fNSlots));
   }

   std::vector<void *>
   GetColumnReadersImpl(std::string_view name, unsigned int nSlots, const std::type_info &){
      fNSlots = nSlots;
      if (fBranchAddresses.empty()) {
         InitAddresses();
         fChains.resize(fNSlots);
      }

      const auto &colNames = GetColumnNames();
      const auto index = std::distance(colNames.begin(), std::find(colNames.begin(), colNames.end(), name));
      std::vector<void *> ret(nSlots);
      for (auto slot : ROOT::TSeqU(nSlots)) {
         ret[slot] = (void*) & fBranchAddresses[index][slot];
      }
      return ret;

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
      auto typeName = ROOT::Internal::TDF::ColumnName2ColumnTypeName(std::string(colName).c_str(), &fModelChain,
                                                                     nullptr /*TCustomColumnBase here*/);
      // We may not have yet loaded the library where the dictionary of this type is
      TClass::GetClass(typeName.c_str());
      return typeName;
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

   void InitSlot(unsigned int slot, ULong64_t firstEntry)
   {
      if (fChains[slot]) {
         std::cout << "The slot is initialised, why are you calling this twice?\n";
         return;
      }
      fChains[slot].reset(new TChain(fTreeName.c_str()));
      fChains[slot]->Add(fFileNameGlob.c_str());
      fChains[slot]->GetEntry(firstEntry);
      auto &theseBranchAddresses = fBranchAddresses[slot];
      for (auto i : ROOT::TSeqU(fListOfBranches.size())) {
         fChains[slot]->SetBranchAddress(fListOfBranches.at(i).c_str(), &fBranchAddresses.at(i).at(slot));
      }
      for (auto&& upc : fChains) std::cout << "Chain ptr per slot " << upc.get() << std::endl;
   }

   const std::vector<std::pair<ULong64_t, ULong64_t>> &GetEntryRanges() const
   {
      // Quick implementation: divide in equal parts.
      // We need to respect the clusters to be fair.
      if (!fEntryRanges.empty())
         return fEntryRanges;
      auto nentries = fModelChain.GetEntriesFast();
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
