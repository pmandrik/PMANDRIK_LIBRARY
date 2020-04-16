///////////////////////////////////////////////////////////////////////////////
//  Author      :     P.S. Mandrik, IHEP
//  Date        :     01/03/20
//  Last Update :     01/03/20
//  Version     :     1.0
///////////////////////////////////////////////////////////////////////////// 

#ifndef PMLIB_ROOT_HIST_DRAWER_HH
#define PMLIB_ROOT_HIST_DRAWER_HH 1

namespace pm {

  void set_minmax_from_hist(double & min, double & max, TH1* hist){
    if( min > hist->GetMinimum() ) min = hist->GetMinimum();
    if( max < hist->GetMaximum() ) max = hist->GetMaximum();
  }

  void draw_hists_difference(TH1* central, const std::vector<TH1*> & others, float range_factor=1.2){
    TH1* cen = (TH1D*)central->Clone();
    cen->SetStats(0);
    cen->Add(central, -1);
    cen->Draw("hist");
    double min = cen->GetMinimum();
    double max = cen->GetMaximum();

    std::vector<TH1*> all_hre_hists;
    for(auto hist : others){
      TH1 * hre = (TH1*)hist->Clone();
      hre->Add( central, -1. );
      hre->Draw("hist same");
      set_minmax_from_hist(min, max, hre);
      all_hre_hists.push_back( hre );
    }

    if(max < 0) max /= range_factor;
    else        max *= range_factor;
    if(min > 0) min /= range_factor;
    else        min *= range_factor;
    double size_y = TMath::Abs( max - min );

    cen->GetYaxis()->SetRangeUser(min, max);
    cen->Draw("hist same");

    // draw hist labels at the canvas
    vector< int > reserved_bins;
    vector< float > reserved_positions;
    for(int i = 0; i < all_hre_hists.size(); i++){
      auto hre = all_hre_hists.at(i);

      vector< pair<float, int> > bins_values;
      for(int j = 1; j <= hre->GetNbinsX(); j++ )
        bins_values.push_back( make_pair(TMath::Abs( hre->GetBinContent(j) ), j ) );
      sort(bins_values.begin(), bins_values.end() );

      int bin = bins_values.at(0).second;
      int bin_to_search = bin;
      for(int j = bins_values.size()-1; j >= 0; j--){
        bin_to_search = bins_values.at( j ).second;
        bool change_position = false;
        for(int j = 0; j < reserved_bins.size(); j++){
          if( reserved_bins[j] != bin_to_search ) continue;
          float distance = TMath::Abs( reserved_positions[j] - 1.1*hre->GetBinContent( bin_to_search ) ) / size_y;
          if( distance > 0.05) continue;
          change_position = true;
          break;
        }
        if( not change_position ) break;
      }
      bin = bin_to_search;

      // int bin = 1 + i * TMath::Max( 1, int((0.75 * hre->GetNbinsX()) / all_hre_hists.size()) ) ;
      // bin = bin % hre->GetNbinsX();
      
      string title = string( hre->GetTitle() );

      double posx = hre->GetBinCenter( bin );
      double posy = 1.1 * hre->GetBinContent( bin );

      reserved_bins.push_back( bin );
      reserved_positions.push_back( posy );

      auto *tt = new TText(posx, posy, title.c_str() );
      tt->SetTextSize(0.025);
      tt->SetTextColor( hre->GetLineColor() );
      if( bin > 0.75 * hre->GetNbinsX() ) tt->SetTextAlign(31);
      tt->Draw();
    }
  }


};

#endif
