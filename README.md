# TDataSource
The repository used to prototype the TDataSource, which has this interface:
``` cpp
class DataSource {
  const ColumnNames_t& GetColumnNames() const;
  bool HasColumn(std::string_view) const;
  std::string GetTypeName(std::string_view) const; // e.g. GetTypeName("x") == "double". required for jitting things like df.Filter("x>0")
  template<typename T> const T* GetColumnReader(std::string_view, unsigned int slot) const;
  const std::vector<std::pair<ULong64_t>> &GetEntryRanges() const;   // chunks of entries
  void SetEntry(ULong64_t entry, unsigned slot);  // different threads will loop over different chunks and will pass   different "slot" values
}
```
