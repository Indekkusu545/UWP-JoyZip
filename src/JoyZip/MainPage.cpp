#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "winrt/Windows.UI.Xaml.Hosting.h"
#include "winrt/Windows.UI.WindowManagement.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::WindowManagement;

namespace winrt::JoyZip::implementation {
    MainPage::MainPage() {
        InitializeComponent();
    }

    fire_and_forget MainPage::archiveButton_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        auto appWindow = co_await AppWindow::TryCreateAsync();
        auto appWindowContentFrame = Frame::Frame();
        appWindowContentFrame.Navigate(xaml_typename<JoyZip::CreatArchivePage>(), box_value(appWindow));
        ElementCompositionPreview::SetAppWindowContent(appWindow, appWindowContentFrame);
        auto titleBar = appWindow.TitleBar();
        titleBar.ExtendsContentIntoTitleBar(true);
        titleBar.ButtonBackgroundColor(Colors::Transparent());
        titleBar.ButtonInactiveBackgroundColor(Colors::Transparent());
        co_await appWindow.TryShowAsync();
    }

    fire_and_forget MainPage::unarchiveButton_Click(IInspectable const& sender, RoutedEventArgs const& e) {
        auto appWindow = co_await AppWindow::TryCreateAsync();
        auto appWindowContentFrame = Frame::Frame();
        appWindowContentFrame.Navigate(xaml_typename<JoyZip::UnarchivePage>(), box_value(appWindow));
        ElementCompositionPreview::SetAppWindowContent(appWindow, appWindowContentFrame);

        auto titleBar = appWindow.TitleBar();
        titleBar.ExtendsContentIntoTitleBar(true);
        titleBar.ButtonBackgroundColor(Colors::Transparent());
        titleBar.ButtonInactiveBackgroundColor(Colors::Transparent());
        co_await appWindow.TryShowAsync();
    }

}