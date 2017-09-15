void readExample() {
  TChain c("t");
  c.Add("data/data.root");
  void* i = new double();
  auto d = new double();

//   auto g = new TGraph();
  void* g(nullptr);

  c.SetBranchAddress("i", i);
  c.SetBranchAddress("d", d);
  c.SetBranchAddress("graph", &g);
  for (int iEvent=0;iEvent < c.GetEntriesFast(); ++iEvent) {
     c.GetEntry(iEvent);
     std::cout << "i " << *(int*)i << std::endl;
     std::cout << "d " << *d << std::endl;
     auto gg = (TGraph*)g;
     std::cout << "g " <<  gg << " " << gg->GetN() << std::endl;
   }

}
