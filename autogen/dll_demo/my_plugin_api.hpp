#pragma once

#include <string>
#include <memory>
#include <functional>

struct Factories {
   std::function<std::string ()> hello;
};

#if 0
#include <string>
#include <memory>

struct SessionApi {
   virtual ~SessionApi() {}
   virtual float calculate() = 0;
};

class Session {
public:
   float calculate() { return pImpl->calculate(); }

   Session(std::shared_ptr<SessionApi> arg) : pImpl(arg) {}

private:
   struct Impl;
   std::shared_ptr<SessionApi> pImpl;

   friend class Publisher;
};

struct PublisherApi {
   virtual ~PublisherApi() {}
   virtual float calculate(std::shared_ptr<SessionApi>&) = 0;
};

class Publisher {
public:
   float calculate(std::shared_ptr<Session> session) { return pImpl->calculate(session->pImpl); }

   Publisher(std::shared_ptr<PublisherApi> arg) : pImpl(arg) {}

private:
   struct Impl;
   std::shared_ptr<PublisherApi> pImpl;
};

struct BOOST_SYMBOL_VISIBLE BoostPlugin {
   virtual std::shared_ptr<SessionApi> session_factory(const std::string& arg) = 0;
   virtual std::shared_ptr<PublisherApi> publisher_factory() = 0;
   virtual ~BoostPlugin() {};
};

#endif