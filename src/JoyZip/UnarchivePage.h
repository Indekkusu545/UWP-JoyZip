#pragma once

#include "UnarchivePage.g.h"
#include "winrt/Windows.Storage.h"
#include "winrt/Windows.UI.WindowManagement.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI;
using namespace Windows::UI::WindowManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::JoyZip::implementation {
    struct UnarchivePage : UnarchivePageT<UnarchivePage> {
        AppWindow window;
        StorageFile sourceFile;
        StorageFolder saveFolder;

        UnarchivePage();
        void OnNavigatedTo(NavigationEventArgs const& e);
        fire_and_forget filePickerButton_Click(IInspectable const& sender, RoutedEventArgs const& e);
        fire_and_forget unarchiveFolderLink_Click(IInspectable const& sender, RoutedEventArgs const& e);
        fire_and_forget unarchiveLaunchButton_Click(IInspectable const& sender, RoutedEventArgs const& e);
        fire_and_forget CancelButton_Click(IInspectable const& sender, RoutedEventArgs const& e);
    };
}

namespace winrt::JoyZip::factory_implementation {
    struct UnarchivePage : UnarchivePageT<UnarchivePage, implementation::UnarchivePage> {};
}
