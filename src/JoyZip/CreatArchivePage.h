#pragma once

#include "CreatArchivePage.g.h"
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
    struct CreatArchivePage : CreatArchivePageT<CreatArchivePage> {
        AppWindow window;
        StorageFile sourceFile;
        StorageFile saveFile;
        CreatArchivePage();
        void OnNavigatedTo(NavigationEventArgs const& e);
        fire_and_forget filePickerButton_Click(IInspectable const& sender, RoutedEventArgs const& e);
        fire_and_forget archiveFileLink_Click(IInspectable const& sender, RoutedEventArgs const& e);
        fire_and_forget archiveLaunchButton_Click(IInspectable const& sender, RoutedEventArgs const& e);
        fire_and_forget CancelButton_Click(IInspectable const& sender, RoutedEventArgs const& e);
        fire_and_forget Page_Loaded(IInspectable const& sender, RoutedEventArgs const& e);
    };
}

namespace winrt::JoyZip::factory_implementation {
    struct CreatArchivePage : CreatArchivePageT<CreatArchivePage, implementation::CreatArchivePage> {};
}
