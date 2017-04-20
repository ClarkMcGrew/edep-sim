#include "TGUIManager.hxx"
#include "TEventDisplay.hxx"

#include <TGFrame.h>
#include <TGButton.h>
#include <TGListBox.h>
#include <TGLabel.h>
#include <TGTextEntry.h>

#include <TEveManager.h>
#include <TEveBrowser.h>

#include <TSystem.h>

EDep::TGUIManager::TGUIManager() {
    MakeResultsTab();
    MakeControlTab();
}

void EDep::TGUIManager::MakeControlTab() {
    TEveBrowser* browser = gEve->GetBrowser();

    // Define the generic layout.  The last four parameters are the padding
    // around the widgets.
    TGLayoutHints* layoutHints 
        = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,
                            2, 2, 2, 2);

    // Embed a new frame in the event browser to have control buttons.
    browser->StartEmbedding(TRootBrowser::kLeft);
    TGMainFrame* mainFrame = new TGMainFrame(gClient->GetRoot(), 1000, 600);
    mainFrame->SetWindowName("Event Control");
    mainFrame->SetCleanup(kDeepCleanup);

    TGVerticalFrame* hf = new TGVerticalFrame(mainFrame);
    TGTextButton* textButton;

    // The general action buttons.
    textButton = new TGTextButton(hf, "Previous Event");
    textButton->SetToolTipText("Go to previous event.");
    textButton->SetTextJustify(36);
    textButton->SetMargins(0,0,0,0);
    textButton->SetWrapLength(-1);
    hf->AddFrame(textButton, layoutHints);
    fPrevEventButton = textButton;

    textButton = new TGTextButton(hf, "Redraw Event");
    textButton->SetToolTipText("Refresh the current view.");
    textButton->SetTextJustify(36);
    textButton->SetMargins(0,0,0,0);
    textButton->SetWrapLength(-1);
    hf->AddFrame(textButton, layoutHints);
    fDrawEventButton = textButton;

    textButton = new TGTextButton(hf, "Next Event");
    textButton->SetToolTipText("Go to previous event.");
    textButton->SetTextJustify(36);
    textButton->SetMargins(0,0,0,0);
    textButton->SetWrapLength(-1);
    hf->AddFrame(textButton, layoutHints);
    fNextEventButton = textButton;

    // Create the buttons to select which types of objects are showed.
    TGCheckButton *checkButton;

    /////////////////////
    // Buttons to control how truth objects are drawn.
    /////////////////////

    checkButton = new TGCheckButton(hf,"Show Trajectories");
    checkButton->SetToolTipText(
        "Show the GEANT4 trajectories and trajectory points.");
    checkButton->SetTextJustify(36);
    checkButton->SetMargins(0,0,0,0);
    checkButton->SetWrapLength(-1);
    hf->AddFrame(checkButton, layoutHints);
    fShowTrajectoriesButton = checkButton;

    checkButton = new TGCheckButton(hf,"Show G4 Hits");
    checkButton->SetToolTipText(
        "Show the GEANT4 hits.  This shows the energy deposition in the "
        "defined sensitive detectors.");
    checkButton->SetTextJustify(36);
    checkButton->SetMargins(0,0,0,0);
    checkButton->SetWrapLength(-1);
    checkButton->SetOn();
    hf->AddFrame(checkButton, layoutHints);
    fShowG4HitsButton = checkButton;

    checkButton = new TGCheckButton(hf,"Recalculate view");
    checkButton->SetToolTipText(
        "Recalculate center of rotation based.");
    checkButton->SetTextJustify(36);
    checkButton->SetMargins(0,0,0,0);
    checkButton->SetWrapLength(-1);
    checkButton->SetOn();
    hf->AddFrame(checkButton, layoutHints);
    fRecalculateViewButton = checkButton;

    
    // Do the final layout and mapping.
    mainFrame->AddFrame(hf, layoutHints);
    mainFrame->MapSubwindows();
    mainFrame->Resize();
    mainFrame->MapWindow();
    browser->StopEmbedding();
    browser->SetTabTitle("Control", 0);
}

void EDep::TGUIManager::MakeResultsTab() {
    TEveBrowser* browser = gEve->GetBrowser();

    // Embed a new frame in the event browser to select fit objects.
    browser->StartEmbedding(TRootBrowser::kLeft);
    TGMainFrame* mainFrame = new TGMainFrame(gClient->GetRoot(), 1000, 600);
    mainFrame->SetWindowName("Fit Selection");
    mainFrame->SetCleanup(kDeepCleanup);

    TGVerticalFrame* hf = new TGVerticalFrame(mainFrame);

    // Create the listbox for the results.
    fResultsList = new TGListBox(hf);
    fResultsList->SetMultipleSelections(true);
    TGLayoutHints* layoutList = new TGLayoutHints(kLHintsLeft
                                                  | kLHintsTop
                                                  | kLHintsExpandX 
                                                  | kLHintsExpandY);
    hf->AddFrame(fResultsList,layoutList);

    // Create a text entry button to choose a default result.
    TGLabel* txt = new TGLabel(hf,"Default Result");
    TGLayoutHints* layoutHints = new TGLayoutHints(kLHintsLeft
                                                   | kLHintsTop
                                                   | kLHintsExpandX,
                                                   2,2,2,2);
    hf->AddFrame(txt,layoutHints);

    // Create a text entry button to choose a default result.  This also sets
    // the default default result.  It should be possible to override this
    // from the command line!
    fDefaultResult = new TGTextEntry(hf);
    fDefaultResult->SetText("TCaptainRecon/final");
    fDefaultResult->SetToolTipText(
        "Enter a regular expression to select algorithm results\n"
        "to be shown.  The usual regexp syntax applys:\n" 
        "    .   -- Match any character\n"
        "    .*  -- Match any sequence of characters\n"
        "    |   -- \'or\'\n"
        "See grep for more details.  A result is show if the\n"
        "regexpn matchs any sub-string in the result name." );

    hf->AddFrame(fDefaultResult,layoutHints);
    
    // Do the final layout and mapping.
    TGLayoutHints* layoutFrame 
        = new TGLayoutHints(kLHintsLeft 
                            | kLHintsTop 
                            | kLHintsExpandX
                            | kLHintsExpandY,
                            2, 2, 2, 2);
    mainFrame->AddFrame(hf, layoutFrame);
    mainFrame->MapSubwindows();
    mainFrame->Resize();
    mainFrame->MapWindow();
    browser->StopEmbedding();
    browser->SetTabTitle("Recon", 0);
}

EDep::TGUIManager::~TGUIManager() { }
