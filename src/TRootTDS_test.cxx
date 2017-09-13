#include "ROOT/TRootTDS.hxx"
#include "ROOT/TSeq.hxx"

#include "TGraph.h"

#include "stdio.h"

int main() {
  TRootTDS tds("t", "data/data.root");

  const auto nSlots = 4;

  auto &colNames = tds.GetColumnNames();

  std::cout << "Column names:\n";
  for (auto &&name : colNames) {
    std::cout << " - " << name << std::endl;
  }

  for (auto &&colName : {"sittingBull", "i", "d", "g"}) {
    std::cout << "Has column \"" << colName << "\" ? " << tds.HasColumn(colName)
              << std::endl;
  }

  for (auto &&name : colNames) {
    std::cout << "Typename of " << name << "  is " << tds.GetTypeName(name)
              << std::endl;
  }

  //   gInterpreter->AddIncludePath("./inc");
  //   gInterpreter->ProcessLine("#include \"ROOT/TRootTDS.hxx\"");
  //   for (auto &&colName : colNames) {
  //     auto colTypeName = tds.GetTypeName(colName);
  //     auto code = TString::Format(
  //         "((TRootTDS*)%p)->template GetColumnReaders<%s>(\"%s\", %u)", &tds,
  //         colTypeName.c_str(), colName.c_str(), nSlots);
  //     std::cout << "Jitting and calling: \"" << code.Data() << "\"\n";
  //     gInterpreter->ProcessLine(code);
  //   }

//   auto vals_i = tds.template GetColumnReaders<int>("i", nSlots);
//   auto vals_d = tds.template GetColumnReaders<double>("d", nSlots);
  auto vals_g = tds.template GetColumnReaders<TGraph>("graph", nSlots);

  auto &ranges = tds.GetEntryRanges();

  auto slot = 0U;
  for (auto &&range : ranges) {
    printf("Chunk %u , Entry Range %llu -  %llu\n", slot, range.first,
           range.second);
    slot++;
  }


   slot = 0;

   tds.InitSlot(slot);
   for (int i : {0, 1}) {
      tds.SetEntry(i, slot);
      auto g = (*((TGraph**)vals_g[slot]));
      std::cout << "TGraph Npoints = " << g->GetN() << std::endl;
      g->Draw();
   }

//    tds.InitSlot(1);
//    for (auto &&v : vals_g) {
//       for (auto i : {1,2}) {
//          tds.SetEntry(1, 1);
//          std::cout <<  ((TGraph*)vals_g[1])->GetN() << std::endl;
//       }
//    }

//   std::vector<std::thread> pool;
//   slot = 0U;
//   for (auto &&range : ranges) {
//     //      auto f = [&]() {
//     tds.InitSlot(slot);
//     for (auto &&v : vals_i) std::cout << vals_i[0] << std::endl;
//     //     for (auto i : ROOT::TSeq<ULong64_t>(range.first, range.second)) {
//     //       tds.SetEntry(i, slot);
//     //       printf("Value of \"i\" for entry %llu is %d\n", i, **vals_i[slot]);
//     //       printf("Value of \"d\" for entry %llu is %f\n", i, **vals_d[slot]);
//   }
//   //     };
//   //     f();
//   slot++;
// }
//
// for (auto &&t : pool)
//   t.join();
}
