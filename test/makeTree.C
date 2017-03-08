#include <TFile.h>

void makeTree() {
    gFile->MakeProject("edepsimTree","*","recreate++");
}
