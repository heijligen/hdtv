import os

import ROOT

hpp = ["MatOp.hpp", "MFileHist.hpp", "VMatrix.hpp"]

ROOT.gSystem.Load("/home/user/.cache/hdtv_mfile/libmfile.so")
ROOT.gInterpreter.AddIncludePath("/home/user/.cache/hdtv_mfile")
ROOT.gInterpreter.ProcessLine('#include <mfile.h>')

ROOT.gSystem.SetBuildDir("~/.cache/hdtv/experimental", True)
for h in hpp:
    h = os.path.join(os.path.dirname(__file__), "mfile-root", h)
    ROOT.gInterpreter.ProcessLine(f".L {h}")
