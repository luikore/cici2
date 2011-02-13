require "cici/ext"

module Cici

  # ease for creating child windows
  module Builder
    def elem *xs, &p
      Elem.new *xs, &p
    end
  end

  # element base
  class Elem
    include Builder

    # windows window_class string
    def window_class
      "canvas"
    end

    def initialize parent, title, size, &p
      Internal.new_child_window self, parent, title, 0, 0, *size
      p[self] if p
    end

    # TODO add mutex for event binding

    # bind callback to event
    # form1:
    #   e.on :click, ->{ app.alert 'hello' }
    # form2:
    #   e.on :click do
    #     app.alert 'hello'
    #   end
    def on event, plambda=nil, &p
      p ||= plambda
      raise "require block or lambda, but got #{p.inspect}" unless p.is_a? Proc
      event = event.to_sym if (event.is_a? String)
      raise 'event should be symbol or string like :click' unless event.is_a? Symbol
      key = Internal.event_to_key event
      @events[key] ||= []
      @events[key] << p
    end

    # symetric to unbind
    alias bind on

    # remove callbacks binded on event
    def unbind event
      event = event.to_sym if (event.is_a? String)
      raise 'event should be symbol or string like :click' unless event.is_a? Symbol
      key = Internal.event_to_key event
      @events.delete key
    end

    # won't call class level processor
    def stub event
      key = Internal.event_to_key event
      @events[key] = []
    end
  end
end

