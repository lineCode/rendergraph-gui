#pragma once
#include "node.h"

namespace render {

	class Observer {
		friend class Node;

	public:
		using Ptr = std::unique_ptr<Observer>;

		Observer(Node *observed, std::function<EventHandler> callback)
			: observed_{ observed }, callback_{ std::move(callback) } {
			observed_->addObserver(this);
		}

		~Observer() { observed_->removeObserver(this); }

		static Ptr make(Node *observed, std::function<EventHandler> callback) {
			return std::make_unique<Observer>(observed, std::move(callback));
		}

	private:
		Node *observed_;
		std::function<EventHandler> callback_;
	};

}