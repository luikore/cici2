# coding: utf-8

module Cici
  # internal processings
  module Internal
    module_function
    Timers = {}

    def register_timer id, p
      Timers[id] = p
    end

    def unregister_timer id
      Timers.delete id
    end

    def fire_timer id
      Timers[id].call
    rescue
      Cici.alert $!
    end
  end
end
