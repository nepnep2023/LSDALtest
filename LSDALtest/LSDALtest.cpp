// LSDALtest.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#pragma comment(lib, "Shlwapi.lib")

#include <filesystem>
#include <fstream>
#include <Windows.h>
#include <iostream>
#include <shlobj.h>
#include <stdio.h>
#include <tchar.h>
#include <pathcch.h>
#include <Shlwapi.h>

#pragma warning(disable : 4995)


namespace fs = std::filesystem;

int main()
{
    TCHAR rspath[MAX_PATH];
    SHGetSpecialFolderPath(NULL, rspath, CSIDL_PERSONAL, 0);
    //std::filesystem::path gtav = fs::current_path();
    fs::current_path(rspath);
    fs::current_path("./Rockstar Games/GTA V/Profiles");
    fs::path profiles = fs::current_path();

    fs::create_directory("./temp");

    //std::vector<std::string>files;
    class check {
    public:
        fs::path path = "";
        FILETIME date;


    };
    check checkList[30];
    int count = 0;

    for (const fs::directory_entry& dir : fs::recursive_directory_iterator(profiles)) {
        std::string pathname = dir.path().string();
        if (pathname.find("SGTA") != std::string::npos) {
            if (!dir.path().has_extension()) {
                fs::path cp = dir.path();
                cp.remove_filename();
                fs::current_path(cp);
                HANDLE hFile = CreateFile(
                    dir.path().string().c_str(), GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
                FILETIME ft;
                GetFileTime(hFile, NULL, NULL, &ft);
                CloseHandle(hFile);
                std::string fileName = dir.path().filename().string();
                std::string copyName = fileName + ".tmp";
                bool copy = CopyFile(pathname.c_str(), copyName.c_str(), true);
                fs::path direx = dir.path();
                fs::absolute(direx);
                direx.replace_extension(".tmp");
                checkList[count].path = direx;
                //checkList[count].date = ft;
                std::cout << checkList[count].path << "がリストに入った" << std::endl;
                if (copy) {
                    std::cout << "拡張子を変更できた" << std::endl;

                }
                DeleteFile(pathname.c_str());
                count++;
            }
        }
    }

    count = 0;
    int latest = 0;
    while (true) {
        if (checkList[count].path == "") {
            std::cout << "チェック処理終了" << std::endl;
            break;
        }
        if (count == 0) {
            FILETIME ft1, ft2;
            HANDLE hFile = CreateFile(
                checkList[count].path.string().c_str(), GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
            GetFileTime(hFile, NULL, NULL, &ft1);
            CloseHandle(hFile);
            count++;
            hFile = CreateFile(
                checkList[count].path.string().c_str(), GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
            GetFileTime(hFile, NULL, NULL, &ft2);
            CloseHandle(hFile);
            LONG lRet = CompareFileTime(&ft1, &ft2);
            if (lRet == 1) {
                std::string temp = checkList[0].path.stem().string();
                CopyFile(checkList[0].path.string().c_str(), temp.c_str(), true);
                DeleteFile(checkList[0].path.string().c_str());
                checkList[0].path.replace_extension("");
                latest = 0;
                std::cout << temp << "が新しいデータになる" << std::endl;
            }
            else if (lRet == -1) {
                std::string temp = checkList[1].path.stem().string();
                CopyFile(checkList[1].path.string().c_str(), temp.c_str(), true);
                DeleteFile(checkList[1].path.string().c_str());
                checkList[1].path.replace_extension("");
                latest = 1;
                std::cout << temp << "が新しいデータになる" << std::endl;
            }
        }
        else {
            FILETIME ft1, ft2;
            HANDLE hFile = CreateFile(
                checkList[latest].path.string().c_str(), GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);            
            GetFileTime(hFile, NULL, NULL, &ft1);
            CloseHandle(hFile);
            count++;
            hFile = CreateFile(
                checkList[count].path.string().c_str(), GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
            GetFileTime(hFile, NULL, NULL, &ft2);
            CloseHandle(hFile);
            LONG lRet = CompareFileTime(&ft1, &ft2);
            if (lRet == -1) {
                std::string temp = checkList[count].path.stem().string();
                CopyFile(checkList[count].path.string().c_str(), temp.c_str(), true);
                DeleteFile(checkList[count].path.string().c_str());
                checkList[count].path.replace_extension("");
                std::string fileName = checkList[latest].path.stem().string();
                std::string backName = fileName + ".tmp";
                CopyFile(checkList[latest].path.string().c_str(), backName.c_str(), true);
                DeleteFile(checkList[latest].path.string().c_str());
                checkList[latest].path.replace_extension(".tmp");
                latest = count;
                std::cout << temp << "が新しいデータになる" << std::endl;
            }
        }
        count++;
    }

    Sleep(10 * 1000);

    count = 0;
    bool done = 0;
    while (true) {
        if (checkList[count].path == "") {
            std::cout << "戻す処理は終了" << std::endl;
            break;
        }
        if (checkList[count].path.has_extension()) {
            std::string temp = checkList[count].path.string();
            std::string back = checkList[count].path.stem().string();
            done = CopyFile(temp.c_str(), back.c_str(), true);
            std::cout << checkList[count].path << "拡張子を戻す処理をしている" << std::endl;
            DeleteFile(checkList[count].path.string().c_str());
        }
        if (done) {
            std::cout << "戻せた" << std::endl;
        }
        count++;
    }

    for (const fs::directory_entry& dir : fs::recursive_directory_iterator(profiles)) {
        std::string pathname = dir.path().string();
        if (pathname.find("SGTA") != std::string::npos) {
            if (dir.path().extension() == ".tmp") {
                fs::path cp = dir.path();
                cp.remove_filename();
                fs::current_path(cp);
                std::string fileName = dir.path().filename().stem().string();;
                CopyFile(pathname.c_str(), fileName.c_str(), false);
                DeleteFile(pathname.c_str());
                std::cout << dir.path() << " の拡張子を削除した" << std::endl;
            }
        }
    }
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
