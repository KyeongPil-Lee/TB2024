#include "TBplotengine.h"
#include "GuiTypes.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TPaveStats.h"

TBplotengine::TBplotengine(const YAML::Node fConfig_, int fRunNum_, bool fLive_, TButility fUtility_)
: fConfig(fConfig_), fRunNum(fRunNum_), fLive(fLive_), fUtility(fUtility_), fCaseName("")
{}

void TBplotengine::init() {

  // fUtility.LoadMapping("../mapping/mapping_TB2024_v1.root");
  fIsFirst = true;

  if (fCaseName == "single") {

    // if (fCalcInfo == TBplotengine::CalcInfo::kIntADC || fCalcInfo == TBplotengine::CalcInfo::kPeakADC) {
    //   if (fCIDtoPlot_Ceren.size() > 5)
    //     fLeg = new TLegend(0.7, 0.2, 0.9, 0.5);

    if (fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc) {
      fLeg = new TLegend(0.7, 0.2, 0.9, 0.5);
     	fLeg->SetFillStyle(0);
     	fLeg->SetBorderSize(0);
     	fLeg->SetTextFont(42);
    }

    gStyle->SetPalette(kVisibleSpectrum);

    for (int i = 0; i < fNametoPlot.size(); i++) {
      // std::string aName = fUtility.GetName(aCID);
      std::string aName = fNametoPlot.at(i);
      TBcid aCID = fUtility.GetCID(aName);
      fCIDtoPlot_Ceren.push_back(aCID);

      TButility::mod_info aInfo = fUtility.GetInfo(aCID);
      // std::cout << aName << " "; aCID.print();

      if (fCalcInfo == TBplotengine::CalcInfo::kIntADC || fCalcInfo == TBplotengine::CalcInfo::kPeakADC) {
        std::vector<int> interval = fConfig[aName].as<std::vector<int>>();
        fPlotter_Ceren.push_back(TBplotengine::PlotInfo(aCID, aName, aInfo, interval.at(0), interval.at(1)));

        if (fCalcInfo == TBplotengine::CalcInfo::kIntADC)
          fPlotter_Ceren.at(i).SetPlot(new TH1D((TString)(aName), ";IntADC;nEvents", 440, -30000., 300000.));

        if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC)
          fPlotter_Ceren.at(i).SetPlot(new TH1D((TString)(aName), ";IntADC;nEvents", 288, -512., 4096.));

        fPlotter_Ceren.at(i).hist1D->SetLineColor(
          gStyle->GetColorPalette((float)(i + 1) * ((float)gStyle->GetNumberOfColors() / ((float)fNametoPlot.size() + 1)))
        );
        fPlotter_Ceren.at(i).hist1D->SetLineWidth(2);

      } else if (fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc) {
        fPlotter_Ceren.push_back(TBplotengine::PlotInfo(aCID, aName, aInfo, 0, 0));
        fPlotter_Ceren.at(i).SetPlot(new TH1D((TString)(aName), ";Bin;ADC", 1000, 0.5, 1000.5));
        fPlotter_Ceren.at(i).hist1D->SetLineColor(
          gStyle->GetColorPalette((float)(i + 1) * ((float)gStyle->GetNumberOfColors() / ((float)fNametoPlot.size() + 1)))
        );
        fPlotter_Ceren.at(i).hist1D->SetLineWidth(2);
        fPlotter_Ceren.at(i).hist1D->SetStats(0);

      } else if (fCalcInfo == TBplotengine::CalcInfo::kOverlay) {
        fPlotter_Ceren.push_back(TBplotengine::PlotInfo(aCID, aName, aInfo, 0, 0));
        fPlotter_Ceren.at(i).SetPlot(new TH2D((TString)(aName), ";Bin;ADC", 1024, 0., 1024., 4096, 0., 4096.));
        fPlotter_Ceren.at(i).hist2D->SetStats(0);

      } else {
        fPlotter_Ceren.push_back(TBplotengine::PlotInfo(aCID, aName, aInfo));
      }
    }

    int argc = 0;
    char* argv[] = {};
    fApp = new TApplication("app", &argc, argv);
    if (fLive)
      fApp->SetReturnFromRun(true);

    if (fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc) {
      fMainFrame = new TH1D("frame", ";Bin;ADC", 1000, 0.5, 1000.5);
      fMainFrame->SetStats(0);
    }

    if (fCalcInfo == TBplotengine::CalcInfo::kIntADC) {
      fMainFrame = new TH1D("frame", ";IntADC;nEvents", 440, -30000., 300000.);
      fMainFrame->SetStats(0);
    }

    if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC) {
      fMainFrame = new TH1D("frame", ";PeakADC;nEvents", 288, -512., 4096.);
      fMainFrame->SetStats(0);
    }

    fCanvas = new TCanvas("", "", 1500, 1000);

    Draw();
  } else if (fCaseName == "heatmap") {

    int argc = 0;
    char* argv[] = {};
    fApp = new TApplication("app", &argc, argv);

    if (fLive)
      fApp->SetReturnFromRun(true);

    fCanvas = new TCanvas("", "", 1900, 1000);
    fCanvas->Divide(2, 1);

    auto tPadLeft = fCanvas->cd(1);
    tPadLeft->SetRightMargin(0.13);

    auto tPadRight = fCanvas->cd(2);
    tPadRight->SetRightMargin(0.13);

    init_2D();
  } else if (fCaseName == "module") {

    int argc = 0;
    char* argv[] = {};
    fApp = new TApplication("app", &argc, argv);

    if (fLive)
      fApp->SetReturnFromRun(true);

    fCanvas = new TCanvas("", "", 1000, 1000);
    if (fModule == "M11") fCanvas->Divide(3, 3);
    else                  fCanvas->Divide(2, 2);

    // std::cout << "fModule: " << fModule << std::endl;

    init_single_module();
  }

  // std::cout << fCIDtoPlot_Ceren.size() << std::endl;
  // for (int i = 0; i < fPlotter_Ceren.size(); i++) {
  //   std::cout << i << " " << fPlotter_Ceren.at(i).name << " " << fPlotter_Ceren.at(i).info.row << " " << fPlotter_Ceren.at(i).info.col << " ";
  //   fPlotter_Ceren.at(i).cid.print();
  // }

  // std::cout << fCIDtoPlot_Scint.size() << std::endl;
  // for (int i = 0; i < fPlotter_Scint.size(); i++) {
  //   std::cout << i << " " << fPlotter_Scint.at(i).name << " " << fPlotter_Scint.at(i).info.row << " " << fPlotter_Scint.at(i).info.col << " ";
  //   fPlotter_Scint.at(i).cid.print();
  // }
}

void TBplotengine::init_single_module() {

  // std::cout << "TBplotengine::init_single_module()" << " " << fModule << std::endl;

  if (fModule != "M11") {
    for (int i = 1; i <= 4; i++) {
      std::string aCName = fModule + "-T" + std::to_string(i) + "-C";
      TBcid aCCID = fUtility.GetCID(aCName);
      TButility::mod_info aCInfo = fUtility.GetInfo(aCName);

      fCIDtoPlot_Ceren.push_back(aCCID);
      std::vector<int> aCinterval = fConfig[aCName].as<std::vector<int>>();
      fPlotter_Ceren.push_back(TBplotengine::PlotInfo(aCCID, aCName, aCInfo, aCinterval.at(0), aCinterval.at(1)));

      if (fCalcInfo == TBplotengine::CalcInfo::kIntADC)
        fPlotter_Ceren.at(i - 1).SetPlot(new TH1D((TString)(aCName), ";IntADC;nEvents", 440, -30000., 300000.));

      if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC)
        fPlotter_Ceren.at(i - 1).SetPlot(new TH1D((TString)(aCName), ";PeakADC;nEvents", 288, -512., 4096.));

      std::string aSName = fModule + "-T" + std::to_string(i) + "-S";
      TBcid aSCID = fUtility.GetCID(aSName);
      TButility::mod_info aSInfo = fUtility.GetInfo(aSName);

      fCIDtoPlot_Scint.push_back(aSCID);
      std::vector<int> aSinterval = fConfig[aSName].as<std::vector<int>>();
      fPlotter_Scint.push_back(TBplotengine::PlotInfo(aSCID, aSName, aSInfo, aSinterval.at(0), aSinterval.at(1)));

      if (fCalcInfo == TBplotengine::CalcInfo::kIntADC)
        fPlotter_Scint.at(i - 1).SetPlot(new TH1D((TString)(aSName), ";IntADC;nEvents", 440, -30000., 300000.));

      if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC)
        fPlotter_Scint.at(i - 1).SetPlot(new TH1D((TString)(aSName), ";PeakADC;nEvents", 288, -512., 4096.));
    }
  } else {
    for (int i = 1; i <= 9; i++) {
      if (i == 5 ) {

        fPlotter_Ceren.push_back(TBplotengine::PlotInfo(TBcid(-1, -1), "M11-T5-C", TButility::mod_info(-1, -1, -1), -1, -1));
        fPlotter_Scint.push_back(TBplotengine::PlotInfo(TBcid(-1, -1), "M11-T5-S", TButility::mod_info(-1, -1, -1), -1, -1));
      } else {

        std::string aCName = fModule + "-T" + std::to_string(i) + "-C";
        TBcid aCCID = fUtility.GetCID(aCName);
        TButility::mod_info aCInfo = fUtility.GetInfo(aCName);

        fCIDtoPlot_Ceren.push_back(aCCID);
        std::vector<int> aCinterval = fConfig[aCName].as<std::vector<int>>();
        fPlotter_Ceren.push_back(TBplotengine::PlotInfo(aCCID, aCName, aCInfo, aCinterval.at(0), aCinterval.at(1)));

        if (fCalcInfo == TBplotengine::CalcInfo::kIntADC)
          fPlotter_Ceren.at(i - 1).SetPlot(new TH1D((TString)(aCName), ";IntADC;nEvents", 440, -30000., 300000.));

        if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC)
          fPlotter_Ceren.at(i - 1).SetPlot(new TH1D((TString)(aCName), ";PeakADC;nEvents", 288, -512., 4096.));

        fPlotter_Ceren.at(i - 1).hist1D->SetLineWidth(2);
        fPlotter_Ceren.at(i - 1).hist1D->SetLineColor(kBlue);

        std::string aSName = fModule + "-T" + std::to_string(i) + "-S";
        TBcid aSCID = fUtility.GetCID(aSName);
        TButility::mod_info aSInfo = fUtility.GetInfo(aSName);

        fCIDtoPlot_Scint.push_back(aSCID);
        std::vector<int> aSinterval = fConfig[aSName].as<std::vector<int>>();
        fPlotter_Scint.push_back(TBplotengine::PlotInfo(aSCID, aSName, aSInfo, aSinterval.at(0), aSinterval.at(1)));

        if (fCalcInfo == TBplotengine::CalcInfo::kIntADC)
          fPlotter_Scint.at(i - 1).SetPlot(new TH1D((TString)(aSName), ";IntADC;nEvents", 440, -30000., 300000.));

        if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC)
          fPlotter_Scint.at(i - 1).SetPlot(new TH1D((TString)(aSName), ";PeakADC;nEvents", 288, -512., 4096.));

        fPlotter_Scint.at(i - 1).hist1D->SetLineWidth(2);
        fPlotter_Scint.at(i - 1).hist1D->SetLineColor(kRed);
      }
    }
  }

  // std::cout << fCIDtoPlot_Ceren.size() << std::endl;
  // for (int i = 0; i < fPlotter_Ceren.size(); i++) {
  //   std::cout << i << " " << fPlotter_Ceren.at(i).name << " " << fPlotter_Ceren.at(i).info.row << " " << fPlotter_Ceren.at(i).info.col << " ";
  //   fPlotter_Ceren.at(i).cid.print();
  // }

  // std::cout << fCIDtoPlot_Scint.size() << std::endl;
  // for (int i = 0; i < fPlotter_Scint.size(); i++) {
  //   std::cout << i << " " << fPlotter_Scint.at(i).name << " " << fPlotter_Scint.at(i).info.row << " " << fPlotter_Scint.at(i).info.col << " ";
  //   fPlotter_Scint.at(i).cid.print();
  // }

  Draw();
}

void TBplotengine::init_2D() {

  if (fModule == "MCPPMT") init_MCPPMT();
  if (fModule == "SiPM")   init_SiPM();
}

void TBplotengine::init_MCPPMT() {

  for (int i = 0; i < 64; i++) {
    std::string aName = "C" + std::to_string(i + 1);
    TBcid aCID = fUtility.GetCID(aName);
    TButility::mod_info aInfo = fUtility.GetInfo(aName);

    fCIDtoPlot_Ceren.push_back(aCID);

    std::vector<int> interval = fConfig[aName].as<std::vector<int>>();
    fPlotter_Ceren.push_back(TBplotengine::PlotInfo(aCID, aName, aInfo, interval.at(0), interval.at(1)));

    if (fCalcInfo == TBplotengine::CalcInfo::kIntADC)
      fPlotter_Ceren.at(i).SetPlot(new TH1D((TString)(aName), ";IntADC;nEvents", 440, -30000., 300000.));

    if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC)
      fPlotter_Ceren.at(i).SetPlot(new TH1D((TString)(aName), ";PeakADC;nEvents", 288, -512., 4096.));
  }

  f2DHistCeren = new TH2D("CERENKOV", "CERENKOV;;", 8, 0.5, 8.5, 8, 0.5, 8.5);
  f2DHistCeren->SetStats(0);

  for (int i = 0; i < 64; i++) {
    std::string aName = "S" + std::to_string(i + 1);
    TBcid aCID = fUtility.GetCID(aName);
    TButility::mod_info aInfo = fUtility.GetInfo(aCID);

    fCIDtoPlot_Scint.push_back(aCID);

    std::vector<int> interval = fConfig[aName].as<std::vector<int>>();
    fPlotter_Scint.push_back(TBplotengine::PlotInfo(aCID, aName, aInfo, interval.at(0), interval.at(1)));

    if (fCalcInfo == TBplotengine::CalcInfo::kIntADC)
      fPlotter_Scint.at(i).SetPlot(new TH1D((TString)(aName), ";IntADC;nEvents", 440, -30000., 300000.));

    if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC)
      fPlotter_Scint.at(i).SetPlot(new TH1D((TString)(aName), ";PeakADC;nEvents", 288, -512., 4096.));
  }

  f2DHistScint = new TH2D("SCINTILLATION", "SCINTILLATION;;", 8, 0.5, 8.5, 8, 0.5, 8.5);
  f2DHistScint->SetStats(0);

  for (int i = 1; i <= 8; i++) {
    f2DHistCeren->GetXaxis()->SetBinLabel(i, std::to_string(i).c_str());
    f2DHistCeren->GetYaxis()->SetBinLabel(i, std::to_string(i).c_str());
    f2DHistScint->GetXaxis()->SetBinLabel(i, std::to_string(i).c_str());
    f2DHistScint->GetYaxis()->SetBinLabel(i, std::to_string(i).c_str());
  }

  // std::cout << fCIDtoPlot_Ceren.size() << std::endl;
  // for (int i = 0; i < fPlotter_Ceren.size(); i++) {
  //   std::cout << i << " " << fPlotter_Ceren.at(i).name << " " << fPlotter_Ceren.at(i).info.row << " " << fPlotter_Ceren.at(i).info.col << " ";
  //   fPlotter_Ceren.at(i).cid.print();
  // }

  // std::cout << fCIDtoPlot_Scint.size() << std::endl;
  // for (int i = 0; i < fPlotter_Scint.size(); i++) {
  //   std::cout << i << " " << fPlotter_Scint.at(i).name << " " << fPlotter_Scint.at(i).info.row << " " << fPlotter_Scint.at(i).info.col << " ";
  //   fPlotter_Scint.at(i).cid.print();
  // }

  Draw();
}

void TBplotengine::init_SiPM() {
  // std::cout << "init_SiPM" << std::endl;
  for (int i = 21; i <= 40; i++) {
    for (int j = 21; j <= 40; j++) {

      std::string aName = std::to_string(i) + "-" + std::to_string(j);
      TBcid aCID = fUtility.GetCID(aName);
      TButility::mod_info aInfo = fUtility.GetInfo(aName);

      // std::cout << i << " " << j << " " << aName << " " << aInfo.isCeren << " " << aInfo.row << " " << aInfo.col << std::endl;

      if (aInfo.isCeren == -1)
        continue;

      if (aInfo.isCeren) {

        fCIDtoPlot_Ceren.push_back(aCID);

        std::vector<int> interval = fConfig[aName].as<std::vector<int>>();
        fPlotter_Ceren.push_back(TBplotengine::PlotInfo(aCID, aName, aInfo, interval.at(0), interval.at(1)));

        if (fCalcInfo == TBplotengine::CalcInfo::kIntADC)
          fPlotter_Ceren.at(fPlotter_Ceren.size() - 1).SetPlot(new TH1D((TString)(aName), ";IntADC;nEvents", 440, -30000., 300000.));

        if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC)
          fPlotter_Ceren.at(fPlotter_Ceren.size() - 1).SetPlot(new TH1D((TString)(aName), ";PeakADC;nEvents", 288, -512., 4096.));

      } else {

        fCIDtoPlot_Scint.push_back(aCID);

        std::vector<int> interval = fConfig[aName].as<std::vector<int>>();
        fPlotter_Scint.push_back(TBplotengine::PlotInfo(aCID, aName, aInfo, interval.at(0), interval.at(1)));

        if (fCalcInfo == TBplotengine::CalcInfo::kIntADC)
          fPlotter_Scint.at(fPlotter_Scint.size() - 1).SetPlot(new TH1D((TString)(aName), ";IntADC;nEvents", 440, -30000., 300000.));

        if (fCalcInfo == TBplotengine::CalcInfo::kPeakADC)
          fPlotter_Scint.at(fPlotter_Scint.size() - 1).SetPlot(new TH1D((TString)(aName), ";PeakADC;nEvents", 288, -512., 4096.));

      }
    }
  }

  f2DHistCeren = new TH2D("CERENKOV", "CERENKOV;;", 20, 0.5, 20.5, 20, 0.5, 20.5);
  f2DHistCeren->SetStats(0);

  f2DHistScint = new TH2D("SCINTILLATION", "SCINTILLATION;;", 20, 0.5, 20.5, 20, 0.5, 20.5);
  f2DHistScint->SetStats(0);

  for (int i = 1; i <= 20; i++) {
    f2DHistCeren->GetXaxis()->SetBinLabel(i, std::to_string(i + 20).c_str());
    f2DHistCeren->GetYaxis()->SetBinLabel(i, std::to_string(i + 20).c_str());
    f2DHistScint->GetXaxis()->SetBinLabel(i, std::to_string(i + 20).c_str());
    f2DHistScint->GetYaxis()->SetBinLabel(i, std::to_string(i + 20).c_str());
  }

  // std::cout << fCIDtoPlot_Ceren.size() << std::endl;
  // for (int i = 0; i < fPlotter_Ceren.size(); i++) {
  //   std::cout << i << " " << fPlotter_Ceren.at(i).name << " " << fPlotter_Ceren.at(i).info.row << " " << fPlotter_Ceren.at(i).info.col << " ";
  //   fPlotter_Ceren.at(i).cid.print();
  // }

  // std::cout << fCIDtoPlot_Scint.size() << std::endl;
  // for (int i = 0; i < fPlotter_Scint.size(); i++) {
  //   std::cout << i << " " << fPlotter_Scint.at(i).name << " " << fPlotter_Scint.at(i).info.row << " " << fPlotter_Scint.at(i).info.col << " ";
  //   fPlotter_Scint.at(i).cid.print();
  // }

  Draw();
}

double TBplotengine::GetPeakADC(std::vector<short> waveform, int xInit, int xFin) {
  double ped = 0;
  for (int i = 1; i < 101; i++)
    ped += (double)waveform.at(i) / 100.;

  std::vector<double> pedCorWave;
  for (int i = xInit; i < xFin; i++)
    pedCorWave.push_back(ped - (double)waveform.at(i));

  return *std::max_element(pedCorWave.begin(), pedCorWave.end());
}

double TBplotengine::GetIntADC(std::vector<short> waveform, int xInit, int xFin) {
  double ped = 0;
  for (int i = 1; i < 101; i++)
    ped += (double)waveform.at(i) / 100.;

  double intADC_ = 0;
  for (int i = xInit; i < xFin; i++)
    intADC_ += ped - (double)waveform.at(i);

  return intADC_;
}


void TBplotengine::PrintInfo() {

}

void TBplotengine::Fill(TBevt<TBwaveform> anEvent) {

  if (fCaseName == "single") {
    if (fCalcInfo == TBplotengine::CalcInfo::kIntADC || fCalcInfo == TBplotengine::CalcInfo::kPeakADC) {
      for (int i = 0; i < fPlotter_Ceren.size(); i++) {
        double value = GetValue(anEvent.GetData(fPlotter_Ceren.at(i).cid).waveform(), fPlotter_Ceren.at(i).xInit, fPlotter_Ceren.at(i).xFin);
        fPlotter_Ceren.at(i).hist1D->Fill(value);
      }
    } else if (fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc) {
      for (int i = 0; i < fPlotter_Ceren.size(); i++) {
        auto tWave = anEvent.GetData(fPlotter_Ceren.at(i).cid).waveform();
        for (int j = 1; j <= 1000; j++) {
          fPlotter_Ceren.at(i).hist1D->Fill(j, tWave.at(j));
        }
        fPlotter_Ceren.at(i).xInit++;
      }
    } else if (fCalcInfo == TBplotengine::CalcInfo::kOverlay) {
      for (int i = 0; i < fPlotter_Ceren.size(); i++) {
        auto tWave = anEvent.GetData(fPlotter_Ceren.at(i).cid).waveform();
        for (int j = 0; j < tWave.size(); j++) {
          fPlotter_Ceren.at(i).hist2D->Fill(j, tWave.at(j));
        }
      }
    }

  } else if (fCaseName == "heatmap") {

    for (int i = 0; i < fPlotter_Ceren.size(); i++) {
      double value = GetValue(anEvent.GetData(fPlotter_Ceren.at(i).cid).waveform(), fPlotter_Ceren.at(i).xInit, fPlotter_Ceren.at(i).xFin);
      fPlotter_Ceren.at(i).hist1D->Fill(value);
    }

    for (int i = 0; i < fPlotter_Scint.size(); i++) {
      double value = GetValue(anEvent.GetData(fPlotter_Scint.at(i).cid).waveform(), fPlotter_Scint.at(i).xInit, fPlotter_Scint.at(i).xFin);
      fPlotter_Scint.at(i).hist1D->Fill(value);
    }
  } else if (fCaseName == "module") {

    for (int i = 0; i < fPlotter_Ceren.size(); i++) {
      if (i == 4) continue;
      double value = GetValue(anEvent.GetData(fPlotter_Ceren.at(i).cid).waveform(), fPlotter_Ceren.at(i).xInit, fPlotter_Ceren.at(i).xFin);
      fPlotter_Ceren.at(i).hist1D->Fill(value);
    }

    for (int i = 0; i < fPlotter_Scint.size(); i++) {
      if (i == 4) continue;
      double value = GetValue(anEvent.GetData(fPlotter_Scint.at(i).cid).waveform(), fPlotter_Scint.at(i).xInit, fPlotter_Scint.at(i).xFin);
      fPlotter_Scint.at(i).hist1D->Fill(value);
    }
  }
}

void TBplotengine::Draw() {
;
  if (fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc) {
    for (int i = 0; i < fPlotter_Ceren.size(); i++)
      fLeg->AddEntry(fPlotter_Ceren.at(i).hist1D, fPlotter_Ceren.at(i).name.c_str(), "l");

  }

  fCanvas->cd();
  if (fCaseName == "single") {
    if (fCalcInfo == TBplotengine::CalcInfo::kOverlay) {
      fPlotter_Ceren.at(0).hist2D->Draw("colz");

    } else {
      fMainFrame->Draw();
      for (int i = 0; i < fPlotter_Ceren.size(); i++)
        fPlotter_Ceren.at(i).hist1D->Draw("sames");

      if (fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc)
        fLeg->Draw("same");
    }

  } else if (fCaseName == "heatmap") {
    fCanvas->cd(1);
    f2DHistCeren->Draw("colz text");

    fCanvas->cd(2);
    f2DHistScint->Draw("colz text");
  } else if (fCaseName == "module") {

    for (int i = 0; i < fPlotter_Ceren.size(); i++) {
      if (i == 4) continue;

      fCanvas->cd(i + 1);
      fPlotter_Ceren.at(i).hist1D->Draw("Hist");
      fPlotter_Scint.at(i).hist1D->Draw("Hist & sames");
    }
  }

  gSystem->ProcessEvents();
  gSystem->Sleep(3000);
}

void TBplotengine::Update() {

  if (fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc)
    for (int i = 0; i < fPlotter_Ceren.size(); i++)
      fPlotter_Ceren.at(i).hist1D->Scale(1./(float)fPlotter_Ceren.at(i).xInit);

  if (fCaseName == "single") {
    if (fCalcInfo == TBplotengine::CalcInfo::kIntADC || fCalcInfo == TBplotengine::CalcInfo::kPeakADC || fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc)
      SetMaximum();

    if (fIsFirst) {
      fIsFirst = false;
      if (fCalcInfo == TBplotengine::CalcInfo::kOverlay) {
        fPlotter_Ceren.at(0).hist2D->Draw("colz");

      } else {
        fMainFrame->Draw();

        double stat_height = (1. - 0.2) / (double)fPlotter_Ceren.size();
        for (int i = 0; i < fPlotter_Ceren.size(); i++) {

          fPlotter_Ceren.at(i).hist1D->Draw("Hist & sames");

          if (fCalcInfo == TBplotengine::CalcInfo::kIntADC || fCalcInfo == TBplotengine::CalcInfo::kPeakADC) {
            fCanvas->Update();
            // TPaveStats* stat = (TPaveStats*)fCanvas->GetPrimitive("stats");
            TPaveStats* stat = (TPaveStats*)fPlotter_Ceren.at(i).hist1D->FindObject("stats");
            // stat->SetName(fPlotter_Ceren.at(i).hist1D->GetName() + (TString)"_stat");
            stat->SetTextColor(fPlotter_Ceren.at(i).hist1D->GetLineColor());
            stat->SetY2NDC(1. - stat_height * i);
            stat->SetY1NDC(1 - stat_height * (i + 1));
            stat->SaveStyle();
          }
        }

        // if ((fCalcInfo == TBplotengine::CalcInfo::kIntADC || fCalcInfo == TBplotengine::CalcInfo::kPeakADC) && fPlotter_Ceren.size() >5)
        //   fLeg->Draw("same");

        if (fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc)
          fLeg->Draw("same");
      }
    }
  } else if (fCaseName == "heatmap") {

    for (int i = 0; i < fPlotter_Ceren.size(); i++) {
      f2DHistCeren->SetBinContent(fPlotter_Ceren.at(i).info.row, fPlotter_Ceren.at(i).info.col, (int)fPlotter_Ceren.at(i).hist1D->GetMean());

      // std::cout << "CEREN - " << i
      //           << " " << fPlotter_Ceren.at(i).name
      //           << " " << fPlotter_Ceren.at(i).cid.mid()
      //           << " " << fPlotter_Ceren.at(i).cid.channel()
      //           << " " << fPlotter_Ceren.at(i).hist1D->GetMean() << std::endl;

    }
    for (int i = 0; i < fPlotter_Scint.size(); i++) {
      f2DHistScint->SetBinContent(fPlotter_Scint.at(i).info.row, fPlotter_Scint.at(i).info.col, (int)fPlotter_Scint.at(i).hist1D->GetMean());

      // std::cout << "SCINT - " << i
      //           << " " << fPlotter_Scint.at(i).name
      //           << " " << fPlotter_Scint.at(i).cid.mid()
      //           << " " << fPlotter_Scint.at(i).cid.channel()
      //           << " " << fPlotter_Scint.at(i).hist1D->GetMean() << std::endl;

    }
    fCanvas->cd(1);
    f2DHistCeren->Draw("colz text");

    fCanvas->cd(2);
    f2DHistScint->Draw("colz text");

  } else if (fCaseName == "module") {

    for (int i = 0; i < fPlotter_Ceren.size(); i++) {
      if (i == 4) continue;

      if (fPlotter_Ceren.at(i).hist1D->GetMaximum() > fPlotter_Ceren.at(i).hist1D->GetMaximum()) {

        fCanvas->cd(i + 1);
        fPlotter_Ceren.at(i).hist1D->Draw("Hist");

        if (fIsFirst) {
          fCanvas->Update();
          TPaveStats* stat_c = (TPaveStats*)fPlotter_Ceren.at(i).hist1D->FindObject("stats");
          stat_c->SetTextColor(4);
          stat_c->SetY2NDC(1.);
          stat_c->SetY1NDC(.8);
          stat_c->SaveStyle();
        }

        fCanvas->cd(i + 1);
        fPlotter_Scint.at(i).hist1D->Draw("Hist & sames");

        if (fIsFirst) {
          fCanvas->Update();
          TPaveStats* stat_s = (TPaveStats*)fPlotter_Scint.at(i).hist1D->FindObject("stats");
          stat_s->SetTextColor(2);
          stat_s->SetY2NDC(.8);
          stat_s->SetY1NDC(.6);
          stat_s->SaveStyle();
        }
      } else {

        fCanvas->cd(i + 1);
        fPlotter_Scint.at(i).hist1D->Draw("Hist");

        if (fIsFirst) {
          fCanvas->Update();
          TPaveStats* stat_s = (TPaveStats*)fPlotter_Scint.at(i).hist1D->FindObject("stats");
          stat_s->SetTextColor(2);
          stat_s->SetY2NDC(.8);
          stat_s->SetY1NDC(.6);
          stat_s->SaveStyle();
        }

        fCanvas->cd(i + 1);
        fPlotter_Ceren.at(i).hist1D->Draw("Hist & sames");

        if (fIsFirst) {
          fCanvas->Update();
          TPaveStats* stat_c = (TPaveStats*)fPlotter_Ceren.at(i).hist1D->FindObject("stats");
          stat_c->SetTextColor(4);
          stat_c->SetY2NDC(1.);
          stat_c->SetY1NDC(.8);
          stat_c->SaveStyle();
        }
      }
    }

    fIsFirst = false;
  }

  SaveAs("");
  fCanvas->Update();
  fCanvas->Pad()->Draw();

  if (!fLive) {
    fApp->Run(true);
  } else {
    gSystem->ProcessEvents();
  }

  gSystem->Sleep(5000);

  if (fLive)
    if (fCalcInfo == TBplotengine::CalcInfo::kAvgTimeStruc)
      for (int i = 0; i < fPlotter_Ceren.size(); i++)
        fPlotter_Ceren.at(i).hist1D->Scale((float)fPlotter_Ceren.at(i).xInit);


}

void TBplotengine::SetMaximum() {

  float max = -999;
  for (int i = 0; i < fPlotter_Ceren.size(); i++) {
    if (max < fPlotter_Ceren.at(i).hist1D->GetMaximum()) {
      max = fPlotter_Ceren.at(i).hist1D->GetMaximum();
    }
    // std::cout << fPlotter_Ceren.at(i).hist1D->GetName() << " " << fPlotter_Ceren.at(i).hist1D->GetMaximum() << std::endl;
  }

  // std::cout << "TBplotengine::SetMaximum() : " << max << std::endl;

  fMainFrame->GetYaxis()->SetRangeUser(0., max * 1.2);
}

void TBplotengine::SaveAs(TString output = "")
{
  if (output == "")
    output = "./output/Run" + std::to_string(fRunNum) + "_" + fCaseName + "_" + fMethod + "_" + fModule + ".root";

  TFile* outoutFile = new TFile(output, "RECREATE");

  outoutFile->cd();
  if (fCaseName == "single") {
    if (fMethod == "Overlay") {
      for (int i = 0; i < fPlotter_Ceren.size(); i++)
        fPlotter_Ceren.at(i).hist2D->Write();
    } else {
      for (int i = 0; i < fPlotter_Ceren.size(); i++)
        fPlotter_Ceren.at(i).hist1D->Write();
    }

  } else if (fCaseName == "heatmap") {
    f2DHistCeren->Write();
    f2DHistScint->Write();

    for (int i = 0; i < fPlotter_Ceren.size(); i++)
      fPlotter_Ceren.at(i).hist1D->Write();

    for (int i = 0; i < fPlotter_Scint.size(); i++)
      fPlotter_Scint.at(i).hist1D->Write();

  } else if (fCaseName == "module") {

    for (int i = 0; i < fPlotter_Ceren.size(); i++) {
      if (i == 4) continue;
      fPlotter_Ceren.at(i).hist1D->Write();
    }

    for (int i = 0; i < fPlotter_Scint.size(); i++) {
      if (i == 4) continue;
      fPlotter_Scint.at(i).hist1D->Write();
    }
  }

  outoutFile->Close();
}

std::vector<int> TBplotengine::GetUniqueMID() {
  if (fCaseName == "single") {

    return fUtility.GetUniqueMID(fCIDtoPlot_Ceren);
  } else if (fCaseName == "heatmap" || fCaseName == "module") {

    return fUtility.GetUniqueMID(fCIDtoPlot_Ceren, fCIDtoPlot_Scint);
  }

  return std::vector<int>{};
}
