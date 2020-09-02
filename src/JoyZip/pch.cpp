#include "pch.h"
#include "winrt/Windows.ApplicationModel.Core.h"
#include "winrt/Windows.UI.ViewManagement.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::ViewManagement;

void makeTitleBarAcrylic() {
    auto coreTitleBar = CoreApplication::GetCurrentView().TitleBar();
    coreTitleBar.ExtendViewIntoTitleBar(true);
    auto titleBar = ApplicationView::GetForCurrentView().TitleBar();
    titleBar.ButtonBackgroundColor(Colors::Transparent());
    titleBar.ButtonInactiveBackgroundColor(Colors::Transparent());
}

hstring toFriendlySize(uint64_t const& size)
{
    hstring units[] = {L"B", L"KB", L"MB", L"GB", L"TB"};
    int index = 0;
    double dsize = size;
    while (dsize >= 1000) {
        dsize /= 1024;
        ++index;
    }
    return to_hstring(floor(dsize * 100) / 100) + units[index];
}
