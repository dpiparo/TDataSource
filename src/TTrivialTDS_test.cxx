#include "ROOT/TTrivialTDS.hxx"
#include "ROOT/TSeq.hxx"

#include "stdio.h"

int main()
{

   TTrivialTDS tds(32, 2);

   for (auto&& name : tds.GetColumnNames()) {
      std::cout << name << std::endl;
   }

   for (auto&& colName : {"test", "col0"}) {
      std::cout << "Has column \"" << colName << "\" ? " << tds.HasColumn(colName) << std::endl;
   }

   std::cout << "Typename of col0  is " << tds.GetTypeName("col0") << std::endl;

   auto ranges = tds.GetEntryRanges();

   auto slot = 0U;
   for (auto&& range : ranges) {
      printf("Chunk %u , Entry Range %llu -  %llu\n", slot, range.first, range.second);
      slot++;
   }

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

}
