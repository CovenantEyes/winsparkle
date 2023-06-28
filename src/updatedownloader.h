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
    // helper functions
    std::wstring GetUniqueTempDirectoryPrefix();
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
        UpdateDownloadSink(Thread& thread, const std::wstring& dir)
            : m_thread(thread),
            m_dir(dir), m_file(NULL),
            m_downloaded(0), m_total(0), m_lastUpdate(-1)
        {}

        ~UpdateDownloadSink() { Close(); }

        void Close()
        {
            if (m_file)
            {
                fclose(m_file);
                m_file = NULL;
            }
        }

        std::wstring GetFilePath(void) { return m_path; }

        virtual void SetLength(size_t l) { m_total = l; }

        virtual void SetFilename(const std::wstring& filename)
        {
            if (m_file)
                throw std::runtime_error("Update file already set");

            m_path = m_dir + L"\\" + filename;
            m_file = _wfopen(m_path.c_str(), L"wb");
            if (!m_file)
                throw std::runtime_error("Cannot save update file");
        }

        virtual void Add(const void* data, size_t len)
        {
            if (!m_file)
                throw std::runtime_error("Filename is not net");

            m_thread.CheckShouldTerminate();

            if (fwrite(data, len, 1, m_file) != 1)
                throw std::runtime_error("Cannot save update file");
            m_downloaded += len;

            // only update at most 10 times/sec so that we don't flood the UI:
            clock_t now = clock();
            if (now == -1 || m_downloaded == m_total ||
                ((double(now - m_lastUpdate) / CLOCKS_PER_SEC) >= 0.1))
            {
                UI::NotifyDownloadProgress(m_downloaded, m_total);
                m_lastUpdate = now;
            }
        }

        Thread& m_thread;
        size_t m_downloaded, m_total;
        FILE* m_file;
        std::wstring m_dir;
        std::wstring m_path;
        clock_t m_lastUpdate;
    };
} // namespace winsparkle

#endif // _updatedownloader_h_
