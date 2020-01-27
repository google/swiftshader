// Copyright 2019 The SwiftShader Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Server.hpp"

#include "Context.hpp"
#include "EventListener.hpp"
#include "File.hpp"
#include "Thread.hpp"
#include "Variable.hpp"

#include "dap/network.h"
#include "dap/protocol.h"
#include "dap/session.h"
#include "marl/waitgroup.h"

#include <thread>
#include <unordered_set>

// Switch for controlling DAP debug logging
#define ENABLE_DAP_LOGGING 0

#if ENABLE_DAP_LOGGING
#	define DAP_LOG(msg, ...) printf(msg "\n", ##__VA_ARGS__)
#else
#	define DAP_LOG(...) \
		do               \
		{                \
		} while(false)
#endif

namespace vk {
namespace dbg {

class Server::Impl : public Server, public EventListener
{
public:
	Impl(const std::shared_ptr<Context> &ctx, int port);
	~Impl();

	// EventListener
	void onThreadStarted(ID<Thread>) override;
	void onThreadStepped(ID<Thread>) override;
	void onLineBreakpointHit(ID<Thread>) override;
	void onFunctionBreakpointHit(ID<Thread>) override;

	dap::Scope scope(const char *type, Scope *);
	dap::Source source(File *);
	std::shared_ptr<File> file(const dap::Source &source);

	const std::shared_ptr<Context> ctx;
	const std::unique_ptr<dap::net::Server> server;
	const std::unique_ptr<dap::Session> session;
	std::atomic<bool> clientIsVisualStudio = { false };
};

Server::Impl::Impl(const std::shared_ptr<Context> &context, int port)
    : ctx(context)
    , server(dap::net::Server::create())
    , session(dap::Session::create())
{
	session->registerHandler([](const dap::DisconnectRequest &req) {
		DAP_LOG("DisconnectRequest receieved");
		return dap::DisconnectResponse();
	});

	session->registerHandler([&](const dap::InitializeRequest &req) {
		DAP_LOG("InitializeRequest receieved");
		dap::InitializeResponse response;
		response.supportsFunctionBreakpoints = true;
		response.supportsConfigurationDoneRequest = true;
		response.supportsEvaluateForHovers = true;
		clientIsVisualStudio = (req.clientID.value("") == "visualstudio");
		return response;
	});

	session->registerSentHandler(
	    [&](const dap::ResponseOrError<dap::InitializeResponse> &response) {
		    DAP_LOG("InitializeResponse sent");
		    session->send(dap::InitializedEvent());
	    });

	session->registerHandler([](const dap::SetExceptionBreakpointsRequest &req) {
		DAP_LOG("SetExceptionBreakpointsRequest receieved");
		dap::SetExceptionBreakpointsResponse response;
		return response;
	});

	session->registerHandler(
	    [this](const dap::SetFunctionBreakpointsRequest &req) {
		    DAP_LOG("SetFunctionBreakpointsRequest receieved");
		    auto lock = ctx->lock();
		    dap::SetFunctionBreakpointsResponse response;
		    for(auto const &bp : req.breakpoints)
		    {
			    DAP_LOG("Setting breakpoint for function '%s'", bp.name.c_str());
			    lock.addFunctionBreakpoint(bp.name.c_str());
			    response.breakpoints.push_back({});
		    }
		    return response;
	    });

	session->registerHandler(
	    [this](const dap::SetBreakpointsRequest &req)
	        -> dap::ResponseOrError<dap::SetBreakpointsResponse> {
		    DAP_LOG("SetBreakpointsRequest receieved");
		    bool verified = false;

		    size_t numBreakpoints = 0;
		    if(req.breakpoints.has_value())
		    {
			    auto const &breakpoints = req.breakpoints.value();
			    numBreakpoints = breakpoints.size();
			    if(auto file = this->file(req.source))
			    {
				    file->clearBreakpoints();
				    for(auto const &bp : breakpoints)
				    {
					    file->addBreakpoint(bp.line);
				    }
				    verified = true;
			    }
			    else if(req.source.name.has_value())
			    {
				    std::vector<int> lines;
				    lines.reserve(breakpoints.size());
				    for(auto const &bp : breakpoints)
				    {
					    lines.push_back(bp.line);
				    }
				    ctx->lock().addPendingBreakpoints(req.source.name.value(),
				                                      lines);
			    }
		    }

		    dap::SetBreakpointsResponse response;
		    for(size_t i = 0; i < numBreakpoints; i++)
		    {
			    dap::Breakpoint bp;
			    bp.verified = verified;
			    bp.source = req.source;
			    response.breakpoints.push_back(bp);
		    }
		    return response;
	    });

	session->registerHandler([this](const dap::ThreadsRequest &req) {
		DAP_LOG("ThreadsRequest receieved");
		auto lock = ctx->lock();
		dap::ThreadsResponse response;
		for(auto thread : lock.threads())
		{
			std::string name = thread->name();
			if(clientIsVisualStudio)
			{
				// WORKAROUND: https://github.com/microsoft/VSDebugAdapterHost/issues/15
				for(size_t i = 0; i < name.size(); i++)
				{
					if(name[i] == '.')
					{
						name[i] = '_';
					}
				}
			}

			dap::Thread out;
			out.id = thread->id.value();
			out.name = name;
			response.threads.push_back(out);
		};
		return response;
	});

	session->registerHandler(
	    [this](const dap::StackTraceRequest &req)
	        -> dap::ResponseOrError<dap::StackTraceResponse> {
		    DAP_LOG("StackTraceRequest receieved");

		    auto lock = ctx->lock();
		    auto thread = lock.get(Thread::ID(req.threadId));
		    if(!thread)
		    {
			    return dap::Error("Thread %d not found", req.threadId);
		    }

		    auto stack = thread->stack();

		    dap::StackTraceResponse response;
		    response.totalFrames = stack.size();
		    response.stackFrames.reserve(stack.size());
		    for(int i = static_cast<int>(stack.size()) - 1; i >= 0; i--)
		    {
			    auto const &frame = stack[i];
			    auto const &loc = frame.location;
			    dap::StackFrame sf;
			    sf.column = 0;
			    sf.id = frame.id.value();
			    sf.name = frame.function;
			    sf.line = loc.line;
			    if(loc.file)
			    {
				    sf.source = source(loc.file.get());
			    }
			    response.stackFrames.emplace_back(std::move(sf));
		    }
		    return response;
	    });

	session->registerHandler([this](const dap::ScopesRequest &req)
	                             -> dap::ResponseOrError<dap::ScopesResponse> {
		DAP_LOG("ScopesRequest receieved");

		auto lock = ctx->lock();
		auto frame = lock.get(Frame::ID(req.frameId));
		if(!frame)
		{
			return dap::Error("Frame %d not found", req.frameId);
		}

		dap::ScopesResponse response;
		response.scopes = {
			scope("locals", frame->locals.get()),
			scope("arguments", frame->arguments.get()),
			scope("registers", frame->registers.get()),
		};
		return response;
	});

	session->registerHandler([this](const dap::VariablesRequest &req)
	                             -> dap::ResponseOrError<dap::VariablesResponse> {
		DAP_LOG("VariablesRequest receieved");

		auto lock = ctx->lock();
		auto vars = lock.get(VariableContainer::ID(req.variablesReference));
		if(!vars)
		{
			return dap::Error("VariablesReference %d not found",
			                  int(req.variablesReference));
		}

		dap::VariablesResponse response;
		vars->foreach(req.start.value(0), req.count.value(~0), [&](const Variable &v) {
			dap::Variable out;
			out.evaluateName = v.name;
			out.name = v.name;
			out.type = v.value->type()->string();
			out.value = v.value->string();
			if(v.value->type()->kind == Kind::VariableContainer)
			{
				auto const vc = static_cast<const VariableContainer *>(v.value.get());
				out.variablesReference = vc->id.value();
			}
			response.variables.push_back(out);
		});
		return response;
	});

	session->registerHandler([this](const dap::SourceRequest &req)
	                             -> dap::ResponseOrError<dap::SourceResponse> {
		DAP_LOG("SourceRequest receieved");

		dap::SourceResponse response;
		uint64_t id = req.sourceReference;

		auto lock = ctx->lock();
		auto file = lock.get(File::ID(id));
		if(!file)
		{
			return dap::Error("Source %d not found", id);
		}
		response.content = file->source;
		return response;
	});

	session->registerHandler([this](const dap::PauseRequest &req)
	                             -> dap::ResponseOrError<dap::PauseResponse> {
		DAP_LOG("PauseRequest receieved");

		dap::StoppedEvent event;
		event.reason = "pause";

		auto lock = ctx->lock();
		if(auto thread = lock.get(Thread::ID(req.threadId)))
		{
			thread->pause();
			event.threadId = req.threadId;
		}
		else
		{
			auto threads = lock.threads();
			for(auto thread : threads)
			{
				thread->pause();
			}
			event.allThreadsStopped = true;

			// Workaround for
			// https://github.com/microsoft/VSDebugAdapterHost/issues/11
			if(clientIsVisualStudio)
			{
				for(auto thread : threads)
				{
					event.threadId = thread->id.value();
					break;
				}
			}
		}

		session->send(event);

		dap::PauseResponse response;
		return response;
	});

	session->registerHandler([this](const dap::ContinueRequest &req)
	                             -> dap::ResponseOrError<dap::ContinueResponse> {
		DAP_LOG("ContinueRequest receieved");

		dap::ContinueResponse response;

		auto lock = ctx->lock();
		if(auto thread = lock.get(Thread::ID(req.threadId)))
		{
			thread->resume();
			response.allThreadsContinued = false;
		}
		else
		{
			for(auto it : lock.threads())
			{
				thread->resume();
			}
			response.allThreadsContinued = true;
		}

		return response;
	});

	session->registerHandler([this](const dap::NextRequest &req)
	                             -> dap::ResponseOrError<dap::NextResponse> {
		DAP_LOG("NextRequest receieved");

		auto lock = ctx->lock();
		auto thread = lock.get(Thread::ID(req.threadId));
		if(!thread)
		{
			return dap::Error("Unknown thread %d", int(req.threadId));
		}

		thread->stepOver();
		return dap::NextResponse();
	});

	session->registerHandler([this](const dap::StepInRequest &req)
	                             -> dap::ResponseOrError<dap::StepInResponse> {
		DAP_LOG("StepInRequest receieved");

		auto lock = ctx->lock();
		auto thread = lock.get(Thread::ID(req.threadId));
		if(!thread)
		{
			return dap::Error("Unknown thread %d", int(req.threadId));
		}

		thread->stepIn();
		return dap::StepInResponse();
	});

	session->registerHandler([this](const dap::StepOutRequest &req)
	                             -> dap::ResponseOrError<dap::StepOutResponse> {
		DAP_LOG("StepOutRequest receieved");

		auto lock = ctx->lock();
		auto thread = lock.get(Thread::ID(req.threadId));
		if(!thread)
		{
			return dap::Error("Unknown thread %d", int(req.threadId));
		}

		thread->stepOut();
		return dap::StepOutResponse();
	});

	session->registerHandler([this](const dap::EvaluateRequest &req)
	                             -> dap::ResponseOrError<dap::EvaluateResponse> {
		DAP_LOG("EvaluateRequest receieved");

		auto lock = ctx->lock();
		if(req.frameId.has_value())
		{
			auto frame = lock.get(Frame::ID(req.frameId.value(0)));
			if(!frame)
			{
				return dap::Error("Unknown frame %d", int(req.frameId.value()));
			}

			auto fmt = FormatFlags::Default;
			auto subfmt = FormatFlags::Default;

			if(req.context.value("") == "hover")
			{
				subfmt.listPrefix = "\n";
				subfmt.listSuffix = "";
				subfmt.listDelimiter = "\n";
				subfmt.listIndent = "  ";
				fmt.listPrefix = "";
				fmt.listSuffix = "";
				fmt.listDelimiter = "\n";
				fmt.listIndent = "";
				fmt.subListFmt = &subfmt;
			}

			dap::EvaluateResponse response;
			auto findHandler = [&](const Variable &var) {
				response.result = var.value->string(fmt);
				response.type = var.value->type()->string();
			};

			std::vector<std::shared_ptr<vk::dbg::VariableContainer>> variables = {
				frame->locals->variables,
				frame->arguments->variables,
				frame->registers->variables,
				frame->hovers->variables,
			};

			for(auto const &vars : variables)
			{
				if(vars->find(req.expression, findHandler)) { return response; }
			}

			// HACK: VSCode does not appear to include the % in %123 tokens
			// TODO: This might be a configuration problem of the SPIRV-Tools
			// spirv-ls plugin. Investigate.
			auto withPercent = "%" + req.expression;
			for(auto const &vars : variables)
			{
				if(vars->find(withPercent, findHandler)) { return response; }
			}
		}

		return dap::Error("Could not evaluate expression");
	});

	session->registerHandler([](const dap::LaunchRequest &req) {
		DAP_LOG("LaunchRequest receieved");
		return dap::LaunchResponse();
	});

	marl::WaitGroup configurationDone(1);
	session->registerHandler([=](const dap::ConfigurationDoneRequest &req) {
		DAP_LOG("ConfigurationDoneRequest receieved");
		configurationDone.done();
		return dap::ConfigurationDoneResponse();
	});

	server->start(port, [&](const std::shared_ptr<dap::ReaderWriter> &rw) {
		session->bind(rw);
		ctx->addListener(this);
	});

	static bool waitForDebugger = getenv("VK_WAIT_FOR_DEBUGGER") != nullptr;
	if(waitForDebugger)
	{
		printf("Waiting for debugger connection...\n");
		configurationDone.wait();
		printf("Debugger connection established\n");
	}
}

Server::Impl::~Impl()
{
	ctx->removeListener(this);
	server->stop();
}

void Server::Impl::onThreadStarted(ID<Thread> id)
{
	dap::ThreadEvent event;
	event.reason = "started";
	event.threadId = id.value();
	session->send(event);
}

void Server::Impl::onThreadStepped(ID<Thread> id)
{
	dap::StoppedEvent event;
	event.threadId = id.value();
	event.reason = "step";
	session->send(event);
}

void Server::Impl::onLineBreakpointHit(ID<Thread> id)
{
	dap::StoppedEvent event;
	event.threadId = id.value();
	event.reason = "breakpoint";
	session->send(event);
}

void Server::Impl::onFunctionBreakpointHit(ID<Thread> id)
{
	dap::StoppedEvent event;
	event.threadId = id.value();
	event.reason = "function breakpoint";
	session->send(event);
}

dap::Scope Server::Impl::scope(const char *type, Scope *s)
{
	dap::Scope out;
	// out.line = s->startLine;
	// out.endLine = s->endLine;
	out.source = source(s->file.get());
	out.name = type;
	out.presentationHint = type;
	out.variablesReference = s->variables->id.value();
	return out;
}

dap::Source Server::Impl::source(File *file)
{
	dap::Source out;
	out.name = file->name;
	if(file->isVirtual())
	{
		out.sourceReference = file->id.value();
	}
	else
	{
		out.path = file->path();
	}
	return out;
}

std::shared_ptr<File> Server::Impl::file(const dap::Source &source)
{
	auto lock = ctx->lock();
	if(source.sourceReference.has_value())
	{
		auto id = source.sourceReference.value();
		if(auto file = lock.get(File::ID(id)))
		{
			return file;
		}
	}

	auto files = lock.files();
	if(source.path.has_value())
	{
		auto path = source.path.value();
		std::shared_ptr<File> out;
		for(auto file : files)
		{
			if(file->path() == path)
			{
				out = file;
				break;
			}
		}
		return out;
	}

	if(source.name.has_value())
	{
		auto name = source.name.value();
		std::shared_ptr<File> out;
		for(auto file : files)
		{
			if(file->name == name)
			{
				out = file;
				break;
			}
		}
		return out;
	}

	return nullptr;
}

std::shared_ptr<Server> Server::create(const std::shared_ptr<Context> &ctx, int port)
{
	return std::make_shared<Server::Impl>(ctx, port);
}

}  // namespace dbg
}  // namespace vk