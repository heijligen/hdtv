import os

import ROOT

hpp = os.path.join(os.path.dirname(__file__), "calibration", "Calibration.hpp")

ROOT.gSystem.SetBuildDir("~/.cache/hdtv/experimental", True)
ROOT.gInterpreter.ProcessLine(f".L {hpp}+")
