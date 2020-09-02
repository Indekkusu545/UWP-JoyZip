#pragma once
winrt::Windows::Foundation::IAsyncOperation<float> compress(winrt::Windows::Storage::StorageFile const& sourceFile, winrt::Windows::Storage::StorageFile const& saveFile);
winrt::Windows::Foundation::IAsyncOperation<float> decompress(winrt::Windows::Storage::StorageFile const& sourceFile, winrt::Windows::Storage::StorageFolder const& saveFolder);