#include "ROOT/TDataFrame.hxx"
#include "ROOT/TRootTDS.hxx"
#include "ROOT/TSeq.hxx"

#include "TGraph.h"

#include "stdio.h"

void testSource()
{
  TRootTDS tds("t", "data/data.root");

  const auto nSlots = 4;

//   auto &colNames = tds.GetColumnNames();

//   std::cout << "Column names:\n";
//   for (auto &&name : colNames) {
//     std::cout << " - " << name << std::endl;
//   }
//
//   for (auto &&colName : {"sittingBull", "i", "d", "g"}) {
//     std::cout << "Has column \"" << colName << "\" ? " << tds.HasColumn(colName)
//               << std::endl;
//   }
//
//   for (auto &&name : colNames) {
//     std::cout << "Typename of " << name << "  is " << tds.GetTypeName(name)
//               << std::endl;
//   }

  auto vals_g = tds.template GetColumnReaders<TGraph>("graph", nSlots);

  auto &ranges = tds.GetEntryRanges();

//   auto slot = 0U;
//   for (auto &&range : ranges) {
//     printf("Chunk %u , Entry Range %llu -  %llu\n", slot, range.first,
//            range.second);
//     slot++;
//   }

   unsigned int slot = 0;

   tds.InitSlot(slot, 0);
   for (int i : {0, 1, 2,3 ,4,5,6,7,8,9}) {
      tds.SetEntry(i, slot);
      auto g = (*((TGraph**)vals_g[slot]));
      std::cout << "TGraph Npoints = " << g->GetN() << std::endl;
      g->Draw();
   }
}

int main() {

   testSource();

   std::cout << "-------- End testing the source...\n";

   std::unique_ptr<ROOT::Experimental::TDF::TDataSource> tds(new TRootTDS("t", "data/data.root"));
   tds.template GetColumnReaders<TGraph>("graph", 0);
   ROOT::Experimental::TDataFrame tdf(std::move(tds));
   auto c = tdf.Count();
   std::cout << "The TDF with TDS had " << *c << " events\n";
}
