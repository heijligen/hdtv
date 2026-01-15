/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This file is part of HDTV - a ROOT-based spectrum analysis software
 *   Copyright (C) 2006-2009  The HDTV development team (see file AUTHORS)
 */

#pragma once


#include <cmath>
#include <list>

#include <TH1.h>
#include <TH2.h>

#ifndef __CLING__
#include "MFileHist.hpp"
#endif

// VMatrix and RMatrix should be moved to a different module, as they are not
// related to MFile...

/*
  Cut  ^
  Axis |
       |
       |+++++++++++++++++++
       |+++++++++++++++++++
       |+++++++++++++++++++
       |    |    |    |
       |    |    |    |
       |    v    v    v
       +------------------->
               Projection axis
*/

//! A ``virtual matrix'', i.e. a matrix that is not necessarily stored in memory
class VMatrix {
public:
  VMatrix() : fFail(false){};
  virtual ~VMatrix() = default;

  void AddCutRegion(int c1, int c2) { AddRegion(fCutRegions, c1, c2); }
  void AddBgRegion(int c1, int c2) { AddRegion(fBgRegions, c1, c2); }
  void ResetRegions() {
    fCutRegions.clear();
    fBgRegions.clear();
  }

  TH1 *Cut(const char *histname, const char *histtitle) {
    int l, l1, l2; // lines
    std::list<int>::iterator iter;
    int nCut = 0, nBg = 0; // total number of cut and background lines
    int pbins = GetProjXbins();

    if (Failed()) {
      return nullptr;
    }

    if (fCutRegions.empty()) {
      return nullptr;
    }

    // Sum of cut lines
    TArrayD sum(pbins);
    sum.Reset(0.0);

    // Sum of background lines
    TArrayD bg(pbins);
    bg.Reset(0.0);

    try {
      // Add up all cut lines
      iter = fCutRegions.begin();
      while (iter != fCutRegions.end()) {
        l1 = *iter++;
        l2 = *iter++;
        for (l = l1; l <= l2; l++) {
          AddLine(sum, l);
          nCut++;
        }
      }

      // Add up all background lines
      iter = fBgRegions.begin();
      while (iter != fBgRegions.end()) {
        l1 = *iter++;
        l2 = *iter++;
        for (l = l1; l <= l2; l++) {
          AddLine(bg, l);
          nBg++;
        }
      }
    } catch (ReadException &) {
      return nullptr;
    }

    double bgFac = (nBg == 0) ? 0.0 : static_cast<double>(nCut) / nBg;
    auto hist = new TH1D(histname, histtitle, GetProjXbins(), GetProjXmin(), GetProjXmax());
    // cols, -0.5, (double) cols - 0.5);
    for (int c = 0; c < pbins; c++) {
      hist->SetBinContent(c + 1, sum[c] - bg[c] * bgFac);
    }

    return hist;
    
  }

  // Cut axis info
  virtual int FindCutBin(double x) = 0;
  virtual int GetCutLowBin() = 0;
  virtual int GetCutHighBin() = 0;

  // Projection axis info
  virtual double GetProjXmin() = 0;
  virtual double GetProjXmax() = 0;
  virtual int GetProjXbins() = 0;

  virtual void AddLine(TArrayD &dst, int l) = 0;

  bool Failed() { return fFail; }

private:
  
  std::list<int> fCutRegions, fBgRegions;
  
  void AddRegion(std::list<int> &reglist, int l1, int l2) {
    std::list<int>::iterator iter, next;
    bool inside = false;
    int min = std::min(l1, l2);
    int max = std::max(l1, l2);

    // Perform clipping
    if (max < GetCutLowBin() || min > GetCutHighBin()) {
      return;
    }
    min = std::max(min, GetCutLowBin());
    max = std::min(max, GetCutHighBin());

    iter = reglist.begin();
    while (iter != reglist.end() && *iter < min) {
      inside = !inside;
      iter++;
    }

    if (!inside) {
      iter = reglist.insert(iter, min);
      iter++;
    }

    while (iter != reglist.end() && *iter < max) {
      inside = !inside;
      next = iter;
      next++;
      reglist.erase(iter);
      iter = next;
    }

    if (!inside) {
      reglist.insert(iter, max);
    }
  }

protected:
  bool fFail;
  class ReadException {};
};

//! ROOT TH2-backed VMatrix
class RMatrix : public VMatrix {
public:
  enum ProjAxis_t { PROJ_X, PROJ_Y };

  RMatrix(TH2 *hist, ProjAxis_t paxis) : VMatrix(), fHist(hist), fProjAxis(paxis) {};
  ~RMatrix() override = default;

  int FindCutBin(double x) override {
    TAxis *a = (fProjAxis == PROJ_X) ? fHist->GetYaxis() : fHist->GetXaxis();
    return a->FindBin(x);
  }

  int GetCutLowBin() override { return 1; }
  int GetCutHighBin() override { return (fProjAxis == PROJ_X) ? fHist->GetNbinsY() : fHist->GetNbinsX(); }

  double GetProjXmin() override {
    TAxis *a = (fProjAxis == PROJ_X) ? fHist->GetXaxis() : fHist->GetYaxis();
    return a->GetXmin();
  }

  double GetProjXmax() override {
    TAxis *a = (fProjAxis == PROJ_X) ? fHist->GetXaxis() : fHist->GetYaxis();
    return a->GetXmax();
  }

  int GetProjXbins() override { return (fProjAxis == PROJ_X) ? fHist->GetNbinsX() : fHist->GetNbinsY(); }

  void AddLine(TArrayD &dst, int l) override {
if (fProjAxis == PROJ_X) {
    int cols = fHist->GetNbinsX();

    for (int c = 1; c <= cols; ++c) {
      // Bad for speed; overloaded operator[] checks array bounds
      dst[c - 1] += fHist->GetBinContent(c, l);
    }
  } else {
    int cols = fHist->GetNbinsY();

    for (int c = 1; c <= cols; ++c) {
      // Bad for speed; overloaded operator[] checks array bounds
      dst[c - 1] += fHist->GetBinContent(l, c);
    }
  }
    
  }

private:
  TH2 *fHist;
  ProjAxis_t fProjAxis;
};

//! MFile-histogram-backed VMatrix
class MFMatrix : public VMatrix {
public:
  MFMatrix(MFileHist *mat, unsigned int level) : VMatrix(), fMatrix(mat), fLevel(level), fBuf() {
  // Sanity checks
  if (fLevel >= fMatrix->GetNLevels()) {
    fFail = true;
  } else {
    fBuf.Set(fMatrix->GetNColumns());
  }
}


  
  ~MFMatrix() override = default;

  int FindCutBin(double x) override // convert channel to bin number
  {
    return std::ceil(x - 0.5);
  }

  int GetCutLowBin() override { return 0; }
  int GetCutHighBin() override { return fMatrix->GetNLines() - 1; }

  double GetProjXmin() override { return -0.5; }
  double GetProjXmax() override { return fMatrix->GetNColumns() - .5; }
  int GetProjXbins() override { return fMatrix->GetNColumns(); }

  void AddLine(TArrayD &dst, int l) override {
    if (!fMatrix->FillBuf1D(fBuf.GetArray(), fLevel, l)) {
      throw ReadException();
    }

    int cols = fMatrix->GetNColumns();

    for (int c = 0; c < cols; ++c) {
      // Bad for speed; overloaded operator[] checks array bounds
      dst[c] += fBuf[c];
    }
  }

private:
  MFileHist *fMatrix;
  unsigned int fLevel;
  TArrayD fBuf;
};

