#include "ROOT/TTrivialTDS.hxx"
#include "ROOT/TSeq.hxx"

#include "stdio.h"

void testSource()
{

   ROOT::EnableThreadSafety();

   TTrivialTDS tds(32);
   const auto nSlots = 4U;
   tds.SetNSlots(nSlots);

   for (auto &&name : tds.GetColumnNames()) {
      std::cout << name << std::endl;
   }

   for (auto &&colName : {"test", "col0"}) {
      std::cout << "Has column \"" << colName << "\" ? " << tds.HasColumn(colName) << std::endl;
   }

   std::cout << "Typename of col0  is " << tds.GetTypeName("col0") << std::endl;

   auto ranges = tds.GetEntryRanges();

   auto slot = 0U;
   for (auto &&range : ranges) {
      printf("Chunk %u , Entry Range %llu -  %llu\n", slot, range.first, range.second);
      slot++;
   }

   auto vals = tds.GetColumnReaders<ULong64_t>("col0");
   std::vector<std::thread> pool;
   slot = 0U;
   for (auto &&range : ranges) {
      auto work = [slot, range, &tds, &vals]() {
         for (auto i : ROOT::TSeq<ULong64_t>(range.first, range.second)) {
            tds.SetEntry(i, slot);
            printf("Value of col0 for entry %llu is %llu\n", i, **vals[slot]);
         }
      };
      pool.emplace_back(work);
      slot++;
   }

   for (auto &&t : pool) t.join();
}

int main()
{
   testSource();
   std::cout << "End TTrivialTDS Units...\n";

   std::unique_ptr<ROOT::Experimental::TDF::TDataSource> tds(new TTrivialTDS(32));
   tds->SetNSlots(1);
   ROOT::Experimental::TDataFrame tdf(std::move(tds));
   auto m = tdf.Max<ULong64_t>("col0");
   auto c = tdf.Count();
   std::cout << "The TDF with TDS has " << *c << " events and the max of col0 is " << *m << std::endl;
}
