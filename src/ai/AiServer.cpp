#include "ai/AiServer.h"

#include <iostream>
#include <string>

#include "ai/AiEnvironment.h"

AiServer::AiServer(int port, int frameSkip, float dt)
    : context_(1), socket_(context_, zmq::socket_type::rep), frameSkip_(frameSkip), dt_(dt)
{
    socket_.set(zmq::sockopt::linger, 0);
    socket_.bind("tcp://127.0.0.1:" + std::to_string(port));
    std::cout << "[AI] Serwer uruchomiony na porcie " << port << std::endl;
}

AiServer::Result AiServer::poll(AiEnvironment& env, int timeoutMs)
{
    socket_.set(zmq::sockopt::rcvtimeo, timeoutMs);

    zmq::message_t request;
    if (!socket_.recv(request, zmq::recv_flags::none))
        return Result::Idle;

    const std::string command(static_cast<const char*>(request.data()), request.size());
    Result            result = Result::Handled;

    if (command.size() == 1 && command[0] >= '0' && command[0] < '0' + AiEnvironment::ACTION_COUNT)
    {
        env.step(command[0] - '0', frameSkip_, dt_);
    }
    else if (command == "reset")
    {
        env.reset();
    }
    else if (command == "quit")
    {
        result = Result::Quit;
    }
    else
    {
        socket_.send(zmq::buffer(std::string("unknown_command")), zmq::send_flags::none);
        return result;
    }

    const std::string reply = env.consumeState().dump();
    socket_.send(zmq::buffer(reply), zmq::send_flags::none);
    return result;
}
