// -- define basic values used over the analysis
#pragma once

#include <utility> // -- include pair

namespace TB2024 {
  int nADCBin = 1000;

  /////////////////////////////
  // -- integration range -- //
  /////////////////////////////

  std::pair<int, int> intRange_drc_default_S = std::pair<int, int>(260, 460);
  std::pair<int, int> intRange_drc_default_C = std::pair<int, int>(260, 420); // -- tighter range
  std::pair<int, int> intRange_drc_M5_S   = std::pair<int, int>(200, 400); // -- shift by 60
  std::pair<int, int> intRange_drc_M5T3_S = std::pair<int, int>(180, 360); // -- MCP-PMT; shaper shape
  std::pair<int, int> intRange_drc_M5_C   = std::pair<int, int>(200, 360); // -- shift by 60
  std::pair<int, int> intRange_drc_M5T3_C = std::pair<int, int>(180, 320); // -- MCP-PMT; shaper shape

  std::pair<int, int> GetIntRange_DRC(TString tag) {
    bool isC = false;
    if( tag.EndsWith("-C") ) isC = true;

    if( isC ) {
      if( tag.BeginsWith("M5-") ) {
        if( tag.BeginsWith("M5-T3") ) return intRange_drc_M5T3_C;
        else                          return intRange_drc_M5_C;
      }
      return intRange_drc_default_C; // -- defualt
    }
    else { // -- S channel
      if( tag.BeginsWith("M5-") ) {
        if( tag.BeginsWith("M5-T3") ) return intRange_drc_M5T3_S;
        else                          return intRange_drc_M5_S;
      }
      return intRange_drc_default_S; // -- defualt
    }

    std::cout << "[TB2024::GetIntRange_DRC] tag = " << tag << " is not supported" << std::endl;
    throw std::invalid_argument("wrong tag");

    return intRange_drc_default_S; // -- not used anyway...
  }

  // ---- aux. detectors
  std::pair<int, int> intRange_ps  = std::pair<int, int>(320, 440);
  std::pair<int, int> intRange_mc  = std::pair<int, int>(720, 880);
  std::pair<int, int> intRange_dwc = std::pair<int, int>(0, 1000); // -- dummy (not used anyway)

  std::pair<int, int> GetIntRange_Aux(TString tag) {
    if( tag == "PS" )              return intRange_ps;
    else if( tag == "MC" )         return intRange_mc;
    else if( tag.Contains("DWC") ) return intRange_dwc;
    else
      throw std::invalid_argument("[GetIntRange_Aux] tag = " + tag + " is not recognized");

    return std::pair<int, int>(0, 0);
  }

  /////////////////////////////////////////// 
  // -- event selection (EM resolution) -- //
  ///////////////////////////////////////////
  std::pair<float, float> cut_DWC_size = std::pair<float, float>(-5.0, 5.0);
  std::pair<float, float> cut_DWC_diff_x = std::pair<float, float>(0, 4); // -- difference between two DWC positions (x)
  std::pair<float, float> cut_DWC_diff_y = std::pair<float, float>(0, 4); // -- difference between two DWC positions (y)

  std::pair<float, float> cut_PS = std::pair<float, float>(700, 1e5);
  std::pair<float, float> cut_MC = std::pair<float, float>(0, 40);
  std::pair<int, int> GetCut_Aux(TString tag) {
    if( tag == "PS" )              return cut_PS;
    else if( tag == "MC")          return cut_MC;
    else if( tag.Contains("DWC") ) return std::pair<int, int>(0, 0); // -- dummy (not used)
    else
      throw std::invalid_argument("[GetCut_Aux] tag = " + tag + " is not recognized");

    return std::pair<int, int>(0, 0);
  }

  std::map<TString, double> map_calibConst = {
    {"M1-T1-C", 0.000727},
    {"M1-T1-S", 0.000522},
    {"M1-T2-C", 0.000799},
    {"M1-T2-S", 0.000489},
    {"M1-T3-C", 0.000842},
    {"M1-T3-S", 0.000396},
    {"M1-T4-C", 0.000783},
    {"M1-T4-S", 0.000468},
    {"M2-T1-C", 0.000792},
    {"M2-T1-S", 0.000472},
    {"M2-T2-C", 0.000765},
    {"M2-T2-S", 0.000461},
    {"M2-T3-C", 0.000826},
    {"M2-T3-S", 0.000472},
    {"M2-T4-C", 0.000794},
    {"M2-T4-S", 0.000473},
    {"M3-T1-C", 0.000781},
    {"M3-T1-S", 0.000441},
    {"M3-T2-C", 0.000785},
    {"M3-T2-S", 0.000465},
    {"M3-T3-C", 0.000800},
    {"M3-T3-S", 0.000465},
    {"M3-T4-C", 0.000795},
    {"M3-T4-S", 0.000467},
    {"M4-T1-C", 0.000777},
    {"M4-T1-S", 0.000461},
    {"M4-T2-C", 0.000824},
    {"M4-T2-S", 0.000482},
    {"M4-T3-C", 0.000814},
    {"M4-T3-S", 0.000494},
    {"M4-T4-C", 0.000767},
    {"M4-T4-S", 0.000483},
    {"M5-T1-C", 0.000838},
    {"M5-T1-S", 0.000507},
    {"M5-T2-C", 0.000861},
    {"M5-T2-S", 0.000483},
    {"M5-T3-C", 0.001224},
    {"M5-T3-S", 0.000870},
    {"M5-T4-C", 0.000805},
    {"M5-T4-S", 0.000524},
    {"M6-T1-C", 0.000790},
    {"M6-T1-S", 0.000494},
    {"M6-T2-C", 0.000772},
    {"M6-T2-S", 0.000492},
    {"M6-T3-C", 0.000787},
    {"M6-T3-S", 0.000491},
    {"M6-T4-C", 0.000803},
    {"M6-T4-S", 0.000535},
    {"M7-T1-C", 0.000765},
    {"M7-T1-S", 0.000439},
    {"M7-T2-C", 0.000788},
    {"M7-T2-S", 0.000493},
    {"M7-T3-C", 0.000810},
    {"M7-T3-S", 0.000493},
    {"M7-T4-C", 0.000827},
    {"M7-T4-S", 0.000494},
    {"M8-T1-C", 0.000793},
    {"M8-T1-S", 0.000491},
    {"M8-T2-C", 0.000827},
    {"M8-T2-S", 0.000479},
    {"M8-T3-C", 0.000818},
    {"M8-T3-S", 0.000499},
    {"M8-T4-C", 0.000778},
    {"M8-T4-S", 0.000473},
    {"M9-T1-C", 0.000740},
    {"M9-T1-S", 0.000477},
    {"M9-T2-C", 0.000815},
    {"M9-T2-S", 0.000452},
    {"M9-T3-C", 0.000797},
    {"M9-T3-S", 0.000495},
    {"M9-T4-C", 0.000761},
    {"M9-T4-S", 0.000492},
  };

  std::map<TString, double> map_sf = {
    {"c", 0.786362},
    {"s", 0.791671},
  };

}; // -- end of namespace TB2024