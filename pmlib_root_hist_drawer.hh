///////////////////////////////////////////////////////////////////////////////
//  Author      :     P.S. Mandrik, IHEP
//  Date        :     01/03/20
//  Last Update :     11/05/20
//  Version     :     1.1
///////////////////////////////////////////////////////////////////////////// 

#ifndef PMLIB_ROOT_HIST_DRAWER_HH
#define PMLIB_ROOT_HIST_DRAWER_HH 1

namespace pm {

  void set_minmax_from_hist(double & min, double & max, TH1* hist){
    if( min > hist->GetMinimum() ) min = hist->GetMinimum();
    if( max < hist->GetMaximum() ) max = hist->GetMaximum();
  }

  
  // mode == "" - just central and other histograms
  // mode == "diff" - draw differents between central and other histograms
  // mode == "diff percent" - draw differents between central and other histograms in percents
  void draw_hists_difference(TH1* central, const std::vector<TH1*> & others, std::string mode="", float range_factor=1.2){
    TH1* cen = (TH1D*)central->Clone();
    cen->SetStats(0);
    if(mode == "diff percent" or mode == "diff"){
      cen->Add(central, -1);
    }
    cen->Draw("hist");
    double min = cen->GetMinimum();
    double max = cen->GetMaximum();
    if(mode == "percent"){
      min = 0;
      max = 0;
    }

    std::vector<TH1*> all_hre_hists;
    for(auto hist : others){
      TH1 * hre = (TH1*)hist->Clone();
      if(mode == "diff percent" or mode == "diff"){
        hre->Add( central, -1. );
      }
      if(mode == "diff percent"){
        for(int j = 1; j <= central->GetNbinsX(); j++ ){
          float central_bin_value = central->GetBinContent(j);
          if(central_bin_value > 0.00000000001){
            hre->SetBinContent(j, 100. * hre->GetBinContent(j) / central_bin_value );
          }
          else hre->SetBinContent(j, 100. * hre->GetBinContent(j) / TMath::Abs( hre->GetBinContent(j) ) );
        }
      }
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


  // draw shade graph with central value and contours
  TGraph * draw_brasil(vector<float> & xs, vector<float> & pl2, vector<float> & pl1, vector<float> & pc, vector<float> & pu1, vector<float> & pu2){
    int n_size = pc.size();
    TGraph * gc  = new TGraph( n_size );
    TGraph * gs1 = new TGraph( 2*n_size );
    TGraph * gs2 = new TGraph( 2*n_size );
    
    for(int i = 0; i < n_size; i++){
      gc->SetPoint(  i,            xs.at(i), pc.at(i) );
      
      gs1->SetPoint( i,            xs.at(i), pl1.at(i) );
      gs1->SetPoint( 2*n_size-1-i, xs.at(i), pu1.at(i) );
      
      gs2->SetPoint( i,            xs.at(i), pl2.at(i) );
      gs2->SetPoint( 2*n_size-1-i, xs.at(i), pu2.at(i) );
    }
    
    gs2->SetFillColor(kYellow);
    gs2->Draw("AF");

    gs1->SetFillColor(kGreen+1);
    gs1->Draw("F");

    gc->SetLineStyle(2);
    gc->SetLineWidth(2);
    gc->Draw("L");
    
    return gs2;
  } 
  
  
  
  
  
  
  
  
  
  
};

#endif
