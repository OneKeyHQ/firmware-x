#include <iostream>
#include <thread>

// #include "onekey_usb.hpp"

#include "onekey_usb_bootloader_mode.hpp"
#include "onekey_usb_firmware_mode.hpp"

int main()
{
    onekey::usb::bootloader::BOOTLOADER okusb_bl;
    okusb_bl.usb_timeout_ms = 1000;

    // thread control vars
    std::atomic_bool thread_run_status = false;
    std::atomic_bool thread_run_control = false;

    // start thread then detach
    thread_run_control = true;
    std::thread thread_h = std::thread(
        &onekey::usb::bootloader::BOOTLOADER::usb_service_routine, &okusb_bl, std::ref(thread_run_control),
        std::ref(thread_run_status)
    );
    thread_h.detach();

    bzh_utils::log::StdLog::Debug("thread_run_control -> " + std::string(thread_run_control ? "true" : "false"));

    // wait for running
    while ( !thread_run_status ) {};

    // wait for Xs
    auto counter = 0;
    while ( thread_run_status )
    {
        std::this_thread::sleep_for(1000ms);
        counter++;
        bzh_utils::log::StdLog::Debug("Thread running count -> ", std::to_string(counter));
        if ( counter >= 20)
        {
            break;
        }
    }

    // stop thread
    thread_run_control = false;
    counter = 0;
    while ( thread_run_status )
    {
        std::this_thread::sleep_for(1000ms);
        counter++;
        bzh_utils::log::StdLog::Debug("Thread stopping count -> ", std::to_string(counter));
    }

    return 0;
}
