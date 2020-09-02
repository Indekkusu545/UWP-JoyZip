#include "pch.h"
#include "CreatArchivePage.h"
#if __has_include("CreatArchivePage.g.cpp")
#include "CreatArchivePage.g.cpp"
#endif
#include "winrt/Windows.Storage.h"
#include "winrt/Windows.Storage.Pickers.h"
#include "winrt/Windows.Storage.Provider.h"
#include "winrt/Windows.Storage.FileProperties.h"
#include "winrt/Windows.System.h"
#include "winrt/Windows.UI.WindowManagement.h"
#include "winrt/Windows.UI.Xaml.Media.Imaging.h"
#include "huffman.h"
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
    CreatArchivePage::CreatArchivePage() :
        sourceFile(nullptr),
        saveFile(nullptr),
        window(nullptr) {
        InitializeComponent();
    }

    void CreatArchivePage::OnNavigatedTo(NavigationEventArgs const& e) {
        window = unbox_value<AppWindow>(e.Parameter());
        __super::OnNavigatedTo(e);
    }

    fire_and_forget CreatArchivePage::filePickerButton_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        auto picker = FileOpenPicker();
        picker.ViewMode(PickerViewMode::List);
        picker.SuggestedStartLocation(PickerLocationId::Desktop);
        picker.FileTypeFilter().Append(L"*");
        sourceFile = co_await picker.PickSingleFileAsync();
        if (sourceFile != nullptr) {
            auto thumbnail = BitmapImage();
            thumbnail.SetSource(co_await sourceFile.GetThumbnailAsync(ThumbnailMode::SingleItem));
            fileThumb().Source(thumbnail);
            fileNameBlock().Text(sourceFile.Name());
            fileSizeBlock().Text(toFriendlySize((co_await sourceFile.GetBasicPropertiesAsync()).as<BasicProperties>().Size()));
            filePathBlock().Text(sourceFile.Path());
        }
    }

    fire_and_forget CreatArchivePage::archiveFileLink_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        auto picker = FileSavePicker();
        picker.SuggestedStartLocation(PickerLocationId::Desktop);
        picker.FileTypeChoices().Insert(L"哈夫曼归档", IVector<hstring>{single_threaded_vector<hstring>({ L".huff" })});
        picker.SuggestedFileName(L"新建归档");
        saveFile = co_await picker.PickSaveFileAsync();
        saveFile.DeleteAsync();
        if (saveFile != nullptr) {
            archiveFileLink().Content(box_value(saveFile.Name()));
            archiveFileLinkFull().Content(box_value(saveFile.Path()));
        }
    }

    fire_and_forget CreatArchivePage::archiveLaunchButton_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        if (sourceFile == nullptr || saveFile == nullptr) {
            Primitives::FlyoutBase::ShowAttachedFlyout(sender.as<FrameworkElement>());
            co_return;
        }
        auto saveFolder = co_await saveFile.GetParentAsync();
        if (saveFolder == nullptr) {
            auto contentDialog = ContentDialog();
            contentDialog.Content(box_value(L"需要文件系统权限，点击确认前往授权。"));
            contentDialog.PrimaryButtonText(L"确认");
            contentDialog.XamlRoot(archiveLaunchButton().XamlRoot());
            auto result = co_await contentDialog.ShowAsync();
            if (result == ContentDialogResult::Primary)
                co_await Windows::System::Launcher::LaunchUriAsync(Uri(L"ms-settings:privacy-broadfilesystemaccess"));
            co_return;
        }
        MainContentPanel().Visibility(Visibility::Collapsed);
        actionPanel().Visibility(Visibility::Collapsed);
        progressBar().Visibility(Visibility::Visible);
        progressBar().IsIndeterminate(true);
        auto start = std::chrono::high_resolution_clock().now();
        saveFile = co_await saveFolder.CreateFileAsync(saveFile.Name());
        auto rate = co_await compress(sourceFile, saveFile);
        auto end = std::chrono::high_resolution_clock().now();
        progressBar().Value(100);
        progressBar().Visibility(Visibility::Collapsed);
        resultRate().Text(L"压缩率：" + to_hstring(rate));
        resultTime().Text(L"用时：" + to_hstring(std::chrono::duration_cast<std::chrono::seconds>(end - start).count()) + L"s");
        resultPanel().Visibility(Visibility::Visible);
    }

    fire_and_forget CreatArchivePage::CancelButton_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        co_await window.CloseAsync();
    }

    fire_and_forget CreatArchivePage::Page_Loaded(IInspectable const& sender, RoutedEventArgs const& e) {
        saveFile = co_await DownloadsFolder::CreateFileAsync(L"新建归档.huff");
        saveFile.DeleteAsync();
        if (saveFile != nullptr) {
            archiveFileLink().Content(box_value(saveFile.Name()));
            archiveFileLinkFull().Content(box_value(saveFile.Path()));
        }
    }

}