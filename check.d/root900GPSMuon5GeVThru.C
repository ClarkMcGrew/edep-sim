#include <string>
#include <iostream>

#include <TFile.h>
#include <TH1.h>
void root900GPSMuon5GeVThru(const char* input, const char* output) {
    TFile inputFile(input);

    TH1* hist = (TH1*) inputFile.Get("driftHits");
    if (!hist) {
        std::cout << "NOT RUN" << std::endl;
        return;
    }

    double histMean = hist->GetMean();
    std::cout << "Histogram Mean: " << histMean << std::endl;
    if (histMean < 0.19 || histMean > 0.21) {
        std::cout << "FAILED" << std::endl;
        return;
    }

    std::cout << "SUCCESS" << std::endl;
}
