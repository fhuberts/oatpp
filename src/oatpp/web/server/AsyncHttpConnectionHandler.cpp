/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "./AsyncHttpConnectionHandler.hpp"

namespace oatpp { namespace web { namespace server {

AsyncHttpConnectionHandler::AsyncHttpConnectionHandler(const std::shared_ptr<HttpProcessor::Components>& components,
                                                       v_int32 threadCount)
  : m_executor(std::make_shared<oatpp::async::Executor>(threadCount))
  , m_components(components)
{
  m_executor->detach();
}

AsyncHttpConnectionHandler::AsyncHttpConnectionHandler(const std::shared_ptr<HttpProcessor::Components>& components,
                                                       const std::shared_ptr<oatpp::async::Executor>& executor)
  : m_executor(executor)
  , m_components(components)
{}

std::shared_ptr<AsyncHttpConnectionHandler> AsyncHttpConnectionHandler::createShared(const std::shared_ptr<HttpRouter>& router, v_int32 threadCount){
  return std::make_shared<AsyncHttpConnectionHandler>(router, threadCount);
}

std::shared_ptr<AsyncHttpConnectionHandler> AsyncHttpConnectionHandler::createShared(const std::shared_ptr<HttpRouter>& router, const std::shared_ptr<oatpp::async::Executor>& executor){
  return std::make_shared<AsyncHttpConnectionHandler>(router, executor);
}

void AsyncHttpConnectionHandler::setErrorHandler(const std::shared_ptr<handler::ErrorHandler>& errorHandler){
  m_components->errorHandler = errorHandler;
  if(!m_components->errorHandler) {
    m_components->errorHandler = handler::DefaultErrorHandler::createShared();
  }
}

void AsyncHttpConnectionHandler::addRequestInterceptor(const std::shared_ptr<handler::RequestInterceptor>& interceptor) {
  m_components->requestInterceptors->pushBack(interceptor);
}

void AsyncHttpConnectionHandler::handleConnection(const std::shared_ptr<IOStream>& connection,
                                                  const std::shared_ptr<const ParameterMap>& params)
{

  (void)params;

  connection->setOutputStreamIOMode(oatpp::data::stream::IOMode::NON_BLOCKING);
  connection->setInputStreamIOMode(oatpp::data::stream::IOMode::NON_BLOCKING);

  m_executor->execute<HttpProcessor::Coroutine>(m_components, connection);
  
}

void AsyncHttpConnectionHandler::stop() {
  m_executor->stop();
}
  
}}}
