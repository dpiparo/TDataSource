#include "ROOT/TRootTDS.hxx"
#include "ROOT/TSeq.hxx"

#include "stdio.h"

int main()
{
   TRootTDS tds("t", "data/data.root");

   auto &colNames = tds.GetColumnNames();

   std::cout << "Column names:\n";
   for (auto &&name : colNames) {
      std::cout << " - " << name << std::endl;
   }

   for (auto &&colName : {"sittingBull", "i", "d"}) {
      std::cout << "Has column \"" << colName << "\" ? " << tds.HasColumn(colName) << std::endl;
   }

   for (auto &&name : colNames) {
      std::cout << "Typename of " << name << "  is " << tds.GetTypeName(name) << std::endl;
   }

   gInterpreter->AddIncludePath("./inc");
   gInterpreter->ProcessLine("#include \"ROOT/TRootTDS.hxx\"");
   for (auto &&colName : colNames) {
   	  auto colTypeName = tds.GetTypeName(colName);
   	  auto code = TString::Format("dynamic_cast<TRootTDS*>(((TDataSource*)%p))->template GetColumnReaders<%s>(\"%s\", 0)", &tds, colTypeName.c_str(), colName.c_str());
   	  std::cout << "Jitting and calling: \"" << code.Data() << "\"\n";
      gInterpreter->ProcessLine(code);
   }

   auto& ranges = tds.GetEntryRanges();

   auto slot = 0U;
   for (auto &&range : ranges) {
      printf("Chunk %u , Entry Range %llu -  %llu\n", slot, range.first, range.second);
      slot++;
   }
   /*
      auto vals = tds.GetColumnReaders<ULong64_t>("col0", slot);
      std::vector<std::thread> pool;
      slot = 0U;
      for (auto&& range : ranges) {
         pool.emplace_back([slot, &range, &tds, &vals]() {
            for (auto i : ROOT::TSeq<ULong64_t>(range.first, range.second)) {
               tds.SetEntry(i, slot);
               printf("Value of col0 for entry %llu is %llu\n", i, **vals[slot]);
            }
         });
         slot++;
      }
      for (auto&& t : pool) t.join();
   */
}
