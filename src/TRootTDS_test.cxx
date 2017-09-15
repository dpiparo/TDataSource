#include "ROOT/TDataFrame.hxx"
#include "ROOT/TRootTDS.hxx"
#include "ROOT/TSeq.hxx"

#include "TGraph.h"

#include "stdio.h"

void testSource()
{
  TRootTDS tds("t", "data/data.root");

  const auto nSlots = 4;

  auto &colNames = tds.GetColumnNames();
  tds.SetNSlots(nSlots);

  std::cout << "Column names:\n";
  for (auto &&name : colNames) {
    std::cout << " - " << name << std::endl;
  }

  for (auto &&colName : {"sittingBull", "i", "d", "graph"}) {
    std::cout << "Has column \"" << colName << "\" ? " << tds.HasColumn(colName)
              << std::endl;
  }

  for (auto &&name : colNames) {
    std::cout << "Typename of " << name << "  is " << tds.GetTypeName(name)
              << std::endl;
  }

   unsigned int slot = 0;
   tds.InitSlot(slot, 0);

  auto vals_g = tds.GetColumnReaders<TGraph>("graph");
  auto vals_i = tds.GetColumnReaders<int>("i");

  auto &ranges = tds.GetEntryRanges();


   for (auto iEvent : {0, 1, 2,3 ,4,5,6,7,8,9}) {
      tds.SetEntry(iEvent, slot);
      auto g = (*((TGraph**)vals_g[slot]));
      std::cout << "TGraph Npoints = " << g->GetN() << std::endl;

      auto i = (*((int**)vals_i[slot]));
      std::cout << "i = " << *i << std::endl;

      g->Draw();
   }
}

int main() {

   testSource();

   std::cout << "-------- End testing the source...\n";

   std::unique_ptr<ROOT::Experimental::TDF::TDataSource> tds(new TRootTDS("t", "data/data.root"));
   ROOT::Experimental::TDataFrame tdf(std::move(tds));
   auto c = tdf.Count();
   std::cout << "The TDF with TDS had " << *c << " events\n";// and the max of column i is " << m->GetMean() << "\n";
}
