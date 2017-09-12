#ifndef ROOT_TDataSource
#define ROOT_TDataSource

#include "ROOT/TDataFrame.hxx"
#include <typeinfo>

using ROOT::Detail::TDF::ColumnNames_t;

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
  virtual const std::vector<std::pair<ULong64_t, ULong64_t>> &GetEntryRanges() const = 0;
  virtual void SetEntry(ULong64_t entry, unsigned slot) = 0;
};

inline TDataSource::~TDataSource() { }

#endif
