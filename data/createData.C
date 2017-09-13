void createData()
{
   ROOT::Experimental::TDataFrame tdf(10);
   int i=0;
   double d=0.;
   tdf.Define("i", [&i](){return i++;})
      .Define("d", [&d](){return d++;})
      .Snapshot("t","data.root");
}
