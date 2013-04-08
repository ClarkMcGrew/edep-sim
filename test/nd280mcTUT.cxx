#include <tut.h>
#include <tut_reporter.h>

#include <iostream>
#include <string>
#include <cstdlib>

namespace tut {
    test_runner_singleton runner;
}

int main(int argc,const char* argv[]) {
    tut::reporter visi;
    enum {errorFound, doRegression, doList, doGroup} command;
    std::string group;
    int test = -1;

    command = errorFound;

    // Validate the input.
    if (argc < 2) command = doRegression;
    else if (argc == 2) {
        if (std::string(argv[1]) == "regression") command = doRegression;
        else if (std::string(argv[1]) == "list") command = doList;
        else {
            command = doGroup;
            group = argv[1];
        }
    }
    else if (argc == 3) {
        command = doGroup;
        group = argv[1];
        test = ::atoi(argv[2]);
    }
    else command = errorFound;

    if (command == errorFound) {
        std::cout << "A test harness for TUT based test." << std::endl;
        std::cout << "Usage: " << argv[0] 
                  << " [regression] | [list] | [ group] [test-number]"
                  << std::endl;
        std::cout << "       list        -- List all groups" << std::endl;
        std::cout << "       regression  -- run all tests" << std::endl;
        std::cout << "       group       -- run one group" << std::endl;
        std::cout << "       group <int> -- run one test in a group." 
                  << std::endl;
        exit (1);
    }
    
    tut::runner.get().set_callback(&visi);
    
    try {
        if (command == doRegression) {
            tut::runner.get().run_tests();
        }
        else if (command == doList) {
            std::cout << "registered test groups:" << std::endl;
            tut::groupnames gl = tut::runner.get().list_groups();
            tut::groupnames::const_iterator i = gl.begin();
            tut::groupnames::const_iterator e = gl.end();
            while( i != e ) {
                std::cout << "  " << *i << std::endl;
                ++i;
            }
        }
        else if (command == doGroup) {
            if (test<0) tut::runner.get().run_tests(group);
            else tut::runner.get().run_test(group,test);
        }
    }
    catch( const std::exception& ex ) {
        std::cerr << "Exception raised: " << ex.what() << std::endl;
    }
    
    if (!visi.all_ok()) exit(1);

    return 0;
}
