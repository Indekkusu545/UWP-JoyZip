#include "pch.h"
#include "UnarchivePage.h"
#if __has_include("UnarchivePage.g.cpp")
#include "UnarchivePage.g.cpp"
#endif
#include "winrt/Windows.Storage.h"
#include "winrt/Windows.Storage.Pickers.h"
#include "winrt/Windows.Storage.Provider.h"
#include "winrt/Windows.Storage.FileProperties.h"
#include "winrt/Windows.UI.WindowManagement.h"
#include "winrt/Windows.UI.Xaml.Media.Imaging.h"
#include "Huffman.h"
#include <chrono>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::FileProperties;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::Provider;
using namespace Windows::UI::WindowManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media::Imaging;

namespace winrt::JoyZip::implementation {
    UnarchivePage::UnarchivePage() :
        sourceFile(nullptr),
        saveFolder(nullptr),
        window(nullptr) {
        InitializeComponent();
    }

    void UnarchivePage::OnNavigatedTo(NavigationEventArgs const& e) {
        window = unbox_value<AppWindow>(e.Parameter());
        __super::OnNavigatedTo(e);
    }

    fire_and_forget UnarchivePage::filePickerButton_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        auto picker = FileOpenPicker();
        picker.ViewMode(PickerViewMode::List);
        picker.SuggestedStartLocation(PickerLocationId::Desktop);
        picker.FileTypeFilter().Append(L".huff");
        sourceFile = co_await picker.PickSingleFileAsync();
        if (sourceFile != nullptr) {
            auto thumbnail = BitmapImage();
            thumbnail.SetSource(co_await sourceFile.GetThumbnailAsync(ThumbnailMode::SingleItem));
            fileThumb().Source(thumbnail);
            fileNameBlock().Text(sourceFile.Name());
            fileSizeBlock().Text(toFriendlySize((co_await sourceFile.GetBasicPropertiesAsync()).as<BasicProperties>().Size()));
            filePathBlock().Text(sourceFile.Path());
            if (saveFolder == nullptr) {
                saveFolder = co_await sourceFile.GetParentAsync();
                unarchiveFolderLink().Content(box_value(saveFolder.Name()));
                unarchiveFolderLinkFull().Content(box_value(saveFolder.Path()));
            }
        }
    }

    fire_and_forget UnarchivePage::unarchiveFolderLink_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        auto picker = FolderPicker();
        picker.FileTypeFilter().Append(L"*");
        picker.SuggestedStartLocation(PickerLocationId::Desktop);
        saveFolder = co_await picker.PickSingleFolderAsync();
        if (saveFolder != nullptr) {
            unarchiveFolderLink().Content(box_value(saveFolder.Name()));
            unarchiveFolderLinkFull().Content(box_value(saveFolder.Path()));
        }
    }

    fire_and_forget UnarchivePage::unarchiveLaunchButton_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        if (sourceFile == nullptr || saveFolder == nullptr) {
            Primitives::FlyoutBase::ShowAttachedFlyout(sender.as<FrameworkElement>());
            return;
        }
        MainContentPanel().Visibility(Visibility::Collapsed);
        actionPanel().Visibility(Visibility::Collapsed);
        progressBar().Visibility(Visibility::Visible);
        progressBar().IsIndeterminate(true);
        auto start = std::chrono::high_resolution_clock().now();
        auto rate = co_await decompress(sourceFile, saveFolder);
        auto end = std::chrono::high_resolution_clock().now();
        progressBar().Value(100);
        progressBar().Visibility(Visibility::Collapsed);
        resultRate().Text(L"完整率：" + to_hstring(rate));
        resultTime().Text(L"用时：" + to_hstring(std::chrono::duration_cast<std::chrono::seconds>(end - start).count()) + L"s");
        resultPanel().Visibility(Visibility::Visible);
    }

    fire_and_forget UnarchivePage::CancelButton_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        co_await window.CloseAsync();
    }

}