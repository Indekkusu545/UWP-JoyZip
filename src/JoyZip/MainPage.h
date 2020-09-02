#pragma once

#include "MainPage.g.h"

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::JoyZip::implementation {
    struct MainPage : MainPageT<MainPage> {
        MainPage();
        fire_and_forget archiveButton_Click(IInspectable const& sender, RoutedEventArgs const& e);
        fire_and_forget unarchiveButton_Click(IInspectable const& sender, RoutedEventArgs const& e);
    };
}

namespace winrt::JoyZip::factory_implementation {
    struct MainPage : MainPageT<MainPage, implementation::MainPage> { };
}
