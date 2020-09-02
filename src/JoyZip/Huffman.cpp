#include "pch.h"
#include "Huffman.h"
#include "winrt/Windows.Storage.h"
#include "winrt/Windows.Storage.Streams.h"

using namespace std;
using namespace winrt;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation;

#define MAXLEN 512+5
#define ASCLLNUM 256

struct HuffNode {
    int parent, lchild, rchild;
    unsigned long count;
    unsigned char alpha;
    char code[MAXLEN];
};

struct Ascll {
    unsigned char alpha;
    unsigned long count;
};

void select(HuffNode* HT, int i, int* s1, int* s2) {
    unsigned int s;
    s = 0;
    for (unsigned int j = 1; j <= i; ++j) {
        if (HT[j].parent == 0) {
            if (s == 0)
                s = j;
            if (HT[j].count < HT[s].count)
                s = j;
        }
    }
    *s1 = s;
    s = 0;
    for (unsigned int j = 1; j <= i; ++j) {
        if (HT[j].parent == 0 && j != *s1) {
            if (s == 0)
                s = j;
            if (HT[j].count < HT[s].count)
                s = j;
        }
    }
    *s2 = s;
}

int creatHuffmanTree(HuffNode* HT, Ascll* ascll) {
    int s1, s2, leafNum = 0, j = 0;
    for (int i = 0; i < 256; ++i) {
        if (ascll[i].count > 0) {
            HT[++j].count = ascll[i].count;
            HT[j].alpha = ascll[i].alpha;
            HT[j].parent = HT[j].lchild = HT[j].rchild = 0;
        }
    }
    leafNum = j;
    int nodeNum = 2 * leafNum - 1;
    for (int i = leafNum + 1; i <= nodeNum; ++i) {
        HT[i].count = 0;
        HT[i].code[0] = 0;
        HT[i].parent = HT[i].lchild = HT[i].rchild = 0;
    }
    for (int i = leafNum + 1; i <= nodeNum; ++i) {
        select(HT, i - 1, &s1, &s2);
        HT[s1].parent = i;
        HT[s2].parent = i;
        HT[i].lchild = s2;
        HT[i].rchild = s1;
        HT[i].count = HT[s1].count + HT[s2].count;
    }
    return leafNum;
}

void HuffmanCoding(string* hTable, HuffNode* HT, int leafNum) {
    int j, m, start;
    char cd[MAXLEN];
    m = MAXLEN;
    cd[m - 1] = 0;
    for (int i = 1; i <= leafNum; i++) {
        start = m - 1;
        for (int c = i, f = HT[c].parent; f != 0; c = f, f = HT[f].parent) {
            if (HT[f].lchild == c)
                cd[start--] = '0';
            else
                cd[start--] = '1';
        }
        ++start;
        for (j = 0; j < m - start; ++j)
            HT[i].code[j] = cd[start + j];
        HT[i].code[j] = '\0';
        hTable[HT[i].alpha] = HT[i].code;
    }
}

IAsyncOperation<float> compress(StorageFile const& sourceFile, StorageFile const& saveFile) {
    auto inStream = co_await sourceFile.OpenAsync(FileAccessMode::Read);
    auto inStreamSize = inStream.Size();
    auto inputStream = inStream.GetInputStreamAt(0);
    auto dataReader = DataReader(inputStream);
    auto readerBytesLoaded = co_await dataReader.LoadAsync(inStreamSize);
    auto outStream = co_await saveFile.OpenAsync(FileAccessMode::ReadWrite);
    auto outputStream = outStream.GetOutputStreamAt(0);
    auto dataWriter = DataWriter(outputStream);
    unsigned char c = '\0';
    int k = 0;
    unsigned long total = 0;
    Ascll ascll[ASCLLNUM];
    for (int i = 0; i < ASCLLNUM; ++i)
        ascll[i].count = 0;
    while (dataReader.UnconsumedBufferLength()) {
        c = dataReader.ReadByte();
        ascll[c].alpha = c;
        ascll[c].count++;
        ++total;
    }
    HuffNode HT[MAXLEN];
    int leafNum = creatHuffmanTree(HT, ascll);
    string hTable[MAXLEN];
    HuffmanCoding(hTable, HT, leafNum);
    auto format = to_string(sourceFile.FileType());
    format.resize(8);
    dataWriter.WriteString(to_hstring(format));
    dataWriter.WriteUInt32(total);
    for (int i = 0; i <= 255; ++i)
        dataWriter.WriteUInt32(ascll[i].count);
    unsigned long j = 0;
    string buf = "\0";
    int charNum = 1036;
    dataReader.Close();
    inputStream = inStream.GetInputStreamAt(0);
    dataReader = DataReader(inputStream);
    readerBytesLoaded = co_await dataReader.LoadAsync(inStreamSize);
    while (dataReader.UnconsumedBufferLength()) {
        c = dataReader.ReadByte();
        string tempCode = hTable[c];
        ++j;
        buf += tempCode;
        k = buf.length();
        c = 0;
        while (k >= 8) {
            for (int i = 0; i < 8; i++) {
                if (buf[i] == '1')
                    c = (c << 1) | 1;
                else
                    c = c << 1;
            }
            dataWriter.WriteByte(c);
            ++charNum;
            buf.erase(0, 8);
            k = buf.length();
        }
        if (j == total)
            break;
    }
    if (k > 0) {
        buf += "00000000";
        for (int i = 0; i < 8; i++) {
            if (buf[i] == '1')
                c = (c << 1) | 1;
            else
                c = c << 1;
        }
        dataWriter.WriteByte(c);
        ++charNum;
    }
    dataReader.Close();
    co_await dataWriter.StoreAsync();
    co_await outputStream.FlushAsync();
    float rate;
    rate = (float)charNum / (float)total;
    co_return rate;
}

IAsyncOperation<float> decompress(StorageFile const& sourceFile, StorageFolder const& saveFolder) {
    auto inStream = co_await sourceFile.OpenAsync(FileAccessMode::Read);
    auto inStreamSize = inStream.Size();
    auto inputStream = inStream.GetInputStreamAt(0);
    auto dataReader = DataReader(inputStream);
    auto readerBytesLoaded = co_await dataReader.LoadAsync(inStreamSize);
    auto format = string();
    for (int i = 0; i < 8; ++i) {
        char c = dataReader.ReadByte();
        if (c != '\0')
            format.push_back(c);
    }
    auto saveFile = co_await saveFolder.CreateFileAsync(sourceFile.DisplayName() + to_hstring(format));
    auto outStream = co_await saveFile.OpenAsync(FileAccessMode::ReadWrite);
    auto outputStream = outStream.GetOutputStreamAt(0);
    auto dataWriter = DataWriter(outputStream);
    unsigned long total = dataReader.ReadUInt32();
    Ascll ascll[ASCLLNUM];
    for (int i = 0; i < ASCLLNUM; ++i) {
        ascll[i].count = dataReader.ReadUInt32();
        ascll[i].alpha = i;
    }
    HuffNode HT[MAXLEN];
    int leafNum = creatHuffmanTree(HT, ascll);
    unsigned char c = 0;
    int index = 2 * leafNum - 1;
    int charNum = 0;
    while (dataReader.UnconsumedBufferLength()) {
        c = dataReader.ReadByte();
        for (int i = 0; i < 8; ++i) {
            unsigned int cod = (c & 128);
            c = c << 1;
            if (cod == 0)
                index = HT[index].lchild;
            else
                index = HT[index].rchild;
            if (HT[index].rchild == 0 && HT[index].lchild == 0) {
                ++charNum;
                char trueChar = HT[index].alpha;
                dataWriter.WriteByte(trueChar);
                index = 2 * leafNum - 1;
                if (charNum >= total)
                    break;
            }
        }
        if (charNum >= total)
            break;
    }
    dataReader.Close();
    co_await dataWriter.StoreAsync();
    co_await outStream.FlushAsync();
    float rate;
    rate = (float)charNum / (float)total;
    co_return rate;
}