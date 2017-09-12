#include "ROOT/TTrivialTDS.hxx"
#include "ROOT/TSeq.hxx"

#include "stdio.h"

int main()
{

   TTrivialTDS tds(512, 128);

   for (auto&& name : tds.GetColumnNames()) {
      std::cout << name << std::endl;
   }

   for (auto&& colName : {"test", "col0"}) {
      std::cout << "Has column \"" << colName << "\" ? " << tds.HasColumn(colName) << std::endl;
   }

   std::cout << "Typename of col0  is " << tds.GetTypeName("col0") << std::endl;

   auto ranges = tds.GetEntryRanges();

   std::vector<std::thread> pool;
   auto slot = 0U;
   for (auto&& range : ranges) {
     pool.emplace_back([slot, &range](){
        for (auto i : ROOT::TSeqUL(range.first, range.second))
         printf("Slot %u , Entry %lu\n", slot, i);
        });
     slot++;
   }

   for (auto&& t : pool) t.join();

}
