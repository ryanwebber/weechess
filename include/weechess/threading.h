#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <thread>
#include <vector>

namespace weechess::threading {

class Token {
public:
    Token();
    Token(const Token&) = delete;
    Token(Token&&) = delete;
    Token& operator=(const Token&) = delete;
    Token& operator=(Token&&) = delete;

    bool invalidate();
    bool invalidated() const;

    bool reset();

private:
    std::atomic<bool> m_invalidated;
};

class JoinThread {
private:
    std::shared_ptr<Token> m_token;
    std::future<void> m_future;
    std::thread m_thread;

public:
    template <typename... Args>
    JoinThread(std::packaged_task<void(std::shared_ptr<Token>, Args...)> task, Args&&... args)
        : m_token(std::make_shared<Token>())
        , m_future(task.get_future())
        , m_thread(std::move(task), m_token, std::forward<Args>(args)...)
    {
    }

    JoinThread(const JoinThread&) = delete;
    JoinThread(JoinThread&&) = delete;
    JoinThread& operator=(const JoinThread&) = delete;
    JoinThread& operator=(JoinThread&&) = delete;

    ~JoinThread()
    {
        m_token->invalidate();
        if (m_thread.joinable())
            m_thread.join();
    }

    void invalidate() { m_token->invalidate(); }
    bool invalidated() const { return m_token->invalidated(); }

    bool joinable() const { return m_thread.joinable(); }
    void join() { m_thread.join(); }

    bool valid() const { return m_future.valid(); }
    bool finished() const { return m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }
};

class ThreadDispatcher {
private:
    std::vector<std::unique_ptr<JoinThread>> m_threads;

    void maintenance()
    {
        auto erase_begin = std::remove_if(m_threads.begin(), m_threads.end(), [](const auto& thread) {
            return !thread->joinable() || thread->invalidated();
        });

        for (auto it = erase_begin; it != m_threads.end(); ++it) {
            (*it)->join();
        }

        m_threads.erase(erase_begin, m_threads.end());
    }

public:
    ThreadDispatcher() = default;

    template <typename F, typename... Args> void dispatch(F&& f, Args&&... args)
    {
        std::packaged_task<void(std::shared_ptr<Token>, Args...)> task(f);
        m_threads.push_back(std::make_unique<JoinThread>(std::move(task), std::forward<Args>(args)...));
    }

    void invalidate_all()
    {
        maintenance();
        for (auto& thread : m_threads) {
            thread->invalidate();
        }
    }

    void join_all()
    {
        maintenance();
        for (auto& thread : m_threads) {
            if (thread->joinable())
                thread->join();
        }
    }
};
}
