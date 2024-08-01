#include "TBmonit.h"
#include "TBread.h"
#include "TBplotengine.h"
#include "TBdetector.h"
#include "TButility.h"
#include "TBconfig.h"

#include <chrono>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include <mach/mach.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>

#include <sys/types.h>
#include <sys/sysctl.h>

#include "TApplication.h"
#include "TSystem.h"

void GetFormattedRamInfo() {
    // Total physical memory
    int64_t physical_memory;
    size_t length = sizeof(physical_memory);
    sysctlbyname("hw.memsize", &physical_memory, &length, NULL, 0);
    double total_memory_GB = static_cast<double>(physical_memory) / (1024 * 1024 * 1024);

    // Memory usage by this process
    task_basic_info_data_t info;
    mach_msg_type_number_t info_count = TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &info_count) == KERN_SUCCESS) {
        double process_memory_GB = static_cast<double>(info.resident_size) / (1024 * 1024 * 1024);

        // system memory usage
        vm_size_t page_size;
        mach_port_t mach_port = mach_host_self();
        vm_statistics64_data_t vm_stats;
        mach_msg_type_number_t count = sizeof(vm_stats) / sizeof(natural_t);
        if (host_page_size(mach_port, &page_size) == KERN_SUCCESS &&
            host_statistics64(mach_port, HOST_VM_INFO, (host_info64_t)&vm_stats, &count) == KERN_SUCCESS) {
            double free_memory_GB = static_cast<double>(vm_stats.free_count * page_size) / (1024 * 1024 * 1024);
            double used_memory_GB = total_memory_GB - free_memory_GB;


            printf("%.1f GB / %.1f GB (%0.2f %%) | Current Process: %.2f MB (%.2f %%)",
              used_memory_GB, total_memory_GB, (used_memory_GB / total_memory_GB * 100),
              process_memory_GB * 1024., (process_memory_GB / total_memory_GB * 100));
        }
    }
}

int main(int argc, char *argv[])
{
  int fRunNum = 6034;
  int fMaxEvent = -1;
  int fMaxFile = -1;
  bool fIsLive = 1;

  // TApplication *theApp = new TApplication("app", &argc, argv);
  // theApp->SetReturnFromRun(true);

  // TButility util = TButility();
  // util.LoadMapping("../mapping/mapping_TB2024_v1.root");

  ANSI_CODE ANSI = ANSI_CODE();

  // 1 17 M11-T1-S 1 0 0 0
  // 1 18 M11-T2-S 1 0 0 0
  // 1 19 M11-T3-S 1 0 0 0
  // 1 20 M11-T4-S 1 0 0 0
  // 1 21 M11-T6-S 1 0 0 0
  // 1 22 M11-T7-S 1 0 0 0
  // 1 23 M11-T8-S 1 0 0 0
  // 1 24 M11-T9-S 1 0 0 0
  // 1 25 M11-T1-C 1 1 0 0
  // 1 26 M11-T2-C 1 1 0 0
  //
  // 3 1 M1-T1-C 1 1 0 0
  // 3 2 M1-T2-C 1 1 0 0
  // 3 3 M1-T3-C 1 1 0 0
  // 3 4 M1-T4-C 1 1 0 0


  std::vector<TBcid> aCID = {TBcid(1, 17), TBcid(3,4), TBcid(1, 26)};

  // std::vector<int> vecMIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  // std::vector<int> vecMIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  std::vector<int> vecMIDs = {1, 3};

  TBconfig config = TBconfig("./config_general.yml");

  TBplotengine plotter = TBplotengine(config.GetConfig()["ModuleConfig"], fRunNum, TButility());
  plotter.SetCase("single");
  plotter.SetMethod("IntADC");
  plotter.SetCID(aCID);
  plotter.init();
  //TBread<TBwaveform> readerWave = TBread<TBwaveform>(fRunNum, fMaxEvent, fMaxFile, fIsLive, "/Users/khwang/scratch/TB2024/dev_240715/TB2024/sample_data", vecMIDs);
  TBread<TBwaveform> readerWave = TBread<TBwaveform>(fRunNum, fMaxEvent, fMaxFile, fIsLive, "../sample_data", vecMIDs);

  while(1) {
    std::cout << readerWave.GetMaxEvent() << std::endl;

    readerWave.CheckNextFileExistence();

    int iLiveCurrentEvent = readerWave.GetLiveCurrentEvent();
    int iCurrentEvent = readerWave.GetCurrentEvent();
    int iMaxEvent = readerWave.GetLiveMaxEvent();
    // std::cout << iCurrentEvent << " " << iLiveCurrentEvent << " " << iMaxEvent << std::endl;

    //std::chrono::time_point time_begin = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point time_begin = std::chrono::system_clock::now();

    for (int i = iCurrentEvent; i < iMaxEvent; i++) {
      if (i > iCurrentEvent && i % 10 == 0) {

        //std::chrono::duration time_taken = std::chrono::system_clock::now() - time_begin; // delete
        std::chrono::seconds time_taken{std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - time_begin)}; // delete
        float percent_done = 1. * (float)(i - iCurrentEvent) / (float)(iMaxEvent - iCurrentEvent);
        //std::chrono::duration time_left = time_taken * (1 / percent_done - 1);
        std::chrono::duration<float, std::ratio<1>> time_left = time_taken * (1 / percent_done - 1);
        std::chrono::minutes minutes_left = std::chrono::duration_cast<std::chrono::minutes>(time_left);
        std::chrono::seconds seconds_left = std::chrono::duration_cast<std::chrono::seconds>(time_left - minutes_left);
        std::cout << "\r\033[F" //+ ANSI.HIGHLIGHTED_GREEN + ANSI.BLACK
                  << " " << i << " / " << iMaxEvent << " events  " << minutes_left.count() << ":";
        printf("%02d left (%.1f %%) | ", int(seconds_left.count()), percent_done * 100);
        GetFormattedRamInfo();

        std::cout << ANSI.END << std::endl;
      }


      TBevt<TBwaveform> aEvent;
      plotter.Fill(readerWave.GetAnEvent());
    }
    plotter.Update();
    // gSystem->ProcessEvents();
    // theApp->Run(true);

    // std::string input;
    // std::cout << ANSI.BOLD << "Want to load next file? (y/n): ";
    // std::cin >> input;

    // if (!(input == "y" || input == "Y"))
    //   break;
  }

  return 0;
}
