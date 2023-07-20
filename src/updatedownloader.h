/*
 *  This file is part of WinSparkle (https://winsparkle.org)
 *
 *  Copyright (C) 2012-2021 Vaclav Slavik
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _updatedownloader_h_
#define _updatedownloader_h_

#include "threads.h"
#include "appcast.h"
#include "download.h"
#include "ui.h"

#include <string>
#include <time.h>

namespace winsparkle
{
    // helper function
    std::wstring CreateUniqueTempDirectory();

    /**
        This class performs application update.

        If an update is found, then UpdateChecker initializes the GUI thread
        and shows information about available update to the user.
     */
    class UpdateDownloader : public Thread
    {
    public:
        /// Creates updater thread.
        UpdateDownloader(const Appcast& appcast);

        /**
            Perform any necessary cleanup after previous updates.

            Should be called on launch to get rid of leftover junk from previous
            updates, such as the installer files. Call it as soon as possible,
            before using other WinSparkle functionality.
         */
        static void CleanLeftovers();

    protected:
        // Thread methods:
        virtual void Run();
        virtual bool IsJoinable() const { return true; }

    private:
        Appcast m_appcast;
    };


    struct UpdateDownloadSink : public IDownloadSink
    {
        UpdateDownloadSink(Thread& thread, const std::wstring& dir);
        ~UpdateDownloadSink();

        void Close();
        std::wstring GetFilePath(void);
        virtual void SetLength(size_t l);
        virtual void SetFilename(const std::wstring& filename);
        virtual void Add(const void* data, size_t len);

        Thread& m_thread;
        size_t m_downloaded, m_total;
        FILE* m_file;
        std::wstring m_dir;
        std::wstring m_path;
        clock_t m_lastUpdate;
    };
} // namespace winsparkle

#endif // _updatedownloader_h_
