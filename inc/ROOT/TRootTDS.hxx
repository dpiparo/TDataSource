#ifndef ROOT_TROOTTDS
#define ROOT_TROOTTDS

#include "ROOT/TDataFrame.hxx"
#include "ROOT/TDataSource.hxx"

#include "ROOT/TSeq.hxx"
#include "TChain.h"
#include "TClass.h"

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
  std::vector<std::vector<void *>>
      fBranchAddresses; // first container-> slot, second -> column;

  std::vector<std::unique_ptr<TChain>> fChains;

  void InitAddresses() {}

  std::vector<void *> GetColumnReadersImpl(std::string_view name,
                                           const std::type_info &) {

    const auto &colNames = GetColumnNames();

    if (fBranchAddresses.empty()) {
      auto nColumns = colNames.size();
      // Initialise the entire set of addresses
      fBranchAddresses.resize(nColumns, std::vector<void *>(fNSlots));
    }

    const auto index = std::distance(
        colNames.begin(), std::find(colNames.begin(), colNames.end(), name));
    std::vector<void *> ret(fNSlots);
    for (auto slot : ROOT::TSeqU(fNSlots)) {
      ret[slot] = (void *)&fBranchAddresses[index][slot];
    }
    return ret;
  }

  // This is not even a method...
  bool IsClass(const std::string &typeName) {
    return nullptr != TClass::GetClass(typeName.c_str());
  }

public:
  TRootTDS(std::string_view treeName, std::string_view fileNameGlob)
      : fTreeName(treeName), fFileNameGlob(fileNameGlob),
        fModelChain(std::string(treeName).c_str()) {
    fModelChain.Add(fFileNameGlob.c_str());
  }

  ~TRootTDS(){};

  std::string GetTypeName(std::string_view colName) const {
    if (!HasColumn(colName)) {
      std::string e = "The dataset does not have column ";
      e += colName;
      throw std::runtime_error(e);
    }
    // TODO: we need to factor out the routine for the branch alone...
    // Maybe a cache for the names?
    auto typeName = ROOT::Internal::TDF::ColumnName2ColumnTypeName(
        std::string(colName).c_str(), &fModelChain,
        nullptr /*TCustomColumnBase here*/);
    // We may not have yet loaded the library where the dictionary of this type
    // is
    TClass::GetClass(typeName.c_str());
    return typeName;
  }

  const ColumnNames_t &GetColumnNames() const {
    if (!fListOfBranches.empty())
      return fListOfBranches;
    auto &lob = *fModelChain.GetListOfBranches();
    fListOfBranches.resize(lob.GetEntries());
    std::transform(lob.begin(), lob.end(), fListOfBranches.begin(),
                   [](TObject *o) { return o->GetName(); });
    return fListOfBranches;
  }

  bool HasColumn(std::string_view colName) const {
    if (!fListOfBranches.empty())
      GetColumnNames();
    return fListOfBranches.end() !=
           std::find(fListOfBranches.begin(), fListOfBranches.end(), colName);
  }

  void InitSlot(unsigned int slot, ULong64_t firstEntry) {

    if (fChains.empty()) {
      fChains.resize(fNSlots);
    }

    if (fChains[slot]) {
      return;
    }

    auto chain = new TChain(fTreeName.c_str());
    fChains[slot].reset(chain);
    chain->Add(fFileNameGlob.c_str());
    chain->GetEntry(firstEntry);
    auto &theseBranchAddresses = fBranchAddresses[slot];
    for (auto i : ROOT::TSeqU(fListOfBranches.size())) {
      auto colName = fListOfBranches[i].c_str();
      auto &addr = fBranchAddresses[i][slot];
      if (IsClass(GetTypeName(colName))) {
        chain->SetBranchAddress(colName, &addr);
      } else {
        if (!addr) {
          addr = new double(); // who frees this :) ?
        }
        chain->SetBranchAddress(colName, addr);
      }
    }
  }

  const std::vector<std::pair<ULong64_t, ULong64_t>> &GetEntryRanges() const {

    // Quick implementation: divide in equal parts.
    // We need to respect the clusters to be fair.
    if (!fEntryRanges.empty())
      return fEntryRanges;
    auto nentries = fModelChain.GetEntries();
    auto chunkSize = nentries / fNSlots;
    auto reminder = 1U == fNSlots ? 0 : nentries % fNSlots;
    auto start = 0UL;
    auto end = 0UL;

    std::cout << "- Number of events: " << nentries << std::endl
              << "- chunkSize: " << chunkSize << std::endl
              << "- reminder: " << reminder << std::endl;
    for (auto i : ROOT::TSeqU(fNSlots)) {
      start = end;
      end += chunkSize;
      fEntryRanges.emplace_back(start, end);
    }
    fEntryRanges.back().second += reminder;

    auto slot = 0U;
    for (auto &&range : fEntryRanges) {
      printf("Chunk %u , Entry Range %llu -  %llu\n", slot, range.first,
             range.second);
      slot++;
    }

    return fEntryRanges;
  }

  void SetEntry(ULong64_t entry, unsigned slot) {
    std::cout << "Setting entry to " << entry << std::endl;
    fChains[slot]->GetEntry(entry);
  }

  void SetNSlots(ULong64_t nSlots) {
    std::cout << "SETTING NSLOTS TO " << nSlots << "\n";
    ;
    fNSlots = nSlots;
  }
};

#endif
