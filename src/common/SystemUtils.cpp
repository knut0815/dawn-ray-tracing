// Copyright 2019 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "common/SystemUtils.h"
#include "common/Assert.h"

#if defined(DAWN_PLATFORM_WINDOWS)
#    include <Windows.h>
#    include <vector>
#elif defined(DAWN_PLATFORM_LINUX)
#    include <limits.h>
#    include <unistd.h>
#    include <cstdlib>
#elif defined(DAWN_PLATFORM_MACOS) || defined(DAWN_PLATFORM_IOS)
#    include <mach-o/dyld.h>
#    include <vector>
#endif

#include <array>

#if defined(DAWN_PLATFORM_WINDOWS)
const char* GetPathSeparator() {
    return "\\";
}

std::string GetEnvironmentVar(const char* variableName) {
    // First pass a size of 0 to get the size of variable value.
    char* tempBuf = nullptr;
    DWORD result = GetEnvironmentVariableA(variableName, tempBuf, 0);
    if (result == 0) {
        return "";
    }

    // Then get variable value with its actual size.
    std::vector<char> buffer(result + 1);
    if (GetEnvironmentVariableA(variableName, buffer.data(), static_cast<DWORD>(buffer.size())) ==
        0) {
        return "";
    }
    return std::string(buffer.data());
}

bool SetEnvironmentVar(const char* variableName, const char* value) {
    return SetEnvironmentVariableA(variableName, value) == TRUE;
}
#elif defined(DAWN_PLATFORM_POSIX)
const char* GetPathSeparator() {
    return "/";
}

std::string GetEnvironmentVar(const char* variableName) {
    char* value = getenv(variableName);
    return value == nullptr ? "" : std::string(value);
}

bool SetEnvironmentVar(const char* variableName, const char* value) {
    return setenv(variableName, value, 1) == 0;
}
#else
#    error "Implement Get/SetEnvironmentVar for your platform."
#endif

#if defined(DAWN_PLATFORM_WINDOWS)
std::string GetExecutablePath() {
    std::array<char, MAX_PATH> executableFileBuf;
    DWORD executablePathLen = GetModuleFileNameA(nullptr, executableFileBuf.data(),
                                                 static_cast<DWORD>(executableFileBuf.size()));
    return executablePathLen > 0 ? std::string(executableFileBuf.data()) : "";
}
#elif defined(DAWN_PLATFORM_LINUX)
std::string GetExecutablePath() {
    std::array<char, PATH_MAX> path;
    ssize_t result = readlink("/proc/self/exe", path.data(), PATH_MAX - 1);
    if (result < 0 || static_cast<size_t>(result) >= PATH_MAX - 1) {
        return "";
    }

    path[result] = '\0';
    return path.data();
}
#elif defined(DAWN_PLATFORM_MACOS) || defined(DAWN_PLATFORM_IOS)
std::string GetExecutablePath() {
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);

    std::vector<char> buffer(size + 1);
    if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
        return "";
    }

    buffer[size] = '\0';
    return buffer.data();
}
#elif defined(DAWN_PLATFORM_FUCHSIA)
std::string GetExecutablePath() {
    // TODO: Implement on Fuchsia
    return "";
}
#else
#    error "Implement GetExecutablePath for your platform."
#endif

std::string GetExecutableDirectory() {
    std::string exePath = GetExecutablePath();
    size_t lastPathSepLoc = exePath.find_last_of(GetPathSeparator());
    return lastPathSepLoc != std::string::npos ? exePath.substr(0, lastPathSepLoc + 1) : "";
}

#if defined(DAWN_PLATFORM_WINDOWS)
// Since kernel32 is always loaded, we use it to receive the module's handle
static const wchar_t kTagForGetModuleHandleEx[] = L"kernel32.dll";
std::string GetModulePath() {
    HMODULE moduleHandle;
    if (GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            kTagForGetModuleHandleEx, &moduleHandle) == 0) {
        return "";
    }
    char lpFilename[MAX_PATH];
    if (GetModuleFileNameA(moduleHandle, lpFilename, sizeof(lpFilename)) == 0) {
        return "";
    }
    return std::string(lpFilename);
}
#elif defined(DAWN_PLATFORM_LINUX)
std::string GetModulePath() {
    // TODO: Implement on Linux
    UNREACHABLE();
    return "";
}
#elif defined(DAWN_PLATFORM_MACOS) || defined(DAWN_PLATFORM_IOS)
std::string GetModulePath() {
    // TODO: Implement on MacOS/iOS
    UNREACHABLE();
    return "";
}
#elif defined(DAWN_PLATFORM_FUCHSIA)
std::string GetModulePath() {
    // TODO: Implement on Fuchsia
    UNREACHABLE();
    return "";
}
#else
#    error "Implement GetModulePath for your platform."
#endif

std::string GetModuleDirectory() {
    std::string moduleDirPath = GetModulePath();
    size_t lastPathSepLoc = moduleDirPath.find_last_of(GetPathSeparator());
    return lastPathSepLoc != std::string::npos ? moduleDirPath.substr(0, lastPathSepLoc + 1) : "";
}
