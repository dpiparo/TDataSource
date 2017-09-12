#ifndef ROOT_TROOTTDS
#define ROOT_TROOTTDS

#include "ROOT/TDataSource.hxx"

#include "ROOT/TSeq.hxx"

#include <deque>
#include <vector>

/*
class TDataSource {
protected:
  virtual const void *GetColumnReaderImpl(std::string_view name,
                                          unsigned int slot,
                                          const std::type_info &ti) = 0;

public:
  virtual ~TDataSource() = 0;
  virtual const ColumnNames_t &GetColumnNames() const = 0;
  virtual bool HasColumn(std::string_view) const = 0;
  virtual std::string GetTypeName(std::string_view) const = 0;
  void InitSlot(unsigned int) {} ;

  // Here the first vector is indexed with slots while the second with columns
  template<typename T>
  std::vector<const T*const*> GetColumnReaders(std::string_view name, unsigned int nSlots) {
    std::vector<const T*const*> readers(nSlots);
    for(auto slot = 0u; slot < nSlots; ++slot)
      readers[slot] = static_cast<const T*const*>(GetColumnReaderImpl(name, slot, typeid(T)));
    return readers;
  }

};
*/

class TRootTDS : public TDataSource {
private:
   std::string fTreeName;
   std::string fFileNameGlob;
   std::vector<std::string> fRequiredColumns;
   std::deque<std::deque<void*>> fBranchAddresses; // first container-> slot, second -> column;

   std::vector<std::unique_ptr<TChain>> fChains;
   const void *GetColumnReaderImpl(std::string_view name,
                                   unsigned int slot,
                                   const std::type_info &ti) {

   }
public:
   TRootTDS(std::string_view treeName, std::string_view fileNameGlob) : fTreeName(treeName), fFileNameGlob(fileNameGlob) {

   }

   void InitSlot(unsigned int slot) {
      if (fBranchAddresses.size() <= slot) {
         fBranchAddresses.resize(1U + slot);
      }
      fChains[i].reset(new TChain(fTreeName.c_str()));
      fChains[i]->Add(fFileNameGlob.c_str());
      const auto nRequiredColumns = fRequiredColumns.size();
      auto& theseBranchAddresses = fBranchAddresses[slot];
      theseBranchAddresses.resize(nRequiredColumns);
      for (size_t i = 0; i< nRequiredColumns ++i) {
         void* addr(nullptr);
         theseBranchAddresses.emplace_back(addr);
         fChains[i]->SetBranchAddress(fRequiredColumns[i], theseBranchAddresses[i]);
      }

   }

   const std::vector<std::pair<ULong64_t, ULong64_t>> &GetEntryRanges() const {
      // Quick implementation: divide in equal parts.
      // We need to respect the clusters to be fair.
      auto nentries = fChains[0]->GetEntriesFast();
      // XXX Here we need to have the number of slots ...XXX

   }

   void SetEntry(ULong64_t entry, unsigned slot) {
      fChains[i]->SetEntry(entry);
   }
};

#endif

