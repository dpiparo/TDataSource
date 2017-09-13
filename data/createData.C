void createData()
{
   ROOT::Experimental::TDataFrame tdf(10);
   int i=0;
   double d=0.;
   TGraph g;
   tdf.Define("i", [&i](){return i++;})
      .Define("d", [&d](){return d++;})
      .Define("graph", [&g, &i](){g.SetPoint(i,i,i);return g;})
      .Snapshot("t","data.root");
}
