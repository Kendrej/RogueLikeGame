#pragma once

#include <zmq.hpp>

class AiEnvironment;

// Serwer REP: odpowiedz wychodzi dopiero po wykonaniu calego kroku.
class AiServer
{
public:
    AiServer(int port, int frameSkip, float dt);

    enum class Result
    {
        Idle,
        Handled,
        Quit
    };

    // Obsluguje najwyzej jedno zadanie, czekajac na nie maksymalnie timeoutMs
    Result poll(AiEnvironment& env, int timeoutMs);

private:
    zmq::context_t context_;
    zmq::socket_t  socket_;
    int            frameSkip_;
    float          dt_;
};
