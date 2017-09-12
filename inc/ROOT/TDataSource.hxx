#ifndef ROOT_TDataSource
#define ROOT_TDataSource

#include "ROOT/TDataFrame.hxx"
#include <typeinfo>

using ROOT::Detail::TDF::ColumnNames_t;

class TDataSource {
protected:
  virtual void **GetColumnReaderImpl(const std::type_info &,
                                     unsigned int slot) = 0;

public:
  virtual ~TDataSource() = 0;
  virtual const ColumnNames_t &GetColumnNames() const = 0;
  virtual bool HasColumn(std::string_view) const = 0;
  virtual std::string GetTypeName(std::string_view) const = 0;
  template <typename T>
  const T **GetColumnReader(std::string_view, unsigned int slot) {
    return (T **)GetColumnReaderImpl(typeid(T), slot);
  }
  virtual const std::vector<std::pair<ULong64_t, ULong64_t>> &GetEntryRanges() const = 0;
  virtual void SetEntry(ULong64_t entry, unsigned slot) = 0;
};

inline TDataSource::~TDataSource() { }

#endif
